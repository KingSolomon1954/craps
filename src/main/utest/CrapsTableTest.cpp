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
        CHECK(t.getNumBets() == 1);
        
        // Place a bet but wrong time for it
        CHECK(t.isComeOutRoll());
        CHECK(t.addBet(p1.getUuid(), BetName::Come, 10, 0, ep) == nullptr);
        CHECK(t.addBet(p1.getUuid(), BetName::DontCome, 10, 0, ep) == nullptr);
        CHECK(t.getNumBets() == 1);

        // Place same bet twice
        CHECK(t.addBet(p1.getUuid(), BetName::Hardway, 10, 8, ep) != nullptr);
        CHECK(t.getNumBets() == 2);
        CHECK(t.addBet(p1.getUuid(), BetName::Hardway, 10, 8, ep) == nullptr);
        CHECK(t.getNumBets() == 2);

        // Add a bet, remember it
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::Field, 10, 0, ep);
        // Ensure table finds it as a bet.
        CHECK(t.haveBet(b1));
        CHECK(t.getNumBets() == 3);

        // Create a bet outside of table, ensure table rejects it
        CrapsTable::BetIntfcPtr b2 = std::make_shared<CrapsBet>(
            p1.getUuid(), BetName::Hardway, 10, 8);
        CHECK(!t.haveBet(b2));
        
// std::cout << ep.diag << std::endl;
    }

    SUBCASE("good bets")
    {
        CrapsTable t;
        Gen::ErrorPass ep;
        Player p1("p1", 1000);
#if 0
        // Create some bets to be used below
        std::shared_ptr<CrapsBet> pass  = std::make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10);
        std::shared_ptr<CrapsBet> place = std::make_shared<CrapsBet>(p1.getUuid(), BetName::Place, 6, 10);
        std::shared_ptr<CrapsBet> field = std::make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10);
        std::shared_ptr<CrapsBet> hard  = std::make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 10);

        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.isComeOutRoll());
        
        CHECK(t.addBet(pass, ep)  == Gen::ReturnCode::Success);
        CHECK(t.addBet(place, ep) == Gen::ReturnCode::Success);
        CHECK(t.addBet(field, ep) == Gen::ReturnCode::Success);
        CHECK(t.addBet(hard, ep)  == Gen::ReturnCode::Success);
#endif
// std::cout << ep.diag << std::endl;
    }
    
}
    
//----------------------------------------------------------------
