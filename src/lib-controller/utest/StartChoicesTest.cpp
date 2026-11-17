//----------------------------------------------------------------
//
// File: StartChoicesTest.cpp
//
//----------------------------------------------------------------

#include <controller/StartChoices.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <doctest/doctest.h>

using Ctrl::StartChoices;
using Ctrl::UserState;
namespace fs = std::filesystem;
using Names = std::vector<std::string>;

namespace {

struct ChoiceFixture
{
    fs::path root;
    StartChoices::Request req;

    ChoiceFixture()
    {
        static int n = 0;
        root = fs::temp_directory_path() /
               ("royalcraps-choicetest-" + std::to_string(::getpid()) +
                "-" + std::to_string(n++));
        fs::remove_all(root);
        fs::create_directories(root / "tables");
        fs::create_directories(root / "players");
        for (auto t : {"las-vegas-strip.yaml", "atlantic-city.yaml"})
            touch(root / "tables" / t);
        for (auto p : {"unnamed.yaml", "arthur.yaml", "paige.yaml",
                       "nathan.yaml", "lenny.yaml", "winnifred.yaml",
                       "a.yaml", "b.yaml", "c.yaml", "d.yaml", "e.yaml"})
            touch(root / "players" / p);

        req.tablesDir         = root / "tables";
        req.playersDir        = root / "players";
        req.defaultTable      = "las-vegas-strip.yaml";
        req.defaultUserPlayer = "unnamed.yaml";
        req.defaultBuddies    = {"arthur.yaml", "paige.yaml"};
    }

    ~ChoiceFixture() { fs::remove_all(root); }

    static void touch(const fs::path& p)
    {
        std::ofstream f(p);
        f << "x";
    }

    UserState last(const std::string& user, Names buddies,
                   const std::string& table = "atlantic-city.yaml") const
    {
        UserState s;
        s.table      = table;
        s.userPlayer = user;
        s.buddies    = std::move(buddies);
        return s;
    }
};

} // namespace

//----------------------------------------------------------------

TEST_CASE("StartChoices: fresh install uses the packaged defaults")
{
    ChoiceFixture fx;
    auto r = StartChoices::resolve(fx.req);
    CHECK(r.table      == "las-vegas-strip.yaml");
    CHECK(r.userPlayer == "unnamed.yaml");
    CHECK(r.buddies    == Names{"arthur.yaml", "paige.yaml"});
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: state fills in what config.yaml leaves out")
{
    ChoiceFixture fx;
    fx.req.last = fx.last("nathan.yaml", {"lenny.yaml"});

    auto r = StartChoices::resolve(fx.req);
    CHECK(r.table      == "atlantic-city.yaml");
    CHECK(r.userPlayer == "nathan.yaml");
    CHECK(r.buddies    == Names{"lenny.yaml"});
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: config.yaml wins, setting by setting")
{
    ChoiceFixture fx;
    fx.req.last        = fx.last("nathan.yaml", {"lenny.yaml"});
    fx.req.userPlayer  = "arthur.yaml";

    auto r = StartChoices::resolve(fx.req);
    CHECK(r.userPlayer == "arthur.yaml");            // from config
    CHECK(r.table      == "atlantic-city.yaml");     // from state
    CHECK(r.buddies    == Names{"lenny.yaml"});      // from state

    fx.req.table   = "las-vegas-strip.yaml";
    fx.req.buddies = Names{"paige.yaml", "winnifred.yaml"};
    r = StartChoices::resolve(fx.req);
    CHECK(r.table   == "las-vegas-strip.yaml");
    CHECK(r.buddies == Names{"paige.yaml", "winnifred.yaml"});

    fx.req.useBuddies = true;                        // Same as absent
    CHECK(StartChoices::resolve(fx.req).buddies ==
          Names{"paige.yaml", "winnifred.yaml"});
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: startWithBuddyPlayers false means no buddies")
{
    ChoiceFixture fx;
    fx.req.useBuddies = false;

    SUBCASE("with nothing remembered")
    {
        CHECK(StartChoices::resolve(fx.req).buddies.empty());
    }
    SUBCASE("though the last game had buddies")
    {
        fx.req.last = fx.last("nathan.yaml", {"lenny.yaml", "paige.yaml"});
        auto r = StartChoices::resolve(fx.req);
        CHECK(r.buddies.empty());
        CHECK(r.userPlayer == "nathan.yaml");
    }
    SUBCASE("though config.yaml lists some")
    {
        fx.req.buddies = Names{"paige.yaml"};
        CHECK(StartChoices::resolve(fx.req).buddies.empty());
    }
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: a remembered game with no buddies stays that way")
{
    ChoiceFixture fx;
    fx.req.last = fx.last("nathan.yaml", {});
    CHECK(StartChoices::resolve(fx.req).buddies.empty());
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: names that are not files are skipped")
{
    ChoiceFixture fx;
    fx.req.last = fx.last("nathan.yaml", {"lenny.yaml", "gone.yaml"});

    SUBCASE("user player in config.yaml is missing: next source")
    {
        fx.req.userPlayer = "typo.yaml";
        CHECK(StartChoices::resolve(fx.req).userPlayer == "nathan.yaml");
    }
    SUBCASE("user player in state is missing too: packaged default")
    {
        fx.req.userPlayer = "typo.yaml";
        fx.req.last->userPlayer = "deleted.yaml";
        CHECK(StartChoices::resolve(fx.req).userPlayer == "unnamed.yaml");
    }
    SUBCASE("table in config.yaml is missing: next source")
    {
        fx.req.table = "typo.yaml";
        CHECK(StartChoices::resolve(fx.req).table == "atlantic-city.yaml");
    }
    SUBCASE("a missing buddy is dropped, the rest stay")
    {
        CHECK(StartChoices::resolve(fx.req).buddies == Names{"lenny.yaml"});
    }
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: buddies never repeat or include the user player")
{
    ChoiceFixture fx;
    fx.req.userPlayer = "arthur.yaml";
    fx.req.buddies = Names{"arthur.yaml", "paige.yaml", "paige.yaml",
                           "lenny.yaml", ""};

    auto r = StartChoices::resolve(fx.req);
    CHECK(r.buddies == Names{"paige.yaml", "lenny.yaml"});
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: at most five buddies")
{
    ChoiceFixture fx;
    fx.req.buddies = Names{"a.yaml", "b.yaml", "c.yaml", "d.yaml",
                           "e.yaml", "lenny.yaml"};

    auto r = StartChoices::resolve(fx.req);
    CHECK(r.buddies.size() == UserState::MaxBuddies);
    CHECK(r.buddies.back() == "e.yaml");
}

//----------------------------------------------------------------

TEST_CASE("StartChoices: kinds keep buddies out of the user slot and back")
{
    using Ctrl::PlayerKind;
    ChoiceFixture fx;
    fx.req.kinds = {
        {"unnamed.yaml", PlayerKind::User},  {"nathan.yaml", PlayerKind::User},
        {"arthur.yaml",  PlayerKind::Buddy}, {"paige.yaml",  PlayerKind::Buddy},
        {"lenny.yaml",   PlayerKind::Buddy} };

    SUBCASE("a buddy named as the user player is skipped")
    {
        fx.req.userPlayer = "arthur.yaml";
        auto r = StartChoices::resolve(fx.req);
        CHECK(r.userPlayer == "unnamed.yaml");       // falls to the default
        CHECK(r.buddies == Names{"arthur.yaml", "paige.yaml"});
    }
    SUBCASE("a user player named as a buddy is skipped")
    {
        fx.req.userPlayer = "nathan.yaml";
        fx.req.buddies    = Names{"unnamed.yaml", "lenny.yaml"};
        auto r = StartChoices::resolve(fx.req);
        CHECK(r.userPlayer == "nathan.yaml");
        CHECK(r.buddies == Names{"lenny.yaml"});
    }
    SUBCASE("a file missing from the manifest is skipped")
    {
        fx.req.buddies = Names{"winnifred.yaml", "paige.yaml"};
        auto r = StartChoices::resolve(fx.req);
        CHECK(r.buddies == Names{"paige.yaml"});
    }
}

//----------------------------------------------------------------
