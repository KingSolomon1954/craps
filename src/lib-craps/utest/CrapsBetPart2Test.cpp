//----------------------------------------------------------------
//
// File: CrapsBetPart2Test.cpp
//
// Part 2 does these tests
//   * Place
//   * Lay
//   * Buy
//   * Hardway
//   * Field
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <chrono>
#include <iostream>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <craps/CrapsBet.h>
#include <craps/DecisionRecord.h>
#include <craps/Dice.h>
#include <craps/EnumBetName.h>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Place")
{
    SUBCASE("Come Out Roll")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Place 4, come out, dice = 2, keep
        CrapsBet b21("Player1", BetName::Place, 100, 4);
        point = 0;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // Place 4, come out, dice = 3, keep
        CrapsBet b31("Player1", BetName::Place, 100, 4);
        point = 0;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // Place 4, come out, dice = 4, keep
        CrapsBet b41("Player1", BetName::Place, 100, 4);
        point = 0;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Place 4, come out, dice = 4, win
        CrapsBet b42("Player1", BetName::Place, 100, 4);
        b42.setOnComeOutRoll();
        point = 0;
        dice.set(3,1);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 180);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());
        CHECK(b42.pivot() == 4);

        // Come out roll, dice = 5, keep
        CrapsBet b51("Player1", BetName::Place, 100, 5);
        point = 0;
        dice.set(4,1);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Come out roll, dice = 5, win
        CrapsBet b52("Player1", BetName::Place, 100, 5);
        b52.setOnComeOutRoll();
        point = 0;
        dice.set(4,1);
        CHECK(b52.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 140);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b52.distance() == 1);
        CHECK(b52.whenDecided() > b52.whenCreated());
        CHECK(b52.pivot() == 5);

        // Come out roll, dice = 6, keep
        CrapsBet b61("Player1", BetName::Place, 100, 6);
        point = 0;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Come out roll, dice = 6, win
        CrapsBet b62("Player1", BetName::Place, 100, 6);
        b62.setOnComeOutRoll();
        point = 0;
        dice.set(5,1);
        CHECK(b62.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 116);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b62.distance() == 1);
        CHECK(b62.whenDecided() > b62.whenCreated());
        CHECK(b62.pivot() == 6);

        // Come out roll, dice = 7, keep
        CrapsBet b71("Player1", BetName::Place, 100, 6);
        point = 0;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // Come out roll, dice = 7, lose
        CrapsBet b72("Player1", BetName::Place, 100, 6);
        b72.setOnComeOutRoll();
        point = 0;
        dice.set(3,4);
        CHECK(b72.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b72.distance() == 1);
        CHECK(b72.whenDecided() > b72.whenCreated());

        // Come out roll, dice = 8, keep
        CrapsBet b81("Player1", BetName::Place, 100, 8);
        point = 0;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Come out roll, dice = 8, win
        CrapsBet b82("Player1", BetName::Place, 100, 8);
        b82.setOnComeOutRoll();
        point = 0;
        dice.set(4,4);
        CHECK(b82.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 116);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b82.distance() == 1);
        CHECK(b82.whenDecided() > b82.whenCreated());
        CHECK(b82.pivot() == 8);

        // Come out roll, dice = 9, keep
        CrapsBet b91("Player1", BetName::Place, 100, 9);
        point = 0;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Come out roll, dice = 9, keep
        CrapsBet b92("Player1", BetName::Place, 100, 9);
        b92.setOnComeOutRoll();
        point = 0;
        dice.set(4,5);
        CHECK(b92.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 140);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b92.distance() == 1);
        CHECK(b92.whenDecided() > b92.whenCreated());
        CHECK(b92.pivot() == 9);

        // Come out roll, dice = 10, keep
        CrapsBet b101("Player1", BetName::Place, 100, 10);
        point = 0;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Come out roll, dice = 10, win
        CrapsBet b102("Player1", BetName::Place, 100, 10);
        b102.setOnComeOutRoll();
        point = 0;
        dice.set(5,5);
        CHECK(b102.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 180);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b102.distance() == 1);
        CHECK(b102.whenDecided() > b102.whenCreated());
        CHECK(b102.pivot() == 10);

        // Come out roll, dice = 11, keep
        CrapsBet b111("Player1", BetName::Place, 100, 4);
        point = 0;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // Come out roll, dice = 12, keep
        CrapsBet b121("Player1", BetName::Place, 100, 4);
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
    
    SUBCASE("Point Rolls")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Have point, Place 4, 4 hits, win
        CrapsBet b41("Player1", BetName::Place, 100, 4);
        point = 10;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 180);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Have point, Place 4, 7 out, lose
        CrapsBet b42("Player1", BetName::Place, 100, 4);
        point = 10;
        dice.set(3,4);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());
        CHECK(b42.pivot() == 4);

        // Have point, Place 5, 5 hits, win
        CrapsBet b51("Player1", BetName::Place, 100, 5);
        point = 10;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 140);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Have point, Place 5, 7 out, lose
        CrapsBet b52("Player1", BetName::Place, 100, 5);
        point = 10;
        dice.set(3,4);
        CHECK(b52.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b52.distance() == 1);
        CHECK(b52.whenDecided() > b52.whenCreated());
        CHECK(b52.pivot() == 5);

        // Have point, Place 6, 6 hits, win
        CrapsBet b61("Player1", BetName::Place, 100, 6);
        point = 10;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 116);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Have point, Place 6, 7 out, lose
        CrapsBet b62("Player1", BetName::Place, 100, 6);
        point = 10;
        dice.set(3,4);
        CHECK(b62.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b62.distance() == 1);
        CHECK(b62.whenDecided() > b62.whenCreated());
        CHECK(b62.pivot() == 6);

        // Have point, Place 8, 8 hits, win
        CrapsBet b81("Player1", BetName::Place, 100, 8);
        point = 10;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 116);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Have point, Place 8, 7 out, lose
        CrapsBet b82("Player1", BetName::Place, 100, 8);
        point = 10;
        dice.set(4,3);
        CHECK(b82.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b82.distance() == 1);
        CHECK(b82.whenDecided() > b82.whenCreated());
        CHECK(b82.pivot() == 8);

        // Have point, Place 9, 9 hits, win
        CrapsBet b91("Player1", BetName::Place, 100, 9);
        point = 9;
        dice.set(6,3);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 140);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Have point, Place 9, 7 out, lose
        CrapsBet b92("Player1", BetName::Place, 100, 9);
        point = 10;
        dice.set(6,1);
        CHECK(b92.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b92.distance() == 1);
        CHECK(b92.whenDecided() > b92.whenCreated());
        CHECK(b92.pivot() == 9);

        // Have point, Place 10, 10 hits, win
        CrapsBet b101("Player1", BetName::Place, 100, 10);
        point = 10;
        dice.set(6,4);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 180);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Have point, Place 10, 7 out, lose
        CrapsBet b102("Player1", BetName::Place, 100, 10);
        point = 10;
        dice.set(6,1);
        CHECK(b102.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b102.distance() == 1);
        CHECK(b102.whenDecided() > b102.whenCreated());
        CHECK(b102.pivot() == 10);
    }

    SUBCASE("Distance")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Place bet of 4, roll 10 times no decision
        // On eleventh roll, 4 hits, win
        CrapsBet b41("Player1", BetName::Place, 100, 4);
        point = 10;
        dice.set(6,6);

        for (unsigned i = 1; i <= 10; ++i)
        {
            CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        }
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 180);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 11);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Place bet of 4, roll 10 times no decision
        // On eleventh roll, 7-out, lose
        CrapsBet b42("Player1", BetName::Place, 100, 4);
        point = 10;
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

        // Place 4 wins, min bet
        CrapsBet b41("Player1", BetName::Place, 1, 4);
        point = 4;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 1);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Place 5 wins, min bet
        CrapsBet b51("Player1", BetName::Place, 1, 5);
        point = 5;
        dice.set(3,2);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 1);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());

        // Place 6 wins, min bet
        CrapsBet b61("Player1", BetName::Place, 1, 6);
        point = 6;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 1);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
    }
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Buy")
{
    SUBCASE("decisions")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Buy 4, come out, dice = 2, keep
        CrapsBet b21("Player1", BetName::Buy, 100, 4);
        point = 0;
        dice.set(1,1);
        CHECK(b21.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b21.distance() == 1);
        CHECK(b21.whenDecided() > b21.whenCreated());

        // Buy 4, come out, dice = 3, keep
        CrapsBet b31("Player1", BetName::Buy, 100, 4);
        point = 0;
        dice.set(2,1);
        CHECK(b31.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b31.distance() == 1);
        CHECK(b31.whenDecided() > b31.whenCreated());

        // Buy 4, come out, dice = 4, keep
        CrapsBet b41("Player1", BetName::Buy, 100, 4);
        point = 0;
        dice.set(3,1);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());
        CHECK(b41.pivot() == 4);

        // Buy 4, come out, dice = 4, win
        CrapsBet b42("Player1", BetName::Buy, 100, 4);
        b42.setOnComeOutRoll();
        point = 0;
        dice.set(3,1);
        CHECK(b42.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == (200 - 5));
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b42.distance() == 1);
        CHECK(b42.whenDecided() > b42.whenCreated());
        CHECK(b42.pivot() == 4);

        // Buy 5, come out, dice = 5, keep
        CrapsBet b51("Player1", BetName::Buy, 100, 5);
        point = 0;
        dice.set(4,1);
        CHECK(b51.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b51.distance() == 1);
        CHECK(b51.whenDecided() > b51.whenCreated());
        CHECK(b51.pivot() == 5);

        // Buy 5, come out, dice = 5, win
        CrapsBet b52("Player1", BetName::Buy, 100, 5);
        b52.setOnComeOutRoll();
        point = 0;
        dice.set(4,1);
        CHECK(b52.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == (150 -5));
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b52.distance() == 1);
        CHECK(b52.whenDecided() > b52.whenCreated());
        CHECK(b52.pivot() == 5);

        // Buy 6, come out, dice = 6, keep
        CrapsBet b61("Player1", BetName::Buy, 100, 6);
        point = 0;
        dice.set(5,1);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
        CHECK(b61.pivot() == 6);

        // Buy 6, come out, dice = 6, win
        CrapsBet b62("Player1", BetName::Buy, 100, 6);
        b62.setOnComeOutRoll();
        point = 0;
        dice.set(5,1);
        CHECK(b62.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == (120 - 5));
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b62.distance() == 1);
        CHECK(b62.whenDecided() > b62.whenCreated());
        CHECK(b62.pivot() == 6);

        // Buy 6, come out, dice = 7, keep
        CrapsBet b71("Player1", BetName::Buy, 100, 6);
        point = 0;
        dice.set(3,4);
        CHECK(b71.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b71.distance() == 1);
        CHECK(b71.whenDecided() > b71.whenCreated());

        // Buy 6, come out, dice = 7, lose
        CrapsBet b72("Player1", BetName::Buy, 100, 6);
        b72.setOnComeOutRoll();
        point = 0;
        dice.set(3,4);
        CHECK(b72.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 100);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b72.distance() == 1);
        CHECK(b72.whenDecided() > b72.whenCreated());

        // Buy 8, come out, dice = 8, keep
        CrapsBet b81("Player1", BetName::Buy, 100, 8);
        point = 0;
        dice.set(4,4);
        CHECK(b81.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b81.distance() == 1);
        CHECK(b81.whenDecided() > b81.whenCreated());
        CHECK(b81.pivot() == 8);

        // Buy 8, come out, dice = 8, win
        CrapsBet b82("Player1", BetName::Buy, 100, 8);
        b82.setOnComeOutRoll();
        point = 0;
        dice.set(4,4);
        CHECK(b82.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == (120 - 5));
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b82.distance() == 1);
        CHECK(b82.whenDecided() > b82.whenCreated());
        CHECK(b82.pivot() == 8);

        // Buy 9, come out, dice = 9, keep
        CrapsBet b91("Player1", BetName::Buy, 100, 9);
        point = 0;
        dice.set(4,5);
        CHECK(b91.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b91.distance() == 1);
        CHECK(b91.whenDecided() > b91.whenCreated());
        CHECK(b91.pivot() == 9);

        // Buy 9, come out, dice = 9, win
        CrapsBet b92("Player1", BetName::Buy, 100, 9);
        b92.setOnComeOutRoll();
        point = 0;
        dice.set(4,5);
        CHECK(b92.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == (150 - 5));
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b92.distance() == 1);
        CHECK(b92.whenDecided() > b92.whenCreated());
        CHECK(b92.pivot() == 9);

        // Buy 10, come out, dice = 10, keep
        CrapsBet b101("Player1", BetName::Buy, 100, 10);
        point = 0;
        dice.set(5,5);
        CHECK(b101.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b101.distance() == 1);
        CHECK(b101.whenDecided() > b101.whenCreated());
        CHECK(b101.pivot() == 10);

        // Buy 10, come out, dice = 10, win
        CrapsBet b102("Player1", BetName::Buy, 100, 10);
        b102.setOnComeOutRoll();
        point = 0;
        dice.set(5,5);
        CHECK(b102.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == (200 - 5));
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b102.distance() == 1);
        CHECK(b102.whenDecided() > b102.whenCreated());
        CHECK(b102.pivot() == 10);

        // Buy 4, come out, dice = 11, keep
        CrapsBet b111("Player1", BetName::Buy, 100, 4);
        point = 0;
        dice.set(6,5);
        CHECK(b111.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == false);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 0);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b111.distance() == 1);
        CHECK(b111.whenDecided() > b111.whenCreated());

        // Buy 4, come out, dice = 12, keep
        CrapsBet b121("Player1", BetName::Buy, 100, 4);
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
}

//----------------------------------------------------------------

void lay1(unsigned point, unsigned num, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Lay, 100, num);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == false);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void lay2(unsigned point, unsigned num, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Lay, 100, num);
    b.setOnComeOutRoll();
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void lay3(unsigned point, unsigned num, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Lay, 100, num);
    b.setOnComeOutRoll();
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    if (b.pivot() == 4 || b.pivot() == 10)
        CHECK(dr.win == (50 - 2));  // subtract 5% commission
    if (b.pivot() == 5 || b.pivot() == 9)
        CHECK(dr.win == (66 - 3));   // subtract 5% commission
    if (b.pivot() == 6 || b.pivot() == 8)
        CHECK(dr.win == (83 - 4));  // subtract 5% commission
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Lay")
{
    SUBCASE("decisions")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;

        lay1(0, 4, 3, 1);   // Lay 4, come out, dice = 4, keep (odds off come out)
        lay1(0, 4, 3, 4);   // Lay 4, come out, dice = 7, keep (odds off come out)
        lay1(0, 4, 3, 5);   // Lay 4, come out, dice = 5, keep (odds off come out)
        lay1(0, 5, 3, 2);   // Lay 5, come out, dice = 5, keep (odds off come out)
        lay1(0, 5, 3, 4);   // Lay 5, come out, dice = 7, keep (odds off come out)
        lay1(0, 5, 3, 1);   // Lay 5, come out, dice = 4, keep (odds off come out)
        lay1(0, 6, 3, 3);   // Lay 6, come out, dice = 6, keep (odds off come out)
        lay1(0, 6, 3, 4);   // Lay 6, come out, dice = 7, keep (odds off come out)
        lay1(0, 6, 3, 1);   // Lay 6, come out, dice = 4, keep (odds off come out)
        lay1(0, 8, 3, 5);   // Lay 8, come out, dice = 8, keep (odds off come out)
        lay1(0, 8, 3, 4);   // Lay 8, come out, dice = 7, keep (odds off come out)
        lay1(0, 8, 3, 1);   // Lay 8, come out, dice = 4, keep (odds off come out)
        lay1(0, 9, 3, 6);   // Lay 9, come out, dice = 9, keep (odds off come out)
        lay1(0, 9, 3, 4);   // Lay 9, come out, dice = 7, keep (odds off come out)
        lay1(0, 9, 3, 1);   // Lay 9, come out, dice = 4, keep (odds off come out)
        lay1(0, 10, 4, 6);  // Lay 10, come out, dice = 10, keep (odds off come out)
        lay1(0, 10, 3, 4);  // Lay 10, come out, dice = 7, keep (odds off come out)
        lay1(0, 10, 3, 1);  // Lay 10, come out, dice = 4, keep (odds off come out)

        lay1(5, 4, 5, 5);   // Lay 4, point roll, dice = 10, keep
        lay1(5, 5, 5, 5);   // Lay 5, point roll, dice = 10, keep
        lay1(5, 6, 5, 5);   // Lay 6, point roll, dice = 10, keep
        lay1(5, 8, 5, 5);   // Lay 8, point roll, dice = 10, keep
        lay1(5, 9, 5, 5);   // Lay 9, point roll, dice = 10, keep
        lay1(5, 10, 5, 4);   // Lay 10, point roll, dice = 9, keep

        lay1(0, 4, 1, 1);   // Lay 4, come out, dice = 2, keep
        lay1(0, 4, 1, 2);   // Lay 4, come out, dice = 3, keep
        lay1(0, 4, 5, 6);   // Lay 4, come out, dice = 11, keep
        lay1(0, 4, 6, 6);   // Lay 4, come out, dice = 12, keep
        lay1(5, 4, 1, 1);   // Lay 4, point roll, dice = 2, keep
        lay1(5, 4, 1, 2);   // Lay 4, point roll, dice = 3, keep
        lay1(5, 4, 5, 6);   // Lay 4, point roll, dice = 11, keep
        lay1(5, 4, 6, 6);   // Lay 4, point roll, dice = 12, keep
        
        lay2(0, 4, 3, 1);   // Lay 4, come out, dice = 4, lose (odds on come out)
        lay2(0, 5, 3, 2);   // Lay 5, come out, dice = 5, lose (odds on come out)
        lay2(0, 6, 3, 3);   // Lay 6, come out, dice = 6, lose (odds on come out)
        lay2(0, 8, 4, 4);   // Lay 8, come out, dice = 8, lose (odds on come out)
        lay2(0, 9, 5, 4);   // Lay 9, come out, dice = 9, lose (odds on come out)
        lay2(0, 10, 5, 5);  // Lay 10, come out, dice = 10, lose (odds on come out)

        lay3(0, 4, 3, 4);   // Lay 4, come out, dice = 7, win (odds on come out)
        lay3(0, 5, 3, 4);   // Lay 5, come out, dice = 7, win (odds on come out)
        lay3(0, 6, 3, 4);   // Lay 6, come out, dice = 7, win (odds on come out)
        lay3(0, 8, 3, 4);   // Lay 8, come out, dice = 7, win (odds on come out)
        lay3(0, 9, 3, 4);   // Lay 9, come out, dice = 7, win (odds on come out)
        lay3(0, 10, 3, 4);  // Lay 10, come out, dice = 7, win (odds on come out)
        
        lay2(4, 4, 3, 1);   // Lay 4, point roll, dice = 4, lose
        lay2(4, 5, 3, 2);   // Lay 5, point roll, dice = 5, lose
        lay2(4, 6, 3, 3);   // Lay 6, point roll, dice = 6, lose
        lay2(4, 8, 3, 5);   // Lay 8, point roll, dice = 8, lose
        lay2(4, 9, 3, 6);   // Lay 9, point roll, dice = 9, lose
        lay2(4, 10, 4, 6);  // Lay 10, point roll, dice = 10, lose

        lay3(4, 4, 3, 4);   // Lay 4, point roll, dice = 7, win
        lay3(4, 5, 3, 4);   // Lay 5, point roll, dice = 7, win
        lay3(4, 6, 3, 4);   // Lay 6, point roll, dice = 7, win
        lay3(4, 8, 3, 4);   // Lay 8, point roll, dice = 7, win
        lay3(4, 9, 3, 4);   // Lay 9, point roll, dice = 7, win
        lay3(4, 10, 3, 4);  // Lay 10, point roll, dice = 7, win
    }
}

//----------------------------------------------------------------

void hard1(unsigned point, unsigned num, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Hardway, 100, num);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void hard2(unsigned point, unsigned num, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Hardway, 100, num);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    if (b.pivot() == 4 || b.pivot() == 10)
        CHECK(dr.win == 700);
    if (b.pivot() == 6 || b.pivot() == 8)
        CHECK(dr.win == 900);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void hard3(unsigned point, unsigned num, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Hardway, 100, num);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == false);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Hardway")
{
    SUBCASE("decisions")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;

        hard1(0, 4, 3, 1);   // hard 4, come out, loses easy way
        hard1(0, 4, 3, 4);   // hard 4, come out, loses on 7
        hard1(9, 4, 3, 1);   // hard 4, point roll, loses easy way
        hard1(9, 4, 3, 4);   // hard 4, point roll, loses on 7
        hard1(0, 6, 4, 2);   // hard 6, come out, loses easy way
        hard1(0, 6, 3, 4);   // hard 6, come out, loses on 7
        hard1(9, 6, 5, 1);   // hard 6, point roll, loses easy way
        hard1(9, 6, 3, 4);   // hard 6, point roll, loses on 7
        hard1(0, 8, 6, 2);   // hard 8, come out, loses easy way
        hard1(0, 8, 3, 4);   // hard 8, come out, loses on 7
        hard1(9, 8, 5, 3);   // hard 8, point roll, loses easy way
        hard1(9, 8, 3, 4);   // hard 8, point roll, loses on 7
        hard1(0, 10, 6, 4);  // hard 8, come out, loses easy way
        hard1(0, 10, 3, 4);  // hard 8, come out, loses on 7
        hard1(9, 10, 6, 4);  // hard 8, point roll, loses easy way
        hard1(9, 10, 3, 4);  // hard 8, point roll, loses on 7
        
        hard2(0, 4, 2, 2);   // hard 4, come out, wins
        hard2(9, 4, 2, 2);   // hard 4, point roll, wins
        hard2(0, 6, 3, 3);   // hard 6, come out, wins
        hard2(9, 6, 3, 3);   // hard 8, point roll, wins
        hard2(0, 8, 4, 4);   // hard 4, come out, wins
        hard2(9, 8, 4, 4);   // hard 4, point roll, wins
        hard2(0, 10, 5, 5);  // hard 6, come out, wins
        hard2(9, 10, 5, 5);  // hard 8, point roll, wins

        hard3(0, 4, 1, 1);   // hard 4, come out, roll 2 keep
        hard3(0, 4, 1, 2);   // hard 4, come out, roll 3 keep
        hard3(0, 4, 6, 5);   // hard 4, come out, roll 11 keep
        hard3(0, 4, 6, 5);   // hard 4, come out, roll 12 keep
        hard3(9, 4, 1, 1);   // hard 4, point out, roll 2 keep
        hard3(9, 4, 1, 2);   // hard 4, point out, roll 3 keep
        hard3(9, 4, 6, 5);   // hard 4, point out, roll 11 keep
        hard3(9, 4, 6, 5);   // hard 4, point out, roll 12 keep

        hard3(0, 6, 1, 1);   // hard 6, come out, roll 2 keep
        hard3(0, 6, 1, 2);   // hard 6, come out, roll 3 keep
        hard3(0, 6, 6, 5);   // hard 6, come out, roll 11 keep
        hard3(0, 6, 6, 5);   // hard 6, come out, roll 12 keep
        hard3(9, 6, 1, 1);   // hard 6, point out, roll 2 keep
        hard3(9, 6, 1, 2);   // hard 6, point out, roll 3 keep
        hard3(9, 6, 6, 5);   // hard 6, point out, roll 11 keep
        hard3(9, 6, 6, 5);   // hard 6, point out, roll 12 keep

        hard3(0, 8, 1, 1);   // hard 8, come out, roll 2 keep
        hard3(0, 8, 1, 2);   // hard 8, come out, roll 3 keep
        hard3(0, 8, 6, 5);   // hard 8, come out, roll 11 keep
        hard3(0, 8, 6, 5);   // hard 8, come out, roll 12 keep
        hard3(9, 8, 1, 1);   // hard 8, point out, roll 2 keep
        hard3(9, 8, 1, 2);   // hard 8, point out, roll 3 keep
        hard3(9, 8, 6, 5);   // hard 8, point out, roll 11 keep
        hard3(9, 8, 6, 5);   // hard 8, point out, roll 12 keep

        hard3(0, 10, 1, 1);  // hard 10, come out, roll 2 keep
        hard3(0, 10, 1, 2);  // hard 10, come out, roll 3 keep
        hard3(0, 10, 6, 5);  // hard 10, come out, roll 11 keep
        hard3(0, 10, 6, 5);  // hard 10, come out, roll 12 keep
        hard3(9, 10, 1, 1);  // hard 10, point out, roll 2 keep
        hard3(9, 10, 1, 2);  // hard 10, point out, roll 3 keep
        hard3(9, 10, 6, 5);  // hard 10, point out, roll 11 keep
        hard3(9, 10, 6, 5);  // hard 10, point out, roll 12 keep
    }

    SUBCASE("Distance")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;
        unsigned point = 0;

        // Hardway bet of 4, roll 10 times no decision
        // On eleventh roll, 4 hits, win
        CrapsBet b41("Player1", BetName::Hardway, 100, 4);
        point = 10;
        dice.set(6,6);

        for (unsigned i = 1; i <= 10; ++i)
        {
            CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        }
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 700);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 11);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Hardway bet of 4, roll 10 times no decision
        // On eleventh roll, 7-out, lose
        CrapsBet b42("Player1", BetName::Hardway, 100, 4);
        point = 10;
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

        // Hardway 4 wins, min bet
        CrapsBet b41("Player1", BetName::Hardway, 1, 4);
        point = 4;
        dice.set(2,2);
        CHECK(b41.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 7);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b41.distance() == 1);
        CHECK(b41.whenDecided() > b41.whenCreated());

        // Hardway 6 wins, min bet
        CrapsBet b61("Player1", BetName::Hardway, 1, 6);
        point = 6;
        dice.set(3,3);
        CHECK(b61.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 9);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b61.distance() == 1);
        CHECK(b61.whenDecided() > b61.whenCreated());
    }
}

//----------------------------------------------------------------

void field1(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Field, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    if (dice.value() == 2 || dice.value() == 12)
        CHECK(dr.win == 200);
    else
        CHECK(dr.win == 100);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void field2(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Field, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Field")
{
    SUBCASE("decisions")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;
        Dice dice;

        field1(0, 1, 1);  // come out, dice 2, win
        field1(0, 1, 2);  // come out, dice 3, win
        field1(0, 1, 3);  // come out, dice 4, win
        field1(0, 6, 3);  // come out, dice 9, win
        field1(0, 6, 4);  // come out, dice 10, win
        field1(0, 6, 5);  // come out, dice 11, win
        field1(0, 6, 6);  // come out, dice 12, win

        field2(0, 4, 1);  // come out, dice 5, lose
        field2(0, 4, 2);  // come out, dice 6, lose
        field2(0, 4, 3);  // come out, dice 7, lose
        field2(0, 4, 4);  // come out, dice 8, lose
    }
}

//----------------------------------------------------------------

void anyCraps1(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::AnyCraps, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 700);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void anyCraps2(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::AnyCraps, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:AnyCraps")
{
    SUBCASE("decisions")
    {
        anyCraps1(0, 1, 1);  // come out, dice 2, win
        anyCraps1(0, 1, 2);  // come out, dice 3, win
        anyCraps1(0, 6, 6);  // come out, dice 12, win
        anyCraps1(4, 1, 1);  // point roll, dice 2, win
        anyCraps1(4, 1, 2);  // point roll, dice 3, win
        anyCraps1(4, 6, 6);  // point roll, dice 12, win

        anyCraps2(0, 2, 2);  // come out, dice 4, lose
        anyCraps2(0, 2, 3);  // come out, dice 5, lose
        anyCraps2(0, 2, 4);  // come out, dice 6 lose
        anyCraps2(0, 2, 5);  // come out, dice 7 lose
        anyCraps2(0, 2, 6);  // come out, dice 8 lose
        anyCraps2(0, 3, 6);  // come out, dice 9 lose
        anyCraps2(0, 4, 6);  // come out, dice 10 lose
        anyCraps2(0, 5, 6);  // come out, dice 11 lose
    }
}

//----------------------------------------------------------------

void anySeven1(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::AnySeven, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:AnySeven")
{
    SUBCASE("decisions")
    {
        Gen::ErrorPass ep;
        DecisionRecord dr;

        anySeven1(0, 1, 1);  // come out, dice 2, lose
        anySeven1(0, 1, 2);  // come out, dice 3, lose
        anySeven1(0, 1, 3);  // come out, dice 4, lose
        anySeven1(0, 1, 4);  // come out, dice 5, lose
        anySeven1(0, 1, 5);  // come out, dice 6 lose

        anySeven1(0, 6, 2);  // come out, dice 8 lose
        anySeven1(0, 6, 3);  // come out, dice 9 lose
        anySeven1(0, 6, 4);  // come out, dice 10 lose
        anySeven1(0, 6, 5);  // come out, dice 11 lose
        anySeven1(0, 6, 6);  // come out, dice 12 lose

        anySeven1(4, 1, 1);  // come out, dice 2, lose
        anySeven1(4, 1, 2);  // come out, dice 3, lose
        anySeven1(4, 1, 3);  // come out, dice 4, lose
        anySeven1(4, 1, 4);  // come out, dice 5, lose
        anySeven1(4, 1, 5);  // come out, dice 6 lose

        anySeven1(4, 6, 2);  // come out, dice 8 lose
        anySeven1(4, 6, 3);  // come out, dice 9 lose
        anySeven1(4, 6, 4);  // come out, dice 10 lose
        anySeven1(4, 6, 5);  // come out, dice 11 lose
        anySeven1(4, 6, 6);  // come out, dice 12 lose

        Dice dice; dice.set(3,4);
        CrapsBet b("Player1", BetName::AnySeven, 100);
        CHECK(b.evaluate(0, dice, dr, ep) == Gen::ReturnCode::Success);
        CHECK(dr.decision == true);
        CHECK(dr.pivotAssigned == false);
        CHECK(dr.win == 400);
        CHECK(dr.lose == 0);
        CHECK(dr.returnToPlayer == 0);
        CHECK(b.distance() == 1);
        CHECK(b.whenDecided() > b.whenCreated());
    }
}

//----------------------------------------------------------------

void cAndE1(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::CandE, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    if (dice.value() == 11) CHECK(dr.win == 750);
    if (dice.value() == 2 || dice.value() == 3 || dice.value() == 12)
        CHECK(dr.win == 350);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void cAndE2(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::CandE, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:C&E")
{
    SUBCASE("decisions")
    {
        cAndE1(0, 1, 1);  // come out, dice 2, win
        cAndE1(0, 1, 2);  // come out, dice 3, win
        cAndE2(0, 1, 3);  // come out, dice 4, lose
        cAndE2(0, 1, 4);  // come out, dice 5, lose
        cAndE2(0, 1, 5);  // come out, dice 6 lose
        cAndE2(0, 1, 6);  // come out, dice 7 lose
        cAndE2(0, 6, 2);  // come out, dice 8 lose
        cAndE2(0, 6, 3);  // come out, dice 9 lose
        cAndE2(0, 6, 4);  // come out, dice 10 lose
        cAndE1(0, 6, 5);  // come out, dice 11 win
        cAndE1(0, 6, 6);  // come out, dice 12 win

        cAndE1(4, 1, 1);  // point roll, dice 2, win
        cAndE1(4, 1, 2);  // point roll, dice 3, win
        cAndE2(4, 1, 3);  // point roll, dice 4, lose
        cAndE2(4, 1, 4);  // point roll, dice 5, lose
        cAndE2(4, 1, 5);  // point roll, dice 6 lose
        cAndE2(4, 1, 6);  // point roll, dice 7 lose
        cAndE2(4, 6, 2);  // point roll, dice 8 lose
        cAndE2(4, 6, 3);  // point roll, dice 9 lose
        cAndE2(4, 6, 4);  // point roll, dice 10 lose
        cAndE1(4, 6, 5);  // point roll, dice 11 win
        cAndE1(4, 6, 6);  // point roll, dice 12 win

        // Bad bet, below min
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::CandE, 1),
                                   std::invalid_argument);
        // Bad bet, not a multiple of 2
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::CandE, 3),
                                   std::invalid_argument);

        CrapsBet b("Player1", BetName::CandE, 2);  // good min bet
    }
}

//----------------------------------------------------------------

void horn1(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Horn, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    if (dice.value() == 2 || dice.value() == 12) CHECK(dr.win == 750);
    if (dice.value() == 3 || dice.value() == 11) CHECK(dr.win == 375);
    CHECK(dr.lose == 0);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

void horn2(unsigned point, unsigned d1, unsigned d2)
{
    Gen::ErrorPass ep;
    DecisionRecord dr;

    Dice dice; dice.set(d1,d2);
    CrapsBet b("Player1", BetName::Horn, 100);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.decision == true);
    CHECK(dr.pivotAssigned == false);
    CHECK(dr.win == 0);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 0);
    CHECK(b.distance() == 1);
    CHECK(b.whenDecided() > b.whenCreated());
}

//----------------------------------------------------------------

TEST_CASE("CrapsBet:evaluate:Horn")
{
    SUBCASE("decisions")
    {
        horn1(0, 1, 1);  // come out, dice 2, win
        horn1(0, 1, 2);  // come out, dice 3, win
        horn2(0, 1, 3);  // come out, dice 4, lose
        horn2(0, 1, 4);  // come out, dice 5, lose
        horn2(0, 1, 5);  // come out, dice 6 lose
        horn2(0, 1, 6);  // come out, dice 7 lose
        horn2(0, 6, 2);  // come out, dice 8 lose
        horn2(0, 6, 3);  // come out, dice 9 lose
        horn2(0, 6, 4);  // come out, dice 10 lose
        horn1(0, 6, 5);  // come out, dice 11 win
        horn1(0, 6, 6);  // come out, dice 12 win

        horn1(4, 1, 1);  // point roll, dice 2, win
        horn1(4, 1, 2);  // point roll, dice 3, win
        horn2(4, 1, 3);  // point roll, dice 4, lose
        horn2(4, 1, 4);  // point roll, dice 5, lose
        horn2(4, 1, 5);  // point roll, dice 6 lose
        horn2(4, 1, 6);  // point roll, dice 7 lose
        horn2(4, 6, 2);  // point roll, dice 8 lose
        horn2(4, 6, 3);  // point roll, dice 9 lose
        horn2(4, 6, 4);  // point roll, dice 10 lose
        horn1(4, 6, 5);  // point roll, dice 11 win
        horn1(4, 6, 6);  // point roll, dice 12 win

        // Bad bet, below min
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Horn, 1),
                                   std::invalid_argument);
        // Bad bet, below min
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Horn, 2),
                                   std::invalid_argument);
        // Bad bet, below min
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Horn, 3),
                                   std::invalid_argument);
        // Good min bet
        CrapsBet b("Player1", BetName::Horn, 4);
        
        // Bad bet, not a multiple of 4
        CHECK_THROWS_AS(CrapsBet b("Player1", BetName::Horn, 5),
                                   std::invalid_argument);
    }
}

//----------------------------------------------------------------
