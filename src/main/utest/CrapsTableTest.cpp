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
        
        // Place a bet but player hasn't yet joined the table
        CHECK(t.getNumPlayers() == 0);
        CHECK(t.addBet(p1.getUuid(), BetName::PassLine, 10, 0, ep) == nullptr);

        // Place a good bet first
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 1);
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

}
    
//----------------------------------------------------------------
