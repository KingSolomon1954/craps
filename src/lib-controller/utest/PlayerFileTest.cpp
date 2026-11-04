//----------------------------------------------------------------
//
// File: PlayerFileTest.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerFile.h>
#include <doctest/doctest.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

using Ctrl::PlayerFile;
namespace fs = std::filesystem;

namespace {

// A scratch directory that goes away when the test is done.
struct TempDir
{
    fs::path dir;

    TempDir()
    {
        dir = fs::temp_directory_path() /
              ("craps-ut-playerfile-" + std::to_string(::getpid()));
        fs::remove_all(dir);
        fs::create_directories(dir);
    }
   ~TempDir()
    {
        std::error_code ec;
        fs::remove_all(dir, ec);
    }
    fs::path path(const std::string& name) const { return dir / name; }
};

void
writeText(const fs::path& p, const std::string& text)
{
    std::ofstream f(p);
    f << text;
}

// True if load() throws runtime_error and the message names the file.
bool
loadFailsNamingFile(const fs::path& p)
{
    PlayerFile f(p);
    try
    {
        f.load();
    }
    catch (const std::runtime_error& e)
    {
        return std::string(e.what()).find(p.filename().string())
               != std::string::npos;
    }
    return false;
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("PlayerFile:save then load")
{
    TempDir t;

    YAML::Node player;
    player["playerId"]   = "uuid1";
    player["playerName"] = "Nathan";
    YAML::Node prefs;
    prefs["LastFills"] = YAML::Node(YAML::NodeType::Sequence);

    PlayerFile out(t.path("a.yaml"));
    out.setPlayerNode(player);
    out.setPrefsNode(prefs);
    out.save();

    CHECK(fs::exists(t.path("a.yaml")));
    CHECK_FALSE(fs::exists(t.path("a.yaml.tmp")));   // temp file is gone

    PlayerFile in(t.path("a.yaml"));
    in.load();
    CHECK(in.getPlayerNode()["playerName"].as<std::string>() == "Nathan");
    CHECK(in.getPrefsNode().IsMap());
    CHECK(in.getPrefsNode()["LastFills"].IsSequence());
}

//----------------------------------------------------------------

TEST_CASE("PlayerFile:save twice overwrites")
{
    TempDir t;

    PlayerFile f(t.path("a.yaml"));
    YAML::Node player;
    player["n"] = 1;
    f.setPlayerNode(player);
    f.save();

    player["n"] = 2;
    f.setPlayerNode(player);
    f.save();

    PlayerFile in(t.path("a.yaml"));
    in.load();
    CHECK(in.getPlayerNode()["n"].as<int>() == 2);
    CHECK_FALSE(fs::exists(t.path("a.yaml.tmp")));
}

//----------------------------------------------------------------

TEST_CASE("PlayerFile:sections are replaced, other sections are kept")
{
    TempDir t;
    writeText(t.path("a.yaml"),
        "Player:\n"
        "  playerId: uuid1\n"
        "  oldKey: gone\n"
        "Other:\n"
        "  keep: me\n");

    PlayerFile f(t.path("a.yaml"));
    f.load();

    YAML::Node player;
    player["playerId"]   = "uuid1";
    player["playerName"] = "New";
    f.setPlayerNode(player);
    f.save();

    const YAML::Node n = YAML::LoadFile(t.path("a.yaml").string());
    CHECK(n["Player"]["playerName"].as<std::string>() == "New");
    CHECK_FALSE(n["Player"]["oldKey"].IsDefined());     // replaced wholesale
    CHECK(n["Other"]["keep"].as<std::string>() == "me"); // untouched
}

//----------------------------------------------------------------

TEST_CASE("PlayerFile:Prefs section is optional")
{
    TempDir t;
    writeText(t.path("a.yaml"),
        "Player:\n"
        "  playerId: uuid1\n");

    PlayerFile f(t.path("a.yaml"));
    f.load();
    CHECK(f.getPlayerNode().IsMap());
    CHECK(f.getPrefsNode().IsNull());
}

//----------------------------------------------------------------

TEST_CASE("PlayerFile:getters and setters hand out copies")
{
    TempDir t;
    PlayerFile f(t.path("a.yaml"));

    YAML::Node player;
    player["a"] = 1;
    f.setPlayerNode(player);

    player["a"] = 2;                                    // caller edits its own
    CHECK(f.getPlayerNode()["a"].as<int>() == 1);

    YAML::Node got = f.getPlayerNode();
    got["a"] = 3;                                       // caller edits the copy
    CHECK(f.getPlayerNode()["a"].as<int>() == 1);
}

//----------------------------------------------------------------

TEST_CASE("PlayerFile:load failures throw and name the file")
{
    TempDir t;

    SUBCASE("file missing")
    {
        CHECK(loadFailsNamingFile(t.path("missing.yaml")));
    }

    SUBCASE("not valid YAML")
    {
        writeText(t.path("bad.yaml"), "Player: [unclosed\n");
        CHECK(loadFailsNamingFile(t.path("bad.yaml")));
    }

    SUBCASE("not a map")
    {
        writeText(t.path("seq.yaml"), "- a\n- b\n");
        CHECK(loadFailsNamingFile(t.path("seq.yaml")));
    }

    SUBCASE("no Player section")
    {
        writeText(t.path("noplayer.yaml"), "Other: 1\n");
        CHECK(loadFailsNamingFile(t.path("noplayer.yaml")));
    }

    SUBCASE("Player section is not a map")
    {
        writeText(t.path("scalar.yaml"), "Player: 5\n");
        CHECK(loadFailsNamingFile(t.path("scalar.yaml")));
    }
}

//----------------------------------------------------------------

TEST_CASE("PlayerFile:save failures throw")
{
    TempDir t;
    YAML::Node player;
    player["a"] = 1;

    SUBCASE("no path")
    {
        PlayerFile f{fs::path()};
        f.setPlayerNode(player);
        CHECK_THROWS_AS(f.save(), std::runtime_error);
    }

    SUBCASE("directory does not exist")
    {
        PlayerFile f(t.path("nodir/a.yaml"));
        f.setPlayerNode(player);
        CHECK_THROWS_AS(f.save(), std::runtime_error);
        CHECK_FALSE(fs::exists(t.path("nodir")));
    }
}

//----------------------------------------------------------------
