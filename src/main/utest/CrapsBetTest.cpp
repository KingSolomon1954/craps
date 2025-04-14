//----------------------------------------------------------------
//
// File: CrapsBetTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <stdexcept> // for std::invalid_argument
#include <chrono>
#include "gen/ErrorPass.h"
#include "gen/ReturnCode.h"
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

        CrapsBet b6(BetName::Field, 100);
        CrapsBet b7(BetName::Field, 100, 55);
        CrapsBet b8(BetName::AnyCraps, 100);
        CrapsBet b9(BetName::AnyCraps, 100, 55);
        CrapsBet b10(BetName::CandE, 100);
        CrapsBet b11(BetName::CandE, 100, 55);

        CrapsBet b12(BetName::Hardway, 100, 4);
        CrapsBet b13(BetName::Hardway, 100, 6);
        CrapsBet b14(BetName::Hardway, 100, 8);
        CrapsBet b15(BetName::Hardway, 100, 10);
    }
    
    SUBCASE("Bad Args")
    {
        CHECK_THROWS_AS(CrapsBet b(BetName::Invalid, 100),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 0),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::PassLine, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b(BetName::Come, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Come, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Come, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Come, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Come, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b(BetName::DontPass, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontPass, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontPass, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontPass, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontPass, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b(BetName::DontCome, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontCome, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontCome, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontCome, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::DontCome, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b(BetName::Place, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Place, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Place, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Place, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Place, 100, 12),
                                   std::invalid_argument);

        CHECK_THROWS_AS(CrapsBet b(BetName::Buy, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Buy, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Buy, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Buy, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Buy, 100, 12),
                                   std::invalid_argument);

        CHECK_THROWS_AS(CrapsBet b(BetName::Lay, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Lay, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Lay, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Lay, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Lay, 100, 12),
                                   std::invalid_argument);

        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 5),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 9),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 12),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b(BetName::Hardway, 100, 99),
                                   std::invalid_argument);
    }
    
    // REQUIRE(true);
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet::evaluate()")
{
    SUBCASE("Bad Args")
    {
        Gen::ErrorPass ep;
        Dice dice;
        CrapsBet::DecisionRecord dr;
        unsigned point = 7;
        CrapsBet b(BetName::Field, 100);
        CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);
        point = 99;
        CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);
    }
    SUBCASE("PassLine")
    {
        Gen::ErrorPass ep;
        Dice dice;
        dice.set(2,2);
        
        CrapsBet::DecisionRecord dr;
        unsigned point = 0;
        CrapsBet b(BetName::PassLine, 100);
        CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    }
}

//----------------------------------------------------------------

