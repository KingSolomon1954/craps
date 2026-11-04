//----------------------------------------------------------------
//
// File: YamlFileTest.cpp
//
//----------------------------------------------------------------

#include <controller/YamlFile.h>
#include <doctest/doctest.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

using Ctrl::YamlFile;
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
    YamlFile f(p);
    try
    {
        f.load("Player");
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

TEST_CASE("YamlFile:save then load")
{
    TempDir t;

    YAML::Node player;
    player["playerId"]   = "uuid1";
    player["playerName"] = "Nathan";
    YAML::Node prefs;
    prefs["LastFills"] = YAML::Node(YAML::NodeType::Sequence);

    YamlFile out(t.path("a.yaml"));
    out.setSection("Player", player);
    out.setSection("Prefs", prefs);
    out.save();

    CHECK(fs::exists(t.path("a.yaml")));
    CHECK_FALSE(fs::exists(t.path("a.yaml.tmp")));   // temp file is gone

    YamlFile in(t.path("a.yaml"));
    in.load("Player");
    CHECK(in.getSection("Player")["playerName"].as<std::string>() == "Nathan");
    CHECK(in.getSection("Prefs").IsMap());
    CHECK(in.getSection("Prefs")["LastFills"].IsSequence());
}

//----------------------------------------------------------------

TEST_CASE("YamlFile:save twice overwrites")
{
    TempDir t;

    YamlFile f(t.path("a.yaml"));
    YAML::Node player;
    player["n"] = 1;
    f.setSection("Player", player);
    f.save();

    player["n"] = 2;
    f.setSection("Player", player);
    f.save();

    YamlFile in(t.path("a.yaml"));
    in.load("Player");
    CHECK(in.getSection("Player")["n"].as<int>() == 2);
    CHECK_FALSE(fs::exists(t.path("a.yaml.tmp")));
}

//----------------------------------------------------------------

TEST_CASE("YamlFile:sections are replaced, other sections are kept")
{
    TempDir t;
    writeText(t.path("a.yaml"),
        "Player:\n"
        "  playerId: uuid1\n"
        "  oldKey: gone\n"
        "Other:\n"
        "  keep: me\n");

    YamlFile f(t.path("a.yaml"));
    f.load("Player");

    YAML::Node player;
    player["playerId"]   = "uuid1";
    player["playerName"] = "New";
    f.setSection("Player", player);
    f.save();

    const YAML::Node n = YAML::LoadFile(t.path("a.yaml").string());
    CHECK(n["Player"]["playerName"].as<std::string>() == "New");
    CHECK_FALSE(n["Player"]["oldKey"].IsDefined());     // replaced wholesale
    CHECK(n["Other"]["keep"].as<std::string>() == "me"); // untouched
}

//----------------------------------------------------------------

TEST_CASE("YamlFile:sections other than the required one are optional")
{
    TempDir t;
    writeText(t.path("a.yaml"),
        "Player:\n"
        "  playerId: uuid1\n");

    YamlFile f(t.path("a.yaml"));
    f.load("Player");
    CHECK(f.getSection("Player").IsMap());
    CHECK(f.getSection("Prefs").IsNull());
}

//----------------------------------------------------------------

TEST_CASE("YamlFile:required section is whatever the caller names")
{
    TempDir t;
    writeText(t.path("table.yaml"),
        "Table:\n"
        "  tableId: Table-1\n");

    YamlFile f(t.path("table.yaml"));
    CHECK_THROWS_AS(f.load("Player"), std::runtime_error);   // not there
    f.load("Table");
    CHECK(f.getSection("Table")["tableId"].as<std::string>() == "Table-1");
    CHECK(f.getSection("Player").IsNull());                  // missing -> null
}

//----------------------------------------------------------------

TEST_CASE("YamlFile:getters and setters hand out copies")
{
    TempDir t;
    YamlFile f(t.path("a.yaml"));

    YAML::Node player;
    player["a"] = 1;
    f.setSection("Player", player);

    player["a"] = 2;                                    // caller edits its own
    CHECK(f.getSection("Player")["a"].as<int>() == 1);

    YAML::Node got = f.getSection("Player");
    got["a"] = 3;                                       // caller edits the copy
    CHECK(f.getSection("Player")["a"].as<int>() == 1);
}

//----------------------------------------------------------------

TEST_CASE("YamlFile:load failures throw and name the file")
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

TEST_CASE("YamlFile:save failures throw")
{
    TempDir t;
    YAML::Node player;
    player["a"] = 1;

    SUBCASE("no path")
    {
        YamlFile f{fs::path()};
        f.setSection("Player", player);
        CHECK_THROWS_AS(f.save(), std::runtime_error);
    }

    SUBCASE("directory does not exist")
    {
        YamlFile f(t.path("nodir/a.yaml"));
        f.setSection("Player", player);
        CHECK_THROWS_AS(f.save(), std::runtime_error);
        CHECK_FALSE(fs::exists(t.path("nodir")));
    }
}

//----------------------------------------------------------------
