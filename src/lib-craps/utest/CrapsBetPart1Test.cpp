//----------------------------------------------------------------
//
// File: CrapsBetPart1Test.cpp
//
// Part 1 does these tests
//   * Constructor
//   * PassLine
//   * Come    
//   * DontPass
//   * DontCome
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <stdexcept> // for std::invalid_argument
#include <chrono>
#include <iostream>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <craps/CrapsBet.h>
#include <craps/Dice.h>
#include <craps/EnumBetName.h>
#include <craps/DecisionRecord.h>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("CrapsBet:Constructor")
{
    SUBCASE("Good Args")
    {
        
        CrapsBet b("Player1", BetName::PassLine, 100);
        CHECK(b.betId() > 0);
        CHECK(b.betName() == BetName::PassLine);
        CHECK(b.pivot() == 0);
        CHECK(b.contractAmount() == 100);
        CHECK(b.oddsAmount() == 0);
        CHECK(b.offComeOutRoll());
        CHECK(b.distance() == 0);
        CHECK(b.whenCreated().hasExpired());

        CrapsBet b1("Player1", BetName::PassLine, 100);
        CHECK(b1.betId() > 0);
        CHECK(b.betId() != b1.betId());
        CHECK(b.whenCreated() != b1.whenCreated());

        CrapsBet b2("Player1", BetName::Place, 100, 4);
        CHECK(b2.pivot() == 4);
    }

    SUBCASE("CopyMove Operator")
    {
        // Copy
        CrapsBet b("Player1", BetName::PassLine, 100);
        CrapsBet b1(b);
        CHECK(b1 == b);

        // Assignment
        CrapsBet b2("Player1", BetName::PassLine, 100);
        CrapsBet b3 = b2;
        CHECK(b3 == b2);
        
        // Move 
        CrapsBet b4("Player1", BetName::DontPass, 100);
        CrapsBet b5("Player1", BetName::DontCome, 100);
        unsigned b5Id = b5.betId();
        b4 = std::move(b5);  // This triggers the move assignment operator
        CHECK(b4.betName() == BetName::DontCome);
        CHECK(b4.betId() == b5Id);

        CrapsBet b6("Player1", BetName::Field, 100);
        CrapsBet b7("Player1", BetName::Field, 100, 55);
        CrapsBet b8("Player1", BetName::AnyCraps, 100);
        CrapsBet b9("Player1", BetName::AnyCraps, 100, 55);
        CrapsBet b10("Player1", BetName::CandE, 100);
        CrapsBet b11("Player1", BetName::CandE, 100, 55);

        CrapsBet b12("Player1", BetName::Hardway, 100, 4);
        CrapsBet b13("Player1", BetName::Hardway, 100, 6);
        CrapsBet b14("Player1", BetName::Hardway, 100, 8);
        CrapsBet b15("Player1", BetName::Hardway, 100, 10);
    }
    
    SUBCASE("Bad Args")
    {
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Invalid, 100),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::PassLine, 0),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::PassLine, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::PassLine, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::PassLine, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::PassLine, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::PassLine, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Come, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Come, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Come, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Come, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Come, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontPass, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontPass, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontPass, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontPass, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontPass, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontCome, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontCome, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontCome, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontCome, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::DontCome, 100, 12),
                                   std::invalid_argument);
        
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Place, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Place, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Place, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Place, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Place, 100, 12),
                                   std::invalid_argument);

        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Buy, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Buy, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Buy, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Buy, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Buy, 100, 12),
                                   std::invalid_argument);

        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Lay, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Lay, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Lay, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Lay, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Lay, 100, 12),
                                   std::invalid_argument);

        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 2),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 3),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 5),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 7),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 9),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 11),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 12),
                                   std::invalid_argument);
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Hardway, 100, 99),
                                   std::invalid_argument);
    }
    
    // REQUIRE(true);
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:args")
{
    SUBCASE("Bad Args")
    {
        Gen::ErrorPass ep;
        Dice dice;
        DecisionRecord dr;

        // bad point
        unsigned point = 7;
        CrapsBet b("Player1", BetName::Field, 100);
        CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);
        
        // bad point
        point = 99;
        CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);
    }
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:PassLine")
{
    SUBCASE("Come Out Roll")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Come out roll, dice = 2, loses
        CrapsBet b21("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // Come out roll, dice = 3, loses
        CrapsBet b31("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // Come out roll, dice = 4, keep
        CrapsBet b41("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Come out roll, dice = 5, keep
        CrapsBet b51("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(4,1);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Come out roll, dice = 6, keep
        CrapsBet b61("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Come out roll, dice = 7, wins
        CrapsBet b71("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // Come out roll, dice = 8, keep
        CrapsBet b81("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Come out roll, dice = 9, keep
        CrapsBet b91("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Come out roll, dice = 10, keep
        CrapsBet b101("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Come out roll, dice = 11, wins
        CrapsBet b111("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // Come out roll, dice = 12, loses
        CrapsBet b121("Player1", BetName::PassLine, 100);
        point = 0;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }
    
    SUBCASE("number hits before 7")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Point of 4 wins (pivot == 0)
        CrapsBet b41("Player1", BetName::PassLine, 100);
        point = 4;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Point of 4 wins (pivot == 4)
        CrapsBet b42("Player1", BetName::PassLine, 100, 4);
        CHECK(b42.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(2,2);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 500);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());
        CHECK(b42.pivot() == 4);

        // Point of 5 wins (pivot == 0)
        CrapsBet b51("Player1", BetName::PassLine, 100);
        point = 5;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Point of 5 wins (pivot == 5)
        CrapsBet b52("Player1", BetName::PassLine, 100, 5);
        CHECK(b52.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,2);
        CHECK(b52.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 400);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b52.distance() == 1);
        CHECK(b52.whenDecided() > b52.whenCreated());
        CHECK(b52.pivot() == 5);

        // Point of 6 wins (pivot == 0)
        CrapsBet b61("Player1", BetName::PassLine, 100);
        point = 6;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Point of 6 wins (pivot == 6)
        CrapsBet b62("Player1", BetName::PassLine, 100, 6);
        CHECK(b62.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,3);
        CHECK(b62.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 340);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b62.distance() == 1);
        CHECK(b62.whenDecided() > b62.whenCreated());
        CHECK(b62.pivot() == 6);

        // Point of 8 wins (pivot == 0)
        CrapsBet b81("Player1", BetName::PassLine, 100);
        point = 8;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Point of 8 wins (pivot == 6)
        CrapsBet b82("Player1", BetName::PassLine, 100, 8);
        CHECK(b82.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 8;
        dice.set(4,4);
        CHECK(b82.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 340);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b82.distance() == 1);
        CHECK(b82.whenDecided() > b82.whenCreated());
        CHECK(b82.pivot() == 8);

        // Point of 9 wins (pivot == 0)
        CrapsBet b91("Player1", BetName::PassLine, 100);
        point = 9;
        dice.set(6,3);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Point of 9 wins (pivot == 9)
        CrapsBet b92("Player1", BetName::PassLine, 100, 9);
        CHECK(b92.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 9;
        dice.set(6,3);
        CHECK(b92.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 400);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b92.distance() == 1);
        CHECK(b92.whenDecided() > b92.whenCreated());
        CHECK(b92.pivot() == 9);

        // Point of 10 wins (pivot == 0)
        CrapsBet b101("Player1", BetName::PassLine, 100);
        point = 10;
        dice.set(6,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Point of 10 wins (pivot == 10)
        CrapsBet b102("Player1", BetName::PassLine, 100, 10);
        CHECK(b102.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 10;
        dice.set(6,4);
        CHECK(b102.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 500);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b102.distance() == 1);
        CHECK(b102.whenDecided() > b102.whenCreated());
        CHECK(b102.pivot() == 10);
    }
    
    SUBCASE("7 hits before number")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Point of 4, dice = 7, loses
        CrapsBet b41("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(3,4);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Point of 5, dice = 7, loses
        CrapsBet b51("Player1", BetName::PassLine, 100, 5);
        point = 5;
        dice.set(3,4);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Point of 6, dice = 7, loses
        CrapsBet b61("Player1", BetName::PassLine, 100, 6);
        point = 6;
        dice.set(3,4);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // Point of 8, dice = 7, loses
        CrapsBet b81("Player1", BetName::PassLine, 100, 8);
        point = 8;
        dice.set(3,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // Point of 9, dice = 7, loses
        CrapsBet b91("Player1", BetName::PassLine, 100, 9);
        point = 9;
        dice.set(3,4);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // Point of 10, dice = 7, loses
        CrapsBet b101("Player1", BetName::PassLine, 100, 10);
        point = 10;
        dice.set(3,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
    }
    
    SUBCASE("No decision")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // After point, dice = 2, keep
        CrapsBet b21("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // After point, dice = 3, keep
        CrapsBet b31("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // After point, dice = 4, keep
        CrapsBet b41("Player1", BetName::PassLine, 100, 10);
        point = 10;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // After point, dice = 5, keep
        CrapsBet b51("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // After point, dice = 6, keep
        CrapsBet b61("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // After point, dice = 7, lose
        CrapsBet b71("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // After point, dice = 8, keep
        CrapsBet b81("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // After point, dice = 9, keep
        CrapsBet b91("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // After point, dice = 10 keep
        CrapsBet b101("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());

        // After point, dice = 11 keep
        CrapsBet b111("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // After point, dice = 12 keep
        CrapsBet b121("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("Distance")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // PassLine bet of 4, roll 10 times no decision
        // On eleventh roll, make point, win
        CrapsBet b41("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(6,6);

        for (unsigned i = 1; i <= 10; ++i)
        {
            CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        }
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 11);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // PassLine bet of 4, roll 10 times no decision
        // On eleventh roll, 7-out, lose
        CrapsBet b42("Player1", BetName::PassLine, 100, 4);
        point = 4;
        dice.set(6,2);

        for (unsigned i = 1; i <= 10; ++i)
        {
            CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        }
        dice.set(3,4);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 11);
        CHECK(b42.whenDecided() > b42.whenCreated());
    }

    SUBCASE("Minimum amount bets")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Point of 4 wins, min contract, min odds 
        CrapsBet b41("Player1", BetName::PassLine, 1, 4);
        CHECK(b41.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 3);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Point of 5 wins, min contract, min odds 
        CrapsBet b51("Player1", BetName::PassLine, 1, 5);
        CHECK(b51.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b51.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 4);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Point of 6 wins, min contract, min odds 
        CrapsBet b61("Player1", BetName::PassLine, 1, 6);
        CHECK(b61.setOddsAmount(3, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(5, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 7);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
    }
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:DontPass")
{
    SUBCASE("Come Out Roll")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Come out roll, dice = 2, wins
        CrapsBet b21("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // Come out roll, dice = 3, loses
        CrapsBet b31("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // Come out roll, dice = 4, keep
        CrapsBet b41("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Come out roll, dice = 5, keep
        CrapsBet b51("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(4,1);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Come out roll, dice = 6, keep
        CrapsBet b61("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Come out roll, dice = 7, lose
        CrapsBet b71("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // Come out roll, dice = 8, keep
        CrapsBet b81("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Come out roll, dice = 9, keep
        CrapsBet b91("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Come out roll, dice = 10, keep
        CrapsBet b101("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Come out roll, dice = 11, lose
        CrapsBet b111("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // Come out roll, dice = 12, push
        CrapsBet b121("Player1", BetName::DontPass, 100);
        point = 0;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("number hits before 7")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Point of 4 (pivot == 0) illegal bet
        CrapsBet b41("Player1", BetName::DontPass, 100);
        point = 4;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);
        
        // Point of 4 loses (pivot == 4)
        CrapsBet b42("Player1", BetName::DontPass, 100, 4);
        CHECK(b42.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(2,2);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());
        CHECK(b42.pivot() == 4);

        // Point of 5 (pivot == 0) illegal bet
        CrapsBet b51("Player1", BetName::DontPass, 100);
        point = 5;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point of 5 loses (pivot == 5)
        CrapsBet b52("Player1", BetName::DontPass, 100, 5);
        CHECK(b52.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,2);
        CHECK(b52.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b52.distance() == 1);
        CHECK(b52.whenDecided() > b52.whenCreated());
        CHECK(b52.pivot() == 5);

        // Point of 6 (pivot == 0) illegal bet
        CrapsBet b61("Player1", BetName::DontPass, 100);
        point = 6;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point of 6 loses (pivot == 6)
        CrapsBet b62("Player1", BetName::DontPass, 100, 6);
        CHECK(b62.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,3);
        CHECK(b62.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b62.distance() == 1);
        CHECK(b62.whenDecided() > b62.whenCreated());
        CHECK(b62.pivot() == 6);

        // Point of 8 (pivot == 0) illegal bet
        CrapsBet b81("Player1", BetName::DontPass, 100);
        point = 8;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point of 8 loses (pivot == 6)
        CrapsBet b82("Player1", BetName::DontPass, 100, 8);
        CHECK(b82.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 8;
        dice.set(4,4);
        CHECK(b82.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b82.distance() == 1);
        CHECK(b82.whenDecided() > b82.whenCreated());
        CHECK(b82.pivot() == 8);

        // Point of 9 (pivot == 0) illegal bet
        CrapsBet b91("Player1", BetName::DontPass, 100);
        point = 9;
        dice.set(6,3);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point of 9 loses (pivot == 9)
        CrapsBet b92("Player1", BetName::DontPass, 100, 9);
        CHECK(b92.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 9;
        dice.set(6,3);
        CHECK(b92.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b92.distance() == 1);
        CHECK(b92.whenDecided() > b92.whenCreated());
        CHECK(b92.pivot() == 9);

        // Point of 10 (pivot == 0) illegal bet
        CrapsBet b101("Player1", BetName::DontPass, 100);
        point = 10;
        dice.set(6,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point of 10 loses (pivot == 10)
        CrapsBet b102("Player1", BetName::DontPass, 100, 10);
        CHECK(b102.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 10;
        dice.set(6,4);
        CHECK(b102.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b102.distance() == 1);
        CHECK(b102.whenDecided() > b102.whenCreated());
        CHECK(b102.pivot() == 10);
    }
    
    SUBCASE("7 hits before number")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Point of 4, dice = 7, wins
        CrapsBet b41("Player1", BetName::DontPass, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,4);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 200);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Point of 5, dice = 7, wins
        CrapsBet b51("Player1", BetName::DontPass, 100, 5);
        CHECK(b51.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,4);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 233);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Point of 6, dice = 7, wins
        CrapsBet b61("Player1", BetName::DontPass, 100, 6);
        CHECK(b61.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,4);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 266);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // Point of 8, dice = 7, wins
        CrapsBet b81("Player1", BetName::DontPass, 100, 8);
        CHECK(b81.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 8;
        dice.set(3,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 266);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // Point of 9, dice = 7, wins
        CrapsBet b91("Player1", BetName::DontPass, 100, 9);
        CHECK(b91.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 9;
        dice.set(3,4);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 233);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // Point of 10, dice = 7, wins
        CrapsBet b101("Player1", BetName::DontPass, 100, 10);
        CHECK(b101.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 10;
        dice.set(3,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 200);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
    }

    SUBCASE("No decision")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // After point, dice = 2, keep
        CrapsBet b21("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // After point, dice = 3, keep
        CrapsBet b31("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // After point, dice = 4, keep
        CrapsBet b41("Player1", BetName::DontPass, 100, 10);
        point = 10;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // After point, dice = 5, keep
        CrapsBet b51("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // After point, dice = 6, keep
        CrapsBet b61("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // After point, dice = 7, wins
        CrapsBet b71("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // After point, dice = 8, keep
        CrapsBet b81("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // After point, dice = 9, keep
        CrapsBet b91("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // After point, dice = 10 keep
        CrapsBet b101("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());

        // After point, dice = 11 keep
        CrapsBet b111("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // After point, dice = 12 keep
        CrapsBet b121("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("Distance")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        
        // DontPass bet of 4, roll 10 times no decision
        // On eleventh roll, make point, win
        CrapsBet b41("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(6,6);

        for (unsigned i = 1; i <= 10; ++i)
        {
            CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        }
        dice.set(3,4);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 11);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // DontPass bet of 4, roll 10 times no decision
        // On eleventh roll, 7-out, win
        CrapsBet b42("Player1", BetName::DontPass, 100, 4);
        point = 4;
        dice.set(6,2);

        for (unsigned i = 1; i <= 10; ++i)
        {
            CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        }
        dice.set(3,4);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 11);
        CHECK(b42.whenDecided() > b42.whenCreated());
    }

    SUBCASE("Odds")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned maxOdds = 5;

        // Bad time for odds bet, no point established 
        CrapsBet b40("Player1", BetName::DontPass, 100, 0);
        CHECK(b40.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Fail);
        // std::cout << ep.diag << std::endl;
        
        // Bad type of bet for odds bet
        CrapsBet b41("Player1", BetName::Place, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Fail);
        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("Minimum amount bets")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Point of 4, 7 out, wins, min contract, min odds 
        CrapsBet b41("Player1", BetName::DontPass, 1, 4);
        CHECK(b41.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b41.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(6,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 2);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Point of 5, 7 out,  wins, min contract, min odds 
        CrapsBet b51("Player1", BetName::DontPass, 1, 5);
        CHECK(b51.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b51.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b51.setOddsAmount(3, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,4);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 3);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Point of 6, 7 out, wins, min contract, min odds 
        CrapsBet b61("Player1", BetName::DontPass, 1, 6);
        CHECK(b61.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(3, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(4, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(5, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(6,       6, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,4);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 6);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
    }
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Come")
{
    SUBCASE("Come Out Roll")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Come out roll, illegal - must make pass bet instead
        CrapsBet b00("Player1", BetName::Come, 100);
        point = 0;
        dice.set(1,1);  // doesn't matter in this case
        CHECK(b00.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point roll, no pivot yet, dice = 2, loses
        CrapsBet b21("Player1", BetName::Come, 100);
        point = 4;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // Point roll, no pivot yet, dice = 3, loses
        CrapsBet b31("Player1", BetName::Come, 100);
        point = 4;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // Point roll, no pivot, dice = 4, keep
        CrapsBet b41("Player1", BetName::Come, 100);
        point = 5;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Point roll, no pivot, dice = 5, keep
        CrapsBet b51("Player1", BetName::Come, 100);
        point = 4;
        dice.set(4,1);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Point roll, no pivot, dice = 6, keep
        CrapsBet b61("Player1", BetName::Come, 100);
        point = 4;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Point roll, no pivot, dice = 7, wins
        CrapsBet b71("Player1", BetName::Come, 100);
        point = 4;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // Point roll, no pivot, dice = 8, keep
        CrapsBet b81("Player1", BetName::Come, 100);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Point roll, no pivot, dice = 9, keep
        CrapsBet b91("Player1", BetName::Come, 100);
        point = 4;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Point roll, no pivot, dice = 10, keep
        CrapsBet b101("Player1", BetName::Come, 100);
        point = 4;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Point roll, no pivot, dice = 11, wins
        CrapsBet b111("Player1", BetName::Come, 100);
        point = 4;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // Point roll, no pivot, dice = 12, loses
        CrapsBet b121("Player1", BetName::Come, 100);
        point = 4;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("number hits before 7")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Number 4 wins
        CrapsBet b41("Player1", BetName::Come, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 500);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Number 5 wins
        CrapsBet b51("Player1", BetName::Come, 100, 5);
        CHECK(b51.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 400);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Number 6 wins
        CrapsBet b61("Player1", BetName::Come, 100, 6);
        CHECK(b61.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 340);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Number 8 wins
        CrapsBet b81("Player1", BetName::Come, 100, 8);
        CHECK(b81.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 340);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Number 9 wins
        CrapsBet b91("Player1", BetName::Come, 100, 9);
        CHECK(b91.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(6,3);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 400);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Number 10 wins
        CrapsBet b101("Player1", BetName::Come, 100, 10);
        CHECK(b101.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(6,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 500);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);
    }
    
    SUBCASE("7 hits before number")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Number is 4, dice = 7, loses
        CrapsBet b41("Player1", BetName::Come, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,4);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Number is 5, dice = 7, loses
        CrapsBet b51("Player1", BetName::Come, 100, 5);
        CHECK(b51.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,4);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Number is 6, dice = 7, loses
        CrapsBet b61("Player1", BetName::Come, 100, 6);
        CHECK(b61.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,4);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // Number is 8, dice = 7, loses
        CrapsBet b81("Player1", BetName::Come, 100, 8);
        CHECK(b81.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 8;
        dice.set(3,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // Number is 9, dice = 7, loses
        CrapsBet b91("Player1", BetName::Come, 100, 9);
        CHECK(b91.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 9;
        dice.set(3,4);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // Number is 10, dice = 7, loses
        CrapsBet b101("Player1", BetName::Come, 100, 10);
        CHECK(b101.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 10;
        dice.set(3,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
    }
    
    SUBCASE("No decision")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // After point, dice = 2, keep
        CrapsBet b21("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // After point, dice = 3, keep
        CrapsBet b31("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // After point, dice = 4, keep
        CrapsBet b41("Player1", BetName::Come, 100, 10);
        point = 10;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // After point, dice = 5, keep
        CrapsBet b51("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // After point, dice = 6, keep
        CrapsBet b61("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // After point, dice = 7, lose
        CrapsBet b71("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // After point, dice = 8, keep
        CrapsBet b81("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // After point, dice = 9, keep
        CrapsBet b91("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // After point, dice = 10 keep
        CrapsBet b101("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());

        // After point, dice = 11 keep
        CrapsBet b111("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // After point, dice = 12 keep
        CrapsBet b121("Player1", BetName::Come, 100, 4);
        point = 4;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("Return to player")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Number hits on Come out roll, wins and returns odds
        CrapsBet b41("Player1", BetName::Come, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 0;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 200);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Same as previous, but enable odds working on come out roll
        // Number hits on Come out roll, wins with odds
        CrapsBet b42("Player1", BetName::Come, 100, 4);
        CHECK(b42.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        b42.setOnComeOutRoll();
        point = 0;
        dice.set(2,2);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 500);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());

        // 7 repeats before Number Come out roll, loses and returns odds
        CrapsBet b43("Player1", BetName::Come, 100, 4);
        CHECK(b43.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 0;
        dice.set(6,1);
        CHECK(b43.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 200);
        CHECK(b43.distance() == 1);
        CHECK(b43.whenDecided() > b43.whenCreated());

        // Same as previous, but enable odds working on come out roll
        // 7 repeats before Number Come out roll, loses odds too
        CrapsBet b44("Player1", BetName::Come, 100, 4);
        CHECK(b44.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        b44.setOnComeOutRoll();
        point = 0;
        dice.set(6,1);
        CHECK(b44.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b44.distance() == 1);
        CHECK(b44.whenDecided() > b44.whenCreated());
    }
    
    SUBCASE("Minimum amount bets")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Point of 4 wins, min contract, min odds 
        CrapsBet b41("Player1", BetName::Come, 1, 4);
        CHECK(b41.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 3);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Point of 5 wins, min contract, min odds 
        CrapsBet b51("Player1", BetName::Come, 1, 5);
        CHECK(b51.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b51.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 4);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Point of 6 wins, min contract, min odds 
        CrapsBet b61("Player1", BetName::Come, 1, 6);
        CHECK(b61.setOddsAmount(3, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(5, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 7);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
    }
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:DontCome")
{
    SUBCASE("Come Out Roll")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Come out roll, illegal - must make DontPass bet instead
        CrapsBet b00("Player1", BetName::DontCome, 100);
        point = 0;
        dice.set(1,1);  // doesn't matter in this case
        CHECK(b00.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Fail);

        // Point roll, no pivot yet, dice = 2, win
        CrapsBet b21("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // Point roll, no pivot yet, dice = 3, win
        CrapsBet b31("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // Point roll, no pivot, dice = 4, keep
        CrapsBet b41("Player1", BetName::DontCome, 100);
        point = 5;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Point roll, no pivot, dice = 5, keep
        CrapsBet b51("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(4,1);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Point roll, no pivot, dice = 6, keep
        CrapsBet b61("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Point roll, no pivot, dice = 7, loses
        CrapsBet b71("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // Point roll, no pivot, dice = 8, keep
        CrapsBet b81("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Point roll, no pivot, dice = 9, keep
        CrapsBet b91("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Point roll, no pivot, dice = 10, keep
        CrapsBet b101("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == true);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Point roll, no pivot, dice = 11, loses
        CrapsBet b111("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // Point roll, no pivot, dice = 12, keep
        CrapsBet b121("Player1", BetName::DontCome, 100);
        point = 4;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("number hits before 7")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;
        
        // Number 4 loses
        CrapsBet b41("Player1", BetName::DontCome, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Number 5 loses
        CrapsBet b51("Player1", BetName::DontCome, 100, 5);
        CHECK(b51.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Number 6 loses
        CrapsBet b61("Player1", BetName::DontCome, 100, 6);
        CHECK(b61.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Number 8 loses
        CrapsBet b81("Player1", BetName::DontCome, 100, 8);
        CHECK(b81.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Number 9 loses
        CrapsBet b91("Player1", BetName::DontCome, 100, 9);
        CHECK(b91.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(6,3);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Number 10 loses
        CrapsBet b101("Player1", BetName::DontCome, 100, 10);
        CHECK(b101.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(6,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);
    }
    
    SUBCASE("7 hits before number")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Number is 4, dice = 7, wins
        CrapsBet b41("Player1", BetName::DontCome, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,4);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 200);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Number is 5, dice = 7, wins
        CrapsBet b51("Player1", BetName::DontCome, 100, 5);
        CHECK(b51.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,4);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 233);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Number is 6, dice = 7, wins
        CrapsBet b61("Player1", BetName::DontCome, 100, 6);
        CHECK(b61.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,4);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 266);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // Number is 8, dice = 7, wins
        CrapsBet b81("Player1", BetName::DontCome, 100, 8);
        CHECK(b81.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 8;
        dice.set(3,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 266);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // Number is 9, dice = 7, wins
        CrapsBet b91("Player1", BetName::DontCome, 100, 9);
        CHECK(b91.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 9;
        dice.set(3,4);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 233);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // Number is 10, dice = 7, loses
        CrapsBet b101("Player1", BetName::DontCome, 100, 10);
        CHECK(b101.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 10;
        dice.set(3,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 200);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
    }
    
    SUBCASE("No decision")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // After point, dice = 2, keep
        CrapsBet b21("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // After point, dice = 3, keep
        CrapsBet b31("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // After point, dice = 4, keep
        CrapsBet b41("Player1", BetName::DontCome, 100, 10);
        point = 10;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // After point, dice = 5, keep
        CrapsBet b51("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // After point, dice = 6, keep
        CrapsBet b61("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());

        // After point, dice = 7, win
        CrapsBet b71("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // After point, dice = 8, keep
        CrapsBet b81("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());

        // After point, dice = 9, keep
        CrapsBet b91("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());

        // After point, dice = 10 keep
        CrapsBet b101("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());

        // After point, dice = 11 keep
        CrapsBet b111("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // After point, dice = 12 keep
        CrapsBet b121("Player1", BetName::DontCome, 100, 4);
        point = 4;
        dice.set(6,6);
        CHECK(b121.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b121.distance() == 1);
        CHECK(b121.whenDecided() > b121.whenCreated());
    }

    SUBCASE("Return to player")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Number hits on Come out roll, loses and returns odds
        CrapsBet b41("Player1", BetName::DontCome, 100, 4);
        CHECK(b41.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 0;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 200);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Same as previous, but enable odds working on come out roll
        // Number hits on Come out roll, loses with odds
        CrapsBet b42("Player1", BetName::DontCome, 100, 4);
        CHECK(b42.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        b42.setOnComeOutRoll();
        point = 0;
        dice.set(2,2);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 300);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());

        // 7 repeats before Number Come out roll, wins and returns odds
        CrapsBet b43("Player1", BetName::DontCome, 100, 4);
        CHECK(b43.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 0;
        dice.set(6,1);
        CHECK(b43.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 100);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 200);
        CHECK(b43.distance() == 1);
        CHECK(b43.whenDecided() > b43.whenCreated());

        // Same as previous, but enable odds working on come out roll
        // 7 repeats before Number Come out roll, loses odds too
        CrapsBet b44("Player1", BetName::DontCome, 100, 4);
        CHECK(b44.setOddsAmount(200, maxOdds, ep) == Gen::ReturnCode::Success);
        b44.setOnComeOutRoll();
        point = 0;
        dice.set(6,1);
        CHECK(b44.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 200);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b44.distance() == 1);
        CHECK(b44.whenDecided() > b44.whenCreated());
    }
    
    SUBCASE("Minimum amount bets")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;
        unsigned maxOdds = 5;

        // Dont 4 wins, min contract, min odds 
        CrapsBet b41("Player1", BetName::DontCome, 1, 4);
        CHECK(b41.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 4;
        dice.set(3,4);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 2);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Dont 5 wins, min contract, min odds 
        CrapsBet b51("Player1", BetName::DontCome, 1, 5);
        CHECK(b51.setOddsAmount(1, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b51.setOddsAmount(2, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b51.setOddsAmount(3, maxOdds, ep) == Gen::ReturnCode::Success);
        point = 5;
        dice.set(3,4);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 3);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Dont 6 wins, min contract, min odds 
        CrapsBet b61("Player1", BetName::DontCome, 1, 6);
        CHECK(b61.setOddsAmount(3, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(4, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(5, maxOdds, ep) == Gen::ReturnCode::Fail);
        CHECK(b61.setOddsAmount(6,       6, ep) == Gen::ReturnCode::Success);
        point = 6;
        dice.set(3,4);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 6);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
    }
}

//----------------------------------------------------------------


#if 0
#include "OddsTables.h"

    SUBCASE("Print Odds")
    {
        for (unsigned i = 0; i <= 12; i++)
        {
            std::cout << "[" << i << "] (" <<  OddsTables::oddsDont[i].numerator 
                      << "," << OddsTables::oddsDont[i].denominator << ")" << std::endl;
        }
    }

#endif

//----------------------------------------------------------------

