//----------------------------------------------------------------
//
// File: PlayerTest.cpp
//
//----------------------------------------------------------------

#include <memory>
#include <craps/Player.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTypes.h>
#include <craps/EventManager.h>
#include <doctest/doctest.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

using namespace Craps;

//----------------------------------------------------------------

struct PlayerFixture
{
    EventManager em;
    PlayerConfig config { "/work/craps/assets/players/Player-1.yaml" };
    PlayerId p1Id { "uuid1" };
    PlayerId p2Id { "uuid2" };

    PlayerFixture()
    {
    }

   ~PlayerFixture()
    {
    }
};

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:ctor")
{
    SUBCASE("via createplayer()")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id, config, em));
        CHECK(p1->getName() == "uuid1");
        CHECK(p2->getName() == "uuid2");
    }

    SUBCASE("via fromConfig()")
    {
        std::unique_ptr<Player> p1(Player::fromConfig(p1Id, config, em));
        std::unique_ptr<Player> p2(Player::fromConfig(p2Id, config, em));
        CHECK(p1->getPlayerId() == "uuid1");
        CHECK(p2->getPlayerId() == "uuid2");
    }

    SUBCASE("fromFile:missing")
    {
        // Clobber path. Use bad playerId/path so file won't be found.
        PlayerFixture::config.playerPath = "missing/FakePlayer-1";
        CHECK_THROWS_AS(Player::fromFile("uuid1", config, em),
                        std::runtime_error);
    }

    SUBCASE("via fromFile()")
    {
#if 0        
        std::unique_ptr<Player> p1(Player::fromFile(p1Id, config, em));
        std::unique_ptr<Player> p2(Player::fromFile(p2Id, config, em));
        CHECK(p1->getPlayerId() == "uuid1");
        CHECK(p2->getPlayerId() == "uuid2");
#endif        
    }
}

//----------------------------------------------------------------


//----------------------------------------------------------------
