//----------------------------------------------------------------
//
// File: UserStateTest.cpp
//
//----------------------------------------------------------------

#include <controller/UserState.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>
#include <yaml-cpp/yaml.h>
#include <doctest/doctest.h>

using Ctrl::UserState;
namespace fs = std::filesystem;

namespace {

struct StateFixture
{
    fs::path dir;

    StateFixture()
    {
        static int n = 0;
        dir = fs::temp_directory_path() /
              ("royalcraps-statetest-" + std::to_string(::getpid()) +
               "-" + std::to_string(n++));
        fs::remove_all(dir);
        fs::create_directories(dir);
    }

    ~StateFixture() { fs::remove_all(dir); }

    fs::path file() const { return dir / UserState::FileName; }

    void write(const std::string& text) const
    {
        std::ofstream f(file());
        f << text;
    }
};

} // namespace

//----------------------------------------------------------------

TEST_CASE("UserState: save then read round trip")
{
    StateFixture fx;
    UserState s;
    s.table      = "las-vegas-strip.yaml";
    s.userPlayer = "paige.yaml";
    s.buddies    = {"arthur.yaml", "nathan.yaml"};
    s.save(fx.dir);

    UserState got;
    std::string why;
    REQUIRE(UserState::read(fx.file(), got, why));
    CHECK(got.table      == "las-vegas-strip.yaml");
    CHECK(got.userPlayer == "paige.yaml");
    CHECK(got.buddies    == s.buddies);
    CHECK_FALSE(fs::exists(fx.dir / "state.yaml.tmp"));
}

//----------------------------------------------------------------

TEST_CASE("UserState: a game with no buddies round trips as no buddies")
{
    StateFixture fx;
    UserState s;
    s.table      = "las-vegas-strip.yaml";
    s.userPlayer = "paige.yaml";
    s.save(fx.dir);

    UserState got;
    got.buddies = {"stale.yaml"};
    std::string why;
    REQUIRE(UserState::read(fx.file(), got, why));
    CHECK(got.buddies.empty());
}

//----------------------------------------------------------------

TEST_CASE("UserState: animation speed round trips")
{
    StateFixture fx;
    UserState s;
    s.table      = "las-vegas-strip.yaml";
    s.userPlayer = "paige.yaml";

    SUBCASE("each speed")
    {
        for (auto speed : Ctrl::AllAnimationSpeeds)
        {
            s.animationSpeed = speed;
            s.save(fx.dir);
            UserState got;
            std::string why;
            REQUIRE(UserState::read(fx.file(), got, why));
            REQUIRE(got.animationSpeed.has_value());
            CHECK((*got.animationSpeed == speed));
        }
    }
    SUBCASE("never chosen stays absent")
    {
        s.save(fx.dir);
        UserState got;
        got.animationSpeed = Ctrl::AnimationSpeed::Slow;
        std::string why;
        REQUIRE(UserState::read(fx.file(), got, why));
        CHECK_FALSE(got.animationSpeed.has_value());
    }
}

//----------------------------------------------------------------

TEST_CASE("UserState: a bad animation speed is ignored, not fatal")
{
    StateFixture fx;
    fx.write("last:\n  table: a.yaml\n  userPlayer: b.yaml\n  buddyPlayers: []\n"
             "settings:\n  animationSpeed: warp\n");
    UserState got;
    std::string why;
    REQUIRE(UserState::read(fx.file(), got, why));
    CHECK_FALSE(got.animationSpeed.has_value());
    CHECK(got.table == "a.yaml");
}

//----------------------------------------------------------------

TEST_CASE("AnimationSpeed: parsing")
{
    using Ctrl::AnimationSpeed;
    using Ctrl::parseAnimationSpeed;
    CHECK((*parseAnimationSpeed("Fast")   == AnimationSpeed::Fast));
    CHECK((*parseAnimationSpeed(" slow ") == AnimationSpeed::Slow));
    CHECK((*parseAnimationSpeed("normal") == AnimationSpeed::Normal));
    CHECK((*parseAnimationSpeed("off")    == AnimationSpeed::Off));
    CHECK((*parseAnimationSpeed("false")  == AnimationSpeed::Off));
    CHECK_FALSE(parseAnimationSpeed("warp").has_value());
    CHECK_FALSE(parseAnimationSpeed("").has_value());
}

//----------------------------------------------------------------

TEST_CASE("UserState: unusable state files are refused")
{
    StateFixture fx;
    UserState got;
    std::string why;

    SUBCASE("garbage")
    {
        fx.write("last: [unclosed\n  : :\n");
    }
    SUBCASE("no last section")
    {
        fx.write("other: 1\n");
    }
    SUBCASE("no buddy list")
    {
        fx.write("last:\n  table: a.yaml\n  userPlayer: b.yaml\n");
    }
    SUBCASE("format of an earlier version")
    {
        fx.write("table:\n  startTable: a.yaml\n  userIsPlayer: b.yaml\n"
                 "  startPlayers: [b.yaml]\n");
    }

    CHECK_FALSE(UserState::read(fx.file(), got, why));
    CHECK_FALSE(why.empty());
}

//----------------------------------------------------------------
