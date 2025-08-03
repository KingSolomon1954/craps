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
    PlayerConfig config { "playerFilesDirectory" };
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
    std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
    std::unique_ptr<Player> p2(Player::createPlayer(p2Id, config, em));
    CHECK(p1->getName() == "uuid1");
    CHECK(p2->getName() == "uuid2");
}

//----------------------------------------------------------------


//----------------------------------------------------------------
