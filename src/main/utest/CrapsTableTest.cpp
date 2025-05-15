//----------------------------------------------------------------
//
// File: CrapsTableTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>

#include <iostream>
#include <vector>
#include "CrapsTable.h"
#include "Player.h"

using namespace App;

//----------------------------------------------------------------

TEST_CASE("CrapsTable:constructor")
{
    SUBCASE("initial values")
    {
        CrapsTable t;
        CHECK(t.getPoint() == 0);
        CHECK(t.getLastRoll().value() == 12);
        CHECK(t.isComeOutRoll());
        CHECK(t.isBettingOpen());
        CHECK(t.getIdShooter().empty());
        CHECK(t.getNumPlayers() == 0);
    }
}
    
//----------------------------------------------------------------

TEST_CASE("CrapsTable:players at table")
{
    SUBCASE("add/remove players")
    {
        CrapsTable t;
        Player p1("p1", 1000);
        Gen::ErrorPass ep;
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 1);
        // Add same player again - error
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Fail);
        Player p2("p2", 1000);
        CHECK(t.addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 2);
        Player p3("p3", 1000);
        Player p4("p4", 1000);
        Player p5("p5", 1000);
        Player p6("p6", 1000);
        Player p7("p7", 1000);
        CHECK(t.addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p4.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p5.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p6.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p7.getUuid(), ep) == Gen::ReturnCode::Fail);
        
        // Remove unknown player
        CHECK(t.removePlayer(p7.getUuid(), ep) == Gen::ReturnCode::Fail);
        CHECK(t.getNumPlayers() == 6);
        
        // Remove from middle
        CHECK(t.removePlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 5);
        
        // Remove from front
        CHECK(t.removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 4);

        // Remove from tail
        CHECK(t.removePlayer(p6.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 3);

        // Able to add after removals
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 4);
        
        // Remove all and still able to add 
        CHECK(t.removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.removePlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.removePlayer(p3.getUuid(), ep) == Gen::ReturnCode::Fail);
        CHECK(t.removePlayer(p4.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.removePlayer(p5.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.removePlayer(p6.getUuid(), ep) == Gen::ReturnCode::Fail);
        CHECK(t.getNumPlayers() == 0);
        CHECK(t.addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 1);
    }

    SUBCASE("list of players")
    {
        CrapsTable t;
        CHECK(t.getNumPlayers() == 0);

        // Get back empty vector
        std::vector<Gen::Uuid> v;
        v = t.getPlayers();
        CHECK(v.size() == 0);
       
        Player p1("p1", 1000);
        Player p2("p2", 1000);
        Player p3("p3", 1000);

        // Get back non-empty vector
        Gen::ErrorPass ep;
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        v = t.getPlayers();
        CHECK(v.size() == 3);
    }
}

//----------------------------------------------------------------

TEST_CASE("CrapsTable:placing bets")
{
    SUBCASE("bad bets")
    {
        CrapsTable t;
        Gen::ErrorPass ep;
        Player p1("p1", 1000);
        Player p2("p2", 1000);
        
        // Place a bet but player hasn't yet joined the table
        CHECK(t.getNumPlayers() == 0);
        CHECK(t.addBet(p1.getUuid(), BetName::PassLine, 10, 0, ep) == nullptr);

        // Place a good bet first
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 2);
        CHECK(t.addBet(p1.getUuid(), BetName::Hardway, 10, 10, ep) != nullptr);
        CHECK(t.getNumBetsOnTable() == 1);
        
        // Place a bet but wrong time for it
        CHECK(t.isComeOutRoll());
        CHECK(t.addBet(p1.getUuid(), BetName::Come, 10, 0, ep) == nullptr);
        CHECK(t.addBet(p1.getUuid(), BetName::DontCome, 10, 0, ep) == nullptr);
        CHECK(t.getNumBetsOnTable() == 1);

        // Place same bet twice
        CHECK(t.addBet(p1.getUuid(), BetName::Hardway, 10, 8, ep) != nullptr);
        CHECK(t.getNumBetsOnTable() == 2);
        CHECK(t.addBet(p1.getUuid(), BetName::Hardway, 10, 8, ep) == nullptr);
        CHECK(t.getNumBetsOnTable() == 2);

        // Add a bet, remember it
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::Field, 10, 0, ep);
        // Ensure table finds it as a bet.
        CHECK(t.haveBet(b1));
        CHECK(t.getNumBetsOnTable() == 3);

        // Create a bet outside of table, ensure table rejects it
        CrapsTable::BetIntfcPtr b2 = std::make_shared<CrapsBet>(
            p1.getUuid(), BetName::Hardway, 10, 8);
        CHECK(!t.haveBet(b2));

        // Can't bet zero amount
        CHECK(t.addBet(p1.getUuid(), BetName::DontPass, 0, 0, ep) == nullptr);

        // Can't bet DontPass if there's a point
        t.testSetState(4,6,6);
        CHECK(t.addBet(p1.getUuid(), BetName::DontPass, 10, 0, ep) == nullptr);

        // OK to bet PassLine if there's a point, pivot will be coerced to point
        CrapsTable::BetIntfcPtr b3 = t.addBet(
            p1.getUuid(), BetName::PassLine, 10, 5, ep);
        CHECK(b3 != nullptr);
        CHECK(b3->pivot() == 4);
        CHECK(t.getNumBetsOnTable() == 4);

        // Again, this time with pivot = zero.
        CrapsTable::BetIntfcPtr b4 = t.addBet(
            p2.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(b4 != nullptr);
        CHECK(b4->pivot() == 4);
        CHECK(t.getNumBetsOnTable() == 5);
        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("change bets")
    {
        CrapsTable t;
        Gen::ErrorPass ep;
        Player p1("p1", 1000);
        
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::Field, 10, 0, ep);
        CHECK(t.changeBetAmount(b1, 10, ep) == Gen::ReturnCode::Success);
        CHECK(b1->contractAmount() == 20);

        CHECK(t.changeBetAmount(b1, -5, ep) == Gen::ReturnCode::Success);
        CHECK(b1->contractAmount() == 15);

        CHECK(t.changeBetAmount(b1, -20, ep) == Gen::ReturnCode::Fail);
        CHECK(b1->contractAmount() == 15);
        CHECK(t.changeBetAmount(b1, -15, ep) == Gen::ReturnCode::Fail);
        CHECK(b1->contractAmount() == 15);

        CrapsTable::BetIntfcPtr b2 = t.addBet(
            p1.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(t.changeBetAmount(b2, 20, ep) == Gen::ReturnCode::Success);
        CHECK(b2->contractAmount() == 30);
        CHECK(t.changeBetAmount(b2, -10, ep) == Gen::ReturnCode::Success);
        CHECK(b2->contractAmount() == 20);

        CrapsTable::BetIntfcPtr b3 = t.addBet(
            p1.getUuid(), BetName::DontPass, 10, 0, ep);
        CHECK(t.changeBetAmount(b3, 20, ep) == Gen::ReturnCode::Success);
        CHECK(b3->contractAmount() == 30);
        CHECK(t.changeBetAmount(b3, -10, ep) == Gen::ReturnCode::Success);
        CHECK(b3->contractAmount() == 20);

        // TODO: add more Pass/Dont tests when table establishes a point.
        // std::cout << ep.diag << std::endl;
        t.testSetState(4, 6, 6);
    }
    
    SUBCASE("amount on table")
    {
        CrapsTable t;
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::Field, 10, 0, ep);
        CHECK(t.getAmountOnTable() == 10);
        
        CHECK(t.changeBetAmount(b1, 10, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 20);

        CrapsTable::BetIntfcPtr b2 = t.addBet(
            p1.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(t.getAmountOnTable() == 30);
        CHECK(t.changeBetAmount(b2, -5, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 25);
    }

    SUBCASE("remove bet")
    {
        CrapsTable t;
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::Field, 10, 0, ep);
        CHECK(t.getAmountOnTable() == 10);
        CHECK(t.getNumBetsOnTable() == 1);
        CHECK(t.removeBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);

        // Can't remove PassLine bet after point established
        t.testSetState(4, 6, 6);
        b1 = t.addBet(p1.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(b1 != nullptr);
        CHECK(t.getAmountOnTable() == 10);
        CHECK(t.getNumBetsOnTable() == 1);
        CHECK(t.removeBet(b1, ep) == Gen::ReturnCode::Fail);
        CHECK(t.getNumBetsOnTable() == 1);
    }

    SUBCASE("odds bet")
    {
        CrapsTable t;
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        // Add odds bet, during come out roll - fail
        CHECK(t.isComeOutRoll());
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(t.getAmountOnTable() == 10);
        CHECK(t.getNumBetsOnTable() == 1);
        CHECK(t.setOdds(b1, 10, ep) == Gen::ReturnCode::Fail);
        CHECK(t.getAmountOnTable() == 10);
        CHECK(t.getNumBetsOnTable() == 1);

        // Reset to 0 bets
        CHECK(t.removeBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumBetsOnTable() == 0);

        // Establish a point
        t.testSetState(4, 6, 6);
        
        // Create a bet outside of table, table rejects odds bet
        CrapsTable::BetIntfcPtr b2 = std::make_shared<CrapsBet>(
            p1.getUuid(), BetName::PassLine, 10, 4);
        CHECK(!t.haveBet(b2));
        CHECK(t.setOdds(b2, 10, ep) == Gen::ReturnCode::Fail);

        // Add a PassLine bet. pivot is focused on point 4.
        CrapsTable::BetIntfcPtr b3 = t.addBet(
            p1.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(b3 != nullptr);
        
        // Make an odds bet with zero amount, does nothing, but allowed
        CHECK(t.setOdds(b3, 0, ep) == Gen::ReturnCode::Success);
        
// std::cout << ep.diag << std::endl;

        // Make a valid odds bet 
        CHECK(t.setOdds(b3, 10, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 20);
        
        // Remove odds bet 
        CHECK(t.setOdds(b3, 0, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 10);
        
        // Add odds back in again
        CHECK(t.setOdds(b3, 10, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 20);
        
        // Change odds bet 
        unsigned newAmount = b3->oddsAmount() + 20;
        CHECK(t.setOdds(b3, newAmount, ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 40);
        
        // Change odds bet to zero
        CHECK(t.setOdds(b3, 0, ep) == Gen::ReturnCode::Success);
        CHECK(b3->oddsAmount() == 0);
        CHECK(t.getAmountOnTable() == 10);
        
        // Change odds from zero to an amount
        CHECK(t.setOdds(b3, 20, ep) == Gen::ReturnCode::Success);
        CHECK(b3->oddsAmount() == 20);
        CHECK(t.getAmountOnTable() == 30);
    }
}
    
//----------------------------------------------------------------
