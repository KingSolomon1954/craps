//----------------------------------------------------------------
//
// File: CrapsBetTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <stdexcept> // for std::invalid_argument
#include <chrono>
#include "CrapsBet.h"
#include "Dice.h"
#include "EnumBetName.h"

using namespace App;

//----------------------------------------------------------------

TEST_CASE("CrapsBet::Constructor")
{
    SUBCASE("Good Args")
    {
        CrapsBet b(BetName::PassLine, 100);
        CHECK(b.betId() > 0);
        CHECK(b.betName() == BetName::PassLine);
        CHECK(b.pivot() == 0);
        CHECK(b.contractAmount() == 100);
        CHECK(b.oddsAmount() == 0);
        CHECK(b.oddsOffComeOutRoll());
        CHECK(b.distance() == 0);
        CHECK(b.whenCreated() < std::chrono::system_clock::now());

        CrapsBet b1(BetName::PassLine, 100);
        CHECK(b1.betId() > 0);
        CHECK(b.betId() != b1.betId());
        CHECK(b.whenCreated() != b1.whenCreated());

        CrapsBet b2(BetName::Place, 100, 4);
        CHECK(b2.pivot() == 4);
    }

    
    SUBCASE("CopyMove Operator")
    {
        // Copy
        CrapsBet b(BetName::PassLine, 100);
        CrapsBet b1(b);
        CHECK(b1 == b);

        // Assignment
        CrapsBet b2(BetName::PassLine, 100);
        CrapsBet b3 = b2;
        CHECK(b3 == b2);
        
        // Move 
        CrapsBet b4(BetName::DontPass, 100);
        CrapsBet b5(BetName::DontCome, 100);
        unsigned b5Id = b5.betId();
        b4 = std::move(b5);  // This triggers the move assignment operator
        CHECK(b4.betName() == BetName::DontCome);
        CHECK(b4.betId() == b5Id);
    }
    
    SUBCASE("Bad Args")
    {
        CHECK_THROWS_AS(CrapsBet b(BetName::Invalid, 100),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 0),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 100, 4),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Place, 100, 7),
                                   std::invalid_argument);
    }
    
    // REQUIRE(true);
}

//----------------------------------------------------------------
