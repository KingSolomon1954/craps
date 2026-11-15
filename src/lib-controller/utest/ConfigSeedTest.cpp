//----------------------------------------------------------------
//
// File: ConfigSeedTest.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigSeed.h>
#include <controller/ConfigManager.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <gen/MultiLayerConfig.h>
#include <doctest/doctest.h>

using Ctrl::ConfigSeed;
using K = Ctrl::ConfigManager;
namespace fs = std::filesystem;

namespace {

void
writeFile(const fs::path& p, const std::string& text)
{
    fs::create_directories(p.parent_path());
    std::ofstream f(p);
    f << text;
}

std::string
readFile(const fs::path& p)
{
    std::ifstream f(p);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// A packaged tree plus empty user locations, all under one temp root.
struct SeedFixture
{
    fs::path root;
    fs::path sys;
    fs::path usr;
    Gen::MultiLayerConfig cfg;

    SeedFixture()
    {
        static int n = 0;
        root = fs::temp_directory_path() /
               ("royalcraps-seedtest-" + std::to_string(::getpid()) +
                "-" + std::to_string(n++));
        fs::remove_all(root);
        sys = root / "sys";
        usr = root / "usr";

        writeFile(sys / "config"  / "default-config.yaml", "table:\n  x: 1\n");
        writeFile(sys / "config"  / "sys-config.yaml",     "sound.enabled: true\n");
        writeFile(sys / "tables"  / "tables.yaml",         "tables: []\n");
        writeFile(sys / "tables"  / "t1.yaml",             "Table: {}\n");
        writeFile(sys / "players" / "players.yaml",        "players: []\n");
        writeFile(sys / "players" / "p1.yaml",             "packaged p1\n");
        writeFile(sys / "players" / ".gitkeep",            "");
        fs::create_directories(sys / "audio");
        writeFile(sys / "images"  / "logo.png",            "png");

        cfg.addLayer("defaults", Gen::ConfigLayer());
        setDirs(sys, usr);
        fs::create_directories(usr / "config");
    }

    ~SeedFixture() { fs::remove_all(root); }

    void setDirs(const fs::path& s, const fs::path& u)
    {
        auto set = [&](const char* k, const fs::path& v)
            { cfg.set("defaults", k, v.string()); };
        set(K::KeyDirsSysConfig,  s / "config");
        set(K::KeyDirsSysTables,  s / "tables");
        set(K::KeyDirsSysPlayers, s / "players");
        set(K::KeyDirsSysAudio,   s / "audio");
        set(K::KeyDirsSysImages,  s / "images");
        set(K::KeyDirsUsrConfig,  u / "config");
        set(K::KeyDirsUsrTables,  u / "tables");
        set(K::KeyDirsUsrPlayers, u / "players");
        set(K::KeyDirsUsrAudio,   u / "audio");
        set(K::KeyDirsUsrImages,  u / "images");
    }
};

} // namespace

//----------------------------------------------------------------

TEST_CASE("ConfigSeed: first run copies packaged files")
{
    SeedFixture fx;

    CHECK(ConfigSeed::isFirstRun(fx.cfg));
    CHECK(ConfigSeed::needsSeeding(fx.cfg));
    CHECK(ConfigSeed::seedUserFiles(fx.cfg));

    CHECK(fs::exists(fx.usr / "config"  / "config.yaml"));
    CHECK(readFile(fx.usr / "config" / "config.yaml") == "table:\n  x: 1\n");
    CHECK(fs::exists(fx.usr / "tables"  / "tables.yaml"));
    CHECK(fs::exists(fx.usr / "tables"  / "t1.yaml"));
    CHECK(fs::exists(fx.usr / "players" / "players.yaml"));
    CHECK(fs::exists(fx.usr / "players" / "p1.yaml"));
    CHECK(fs::exists(fx.usr / "images"  / "logo.png"));
    CHECK(fs::is_directory(fx.usr / "audio"));

    // Dot files are not copied, and sys-config.yaml is not a user file.
    CHECK_FALSE(fs::exists(fx.usr / "players" / ".gitkeep"));
    CHECK_FALSE(fs::exists(fx.usr / "config"  / "sys-config.yaml"));

    // No temporary files left behind
    for (const auto& e : fs::recursive_directory_iterator(fx.usr))
    {
        CHECK(e.path().extension() != ".tmp");
    }
}

//----------------------------------------------------------------

TEST_CASE("ConfigSeed: second startup does nothing")
{
    SeedFixture fx;
    REQUIRE(ConfigSeed::seedUserFiles(fx.cfg));

    // The player plays and is saved
    writeFile(fx.usr / "players" / "p1.yaml", "my stats\n");
    // User removes a table file; with all markers present it stays gone
    fs::remove(fx.usr / "tables" / "t1.yaml");

    CHECK_FALSE(ConfigSeed::isFirstRun(fx.cfg));
    CHECK_FALSE(ConfigSeed::needsSeeding(fx.cfg));
    CHECK_FALSE(ConfigSeed::seedUserFiles(fx.cfg));

    CHECK(readFile(fx.usr / "players" / "p1.yaml") == "my stats\n");
    CHECK_FALSE(fs::exists(fx.usr / "tables" / "t1.yaml"));
}

//----------------------------------------------------------------

TEST_CASE("ConfigSeed: never overwrites existing files")
{
    SeedFixture fx;

    // Config deleted, so seeding runs again, but the player file exists.
    fs::create_directories(fx.usr / "players");
    writeFile(fx.usr / "players" / "p1.yaml", "my stats\n");

    CHECK(ConfigSeed::seedUserFiles(fx.cfg));
    CHECK(readFile(fx.usr / "players" / "p1.yaml") == "my stats\n");
    CHECK(fs::exists(fx.usr / "players" / "players.yaml"));
}

//----------------------------------------------------------------

TEST_CASE("ConfigSeed: missing user manifest triggers reseeding")
{
    SeedFixture fx;
    REQUIRE(ConfigSeed::seedUserFiles(fx.cfg));
    fs::remove(fx.usr / "players" / "players.yaml");

    CHECK(ConfigSeed::needsSeeding(fx.cfg));
    CHECK_FALSE(ConfigSeed::seedUserFiles(fx.cfg));  // not a first run
    CHECK(fs::exists(fx.usr / "players" / "players.yaml"));
}

//----------------------------------------------------------------

TEST_CASE("ConfigSeed: missing packaged default config throws")
{
    SeedFixture fx;
    fs::remove(fx.sys / "config" / "default-config.yaml");

    CHECK_THROWS_AS(ConfigSeed::seedUserFiles(fx.cfg), std::runtime_error);
    CHECK_FALSE(fs::exists(fx.usr / "config" / "config.yaml"));
}

//----------------------------------------------------------------

TEST_CASE("ConfigSeed: same directories are left alone")
{
    SeedFixture fx;
    // Development: user directories are the packaged ones
    fx.setDirs(fx.sys, fx.sys);
    writeFile(fx.sys / "config" / "config.yaml", "dev\n");

    CHECK_FALSE(ConfigSeed::seedUserFiles(fx.cfg));
    CHECK(readFile(fx.sys / "config" / "config.yaml") == "dev\n");
    CHECK(readFile(fx.sys / "players" / "p1.yaml") == "packaged p1\n");
}

//----------------------------------------------------------------
