//----------------------------------------------------------------
//
// File: UserStateTest.cpp
//
//----------------------------------------------------------------

#include <controller/UserState.h>
#include <controller/ConfigManager.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>
#include <gen/MultiLayerConfig.h>
#include <yaml-cpp/yaml.h>
#include <doctest/doctest.h>

using Ctrl::UserState;
using K = Ctrl::ConfigManager;
namespace fs = std::filesystem;

namespace {

void
touch(const fs::path& p, const std::string& text = "x")
{
    fs::create_directories(p.parent_path());
    std::ofstream f(p);
    f << text;
}

struct StateFixture
{
    fs::path root;
    fs::path tables;
    fs::path players;
    fs::path stateDir;
    Gen::MultiLayerConfig cfg;

    StateFixture()
    {
        static int n = 0;
        root = fs::temp_directory_path() /
               ("royalcraps-statetest-" + std::to_string(::getpid()) +
                "-" + std::to_string(n++));
        fs::remove_all(root);
        tables   = root / "tables";
        players  = root / "players";
        stateDir = root / "state";
        touch(tables  / "las-vegas-strip.yaml");
        touch(players / "arthur.yaml");
        touch(players / "paige.yaml");
        fs::create_directories(stateDir);

        cfg.addLayer("defaults", Gen::ConfigLayer());
        cfg.set("defaults", K::KeyDirsUsrState,   stateDir.string());
        cfg.set("defaults", K::KeyDirsUsrTables,  tables.string());
        cfg.set("defaults", K::KeyDirsUsrPlayers, players.string());
    }

    ~StateFixture() { fs::remove_all(root); }

    UserState good() const
    {
        UserState s;
        s.table      = "las-vegas-strip.yaml";
        s.userPlayer = "paige.yaml";
        s.players    = {"paige.yaml", "arthur.yaml"};
        return s;
    }
};

} // namespace

//----------------------------------------------------------------

TEST_CASE("UserState: save then read round trip")
{
    StateFixture fx;
    fx.good().save(fx.stateDir);

    UserState got;
    std::string why;
    REQUIRE(UserState::read(fx.stateDir / UserState::FileName,
                            fx.tables, fx.players, got, why));
    CHECK(got.table      == "las-vegas-strip.yaml");
    CHECK(got.userPlayer == "paige.yaml");
    REQUIRE(got.players.size() == 2);
    CHECK(got.players[0] == "paige.yaml");
    CHECK(got.players[1] == "arthur.yaml");
    CHECK_FALSE(fs::exists(fx.stateDir / "state.yaml.tmp"));
}

//----------------------------------------------------------------

TEST_CASE("UserState: loadInto sets the start keys")
{
    StateFixture fx;
    fx.good().save(fx.stateDir);

    Gen::ConfigLayer layer;
    UserState::loadInto(fx.cfg, layer);

    CHECK(layer.get(K::KeyTableStart).value()        == "las-vegas-strip.yaml");
    CHECK(layer.get(K::KeyTableUserIsPlayer).value() == "paige.yaml");
    CHECK(layer.get(K::KeyTablePlayer1).value()      == "paige.yaml");
    CHECK(layer.get(K::KeyTablePlayer2).value()      == "arthur.yaml");
    CHECK(layer.get(K::KeyTablePlayer3).value()      == "");
    CHECK(layer.get(K::KeyTablePlayer6).value()      == "");
}

//----------------------------------------------------------------

TEST_CASE("UserState: no state file leaves the layer untouched")
{
    StateFixture fx;
    Gen::ConfigLayer layer;
    UserState::loadInto(fx.cfg, layer);
    CHECK(layer.getAll().empty());
}

//----------------------------------------------------------------

TEST_CASE("UserState: unusable state files are ignored")
{
    StateFixture fx;
    const fs::path file = fx.stateDir / UserState::FileName;
    UserState got;
    std::string why;

    SUBCASE("garbage")
    {
        touch(file, "table: [unclosed\n  : :\n");
    }
    SUBCASE("table file is gone")
    {
        UserState s = fx.good();
        s.table = "gone.yaml";
        s.save(fx.stateDir);
    }
    SUBCASE("player file is gone")
    {
        UserState s = fx.good();
        s.players.push_back("deleted.yaml");
        s.save(fx.stateDir);
    }
    SUBCASE("user player is not at the table")
    {
        UserState s = fx.good();
        s.userPlayer = "nobody.yaml";
        s.save(fx.stateDir);
    }
    SUBCASE("missing sections")
    {
        touch(file, "other: 1\n");
    }

    CHECK_FALSE(UserState::read(file, fx.tables, fx.players, got, why));
    CHECK_FALSE(why.empty());

    Gen::ConfigLayer layer;
    UserState::loadInto(fx.cfg, layer);
    CHECK(layer.getAll().empty());
}

//----------------------------------------------------------------

TEST_CASE("UserState: only six players are written")
{
    StateFixture fx;
    UserState s = fx.good();
    s.players = {"a", "b", "c", "d", "e", "f", "g"};
    s.save(fx.stateDir);

    const YAML::Node root =
        YAML::LoadFile((fx.stateDir / UserState::FileName).string());
    CHECK(root["table"]["startPlayers"].size() == 6);
}

//----------------------------------------------------------------
