//----------------------------------------------------------------
//
// File: CrapsTableTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>

#include <iostream>
#include <vector>
#include <controller/ConfigManager.h>
#include <controller/EventManager.h>
#include <controller/PlayerManager.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>

using namespace Craps;
using namespace Ctrl;

//----------------------------------------------------------------

struct CrapsTableFixture
{
    Ctrl::ConfigManager configMgr;
    Ctrl::EventManager  em;
    Ctrl::PlayerManager pm;

    CrapsTableFixture()
        : configMgr(5, std::vector<char*>{
            const_cast<char*>("RoyalCraps"),
            const_cast<char*>("--sys-config-dir"),
            const_cast<char*>("/work/craps/assets/"),
            const_cast<char*>("usr-config-dir"),
            const_cast<char*>("/work/craps/assets/")
        }.data())
    {
        Gbl::pConfigMgr = &configMgr;
        Gbl::pEventMgr  = &em;
        Gbl::pPlayerMgr = &pm;
    }

   ~CrapsTableFixture()
    {
        Gbl::pConfigMgr = nullptr;
        Gbl::pEventMgr  = nullptr;
        Gbl::pPlayerMgr = nullptr;
    }
};

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:ctor")
{
    SUBCASE("normalCtor")
    {
        CrapsTable t("Table-1");
        CHECK(t.getPoint() == 0);
        CHECK(t.getCurRoll().value() == 12);
        CHECK(t.isComeOutRoll());
        CHECK(t.isBettingOpen());
        CHECK(t.getShooterId().empty());
        CHECK(t.getNumPlayers() == 0);
    }

    SUBCASE("fromConfig")
    {
        auto* t = CrapsTable::fromConfig("Table-1");
        REQUIRE(t != nullptr);
        CHECK(t->getPoint() == 0);
        CHECK(t->getCurRoll().value() == 12);
        CHECK(t->isComeOutRoll());
        CHECK(t->isBettingOpen());
        CHECK(t->getShooterId().empty());
        CHECK(t->getNumPlayers() == 0);
        delete t;
    }
    
    SUBCASE("fromFile:exists")
    {
        CrapsTable* p = CrapsTable::fromFile("Table-1");
        REQUIRE(p != nullptr);
        CHECK(p->getPoint() == 0);
        CHECK(p->getCurRoll().value() == 12);
        CHECK(p->isComeOutRoll());
        CHECK(p->isBettingOpen());
        CHECK(p->getShooterId().empty());
        CHECK(p->getNumPlayers() == 0);
        delete p;
    }

    SUBCASE("fromFile:missing")
    {
        // Use bad tableId so file won't be found.
        CHECK_THROWS_AS(CrapsTable::fromFile("FakeTable-1"), std::runtime_error);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:playersAtTable")
{
    SUBCASE("addRemove")
    {
        CrapsTable t("Table-1");
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

    SUBCASE("playerList")
    {
        CrapsTable t("Table-1");
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

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:placingBets")
{
    SUBCASE("badBets")
    {
        CrapsTable t("Table-1");
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

        // Add a bet outside the table limits
        CHECK(t.addBet(p1.getUuid(), BetName::PassLine,
                       t.getMinLineBet() - 1, 0, ep) == nullptr);
        CHECK(t.addBet(p1.getUuid(), BetName::PassLine,
                       t.getMaxLineBet() + 1, 0, ep) == nullptr);
    }

    SUBCASE("changeBets")
    {
        CrapsTable t("Table-1");
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
    
    SUBCASE("amountOnTable")
    {
        CrapsTable t("Table-1");
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

    SUBCASE("removeBet")
    {
        CrapsTable t("Table-1");
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


    SUBCASE("removePlayerOutstandingBets")
    {
        CrapsTable t("Table-1");
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        
        CrapsTable::BetIntfcPtr b1 = t.addBet(
            p1.getUuid(), BetName::Field, 10, 0, ep);
        CrapsTable::BetIntfcPtr b2 = t.addBet(
            p1.getUuid(), BetName::PassLine, 10, 0, ep);
        CHECK(t.getAmountOnTable() == 20);
        CHECK(t.getNumBetsOnTable() == 2);
        CHECK(t.removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);
    }

    SUBCASE("oddsBet")
    {
        CrapsTable t("Table-1");
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

void printDice(unsigned point, const Dice& d)
{
    std::cout << "point " << point << " dice " << d.value() << "(" << d.d1() << "," << d.d2() << ")\n";
}
    
TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:rollDice")
{
    SUBCASE("firstRoll")
    {
        CrapsTable t("Table-1");
        Gbl::pTable = &t;
        Gen::ErrorPass ep;

        // First roll, no players, it's a come out roll.
        CHECK(t.isComeOutRoll());
        t.testRollDice(6,6);       // roll a 12, success is no crash
        CHECK(t.isComeOutRoll());  // still coming out
        CHECK(t.getCurRoll().value() == 12);
        CHECK(t.getCurRoll().d1() == 6);
        CHECK(t.getCurRoll().d2() == 6);

        // Create and add a couple of players
        Ctrl::PlayerManager::PlayerPtr john = Gbl::pPlayerMgr->createPlayer("John");
        Ctrl::PlayerManager::PlayerPtr jane = Gbl::pPlayerMgr->createPlayer("Jane");
        CHECK(john->joinTable(ep) == Gen::ReturnCode::Success);
        CHECK(jane->joinTable(ep) == Gen::ReturnCode::Success);
        CHECK(t.getNumPlayers() == 2);
        Gen::Money johnBalance = john->getBalance();
        Gen::Money janeBalance = jane->getBalance();

        // come out roll, roll a 7, pass line win, dont pass lose
        CHECK(john->makeBet(BetName::PassLine, 10, 0, ep) == Gen::ReturnCode::Success);
        CHECK(john->getBalance() == (johnBalance - 10));
        std::cout << "John makes PassLine bet\n";
        CHECK(jane->makeBet(BetName::DontPass, 10, 0, ep) == Gen::ReturnCode::Success);
        CHECK(jane->getBalance() == (janeBalance - 10));
        std::cout << "Jane makes DontPass bet\n";

        t.testRollDice(3,4);
        CHECK(john->getBalance() == (johnBalance + 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t.isComeOutRoll());

        // come out roll, roll a 11, pass line win, dont pass lose
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        CHECK(john->makeBet(BetName::PassLine, 10, 0, ep) == Gen::ReturnCode::Success);
        CHECK(john->getBalance() == (johnBalance - 10));
        std::cout << "John makes PassLine bet\n";
        CHECK(jane->makeBet(BetName::DontPass, 10, 0, ep) == Gen::ReturnCode::Success);
        CHECK(jane->getBalance() == (janeBalance - 10));
        std::cout << "Jane makes DontPass bet\n";

        t.testRollDice(6,5);
        CHECK(john->getBalance() == (johnBalance + 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t.isComeOutRoll());

        // come out roll, roll a 2, pass line lose, dont pass win
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        CHECK(john->makeBet(BetName::PassLine, 10, 0, ep) == Gen::ReturnCode::Success);
        CHECK(john->getBalance() == (johnBalance - 10));
        std::cout << "John makes PassLine bet\n";
        CHECK(jane->makeBet(BetName::DontPass, 10, 0, ep) == Gen::ReturnCode::Success);
        CHECK(jane->getBalance() == (janeBalance - 10));
        std::cout << "Jane makes DontPass bet\n";

        t.testRollDice(1,1);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance + 10));
        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t.isComeOutRoll());

        // come out roll, roll a 3, pass line lose, dont pass win
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        CHECK(john->makeBet(BetName::PassLine, 10, 0, ep) == Gen::ReturnCode::Success);
        std::cout << "John makes PassLine bet\n";
        CHECK(jane->makeBet(BetName::DontPass, 10, 0, ep) == Gen::ReturnCode::Success);
        std::cout << "Jane makes DontPass bet\n";

        t.testRollDice(1,2);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance + 10));
        CHECK(t.getAmountOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t.isComeOutRoll());

        // come out roll, roll a 12, pass line lose, dont pass push
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        CHECK(john->makeBet(BetName::PassLine, 10, 0, ep) == Gen::ReturnCode::Success);
        std::cout << "John makes PassLine bet\n";
        CHECK(jane->makeBet(BetName::DontPass, 10, 0, ep) == Gen::ReturnCode::Success);
        std::cout << "Jane makes DontPass bet\n";

        t.testRollDice(6,6);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == janeBalance - 10);
        CHECK(t.getAmountOnTable() == 10);
        CHECK(t.getNumBetsOnTable() == 1);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 1);
        CHECK(jane->getAmountOnTable() == 10);
        CHECK(t.isComeOutRoll());

        jane->removeBet(BetName::DontPass, 0, ep);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(t.getNumBetsOnTable() == 0);
        
        // come out roll, roll a 4, pass line kepp, dont pass keep
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        CHECK(john->makeBet(BetName::PassLine, 10, 0, ep) == Gen::ReturnCode::Success);
        std::cout << "John makes PassLine bet\n";
        CHECK(jane->makeBet(BetName::DontPass, 10, 0, ep) == Gen::ReturnCode::Success);
        std::cout << "Jane makes DontPass bet\n";
        CHECK(john->getBalance() == johnBalance - 10);
        CHECK(jane->getBalance() == janeBalance - 10);

        t.testRollDice(2,2);
        CHECK(john->getBalance() == johnBalance - 10);
        CHECK(jane->getBalance() == janeBalance - 10);
        CHECK(t.getAmountOnTable() == 20);
        CHECK(t.getNumBetsOnTable() == 2);
        CHECK(john->getNumBetsOnTable() == 1);
        CHECK(john->getAmountOnTable() == 10);
        CHECK(jane->getNumBetsOnTable() == 1);
        CHECK(jane->getAmountOnTable() == 10);
        CHECK(!t.isComeOutRoll());
        CHECK(t.getPoint() == 4);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:recentRolls")
{
    CrapsTable t("Table-1");
    const std::deque<Dice>& recentRolls = t.getRecentRolls();
    CHECK(recentRolls.size() == 0);
    
    for (unsigned roll = 2; roll < 13; roll++)
    {
        unsigned d1; unsigned d2;
        switch(roll)
        {
            case 2:  d1 = 1; d2 = 1; break;
            case 3:  d1 = 1; d2 = 2; break;
            case 4:  d1 = 2; d2 = 2; break;
            case 5:  d1 = 2; d2 = 3; break;
            case 6:  d1 = 3; d2 = 3; break;
            case 7:  d1 = 3; d2 = 4; break;
            case 8:  d1 = 4; d2 = 4; break;
            case 9:  d1 = 5; d2 = 4; break;
            case 10: d1 = 6; d2 = 4; break;
            case 11: d1 = 6; d2 = 5; break;
            case 12: d1 = 6; d2 = 6; break;
        }
        t.testRollDice(d1, d2);
    }

    unsigned expectedRoll = 2;
    for (const auto roll : recentRolls)
    {
        CHECK(roll.value() == expectedRoll++);
    }
}

//----------------------------------------------------------------
