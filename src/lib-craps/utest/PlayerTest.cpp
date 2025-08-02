//----------------------------------------------------------------
//
// File: PlayerTest.cpp
//
//----------------------------------------------------------------

#include <craps/Player.h>
#include <craps/CrapsBet.h>
#include <craps/EventManager.h>
#include <doctest/doctest.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

using namespace Craps;

//----------------------------------------------------------------

struct PlayerFixture
{
    EventManager em;
    
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
    Player p1("Sam", 1000, em);
    CHECK(p1.getName() == "Sam");
}

//----------------------------------------------------------------


//----------------------------------------------------------------
