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
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Player p1("p1", 1000);
        Gen::ErrorPass ep;

        CHECK(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 1);
        // Add same player again - error
        CHECK(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Fail);
        Player p2("p2", 1000);
        CHECK(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 2);
        Player p3("p3", 1000);
        Player p4("p4", 1000);
        Player p5("p5", 1000);
        Player p6("p6", 1000);
        Player p7("p7", 1000);
        CHECK(t->addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p4.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p5.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p6.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p7.getUuid(), ep) == Gen::ReturnCode::Fail);
        // Remove unknown player
        CHECK(t->removePlayer(p7.getUuid(), ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumPlayers() == 6);

        // Remove from middle
        CHECK(t->removePlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 5);

        // Remove from front
        CHECK(t->removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 4);

        // Remove from tail
        CHECK(t->removePlayer(p6.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 3);

        // Able to add after removals
        CHECK(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 4);

        // Remove all and still able to add
        CHECK(t->removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p3.getUuid(), ep) == Gen::ReturnCode::Fail);
        CHECK(t->removePlayer(p4.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p5.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p6.getUuid(), ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumPlayers() == 0);
        CHECK(t->addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 1);
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
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Gen::ErrorPass ep;
        Player p1("p1", 1001);
        Player p2("p2", 1002);

        // Place a bet but player hasn't yet joined the table
        CHECK(t->getNumPlayers() == 0);

        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Fail);

        // Place a good bet
        CHECK(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 2);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 1);

        // Place a bet but wrong time for it
        CHECK(t->isComeOutRoll());
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::Come,     10, 0);
        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::DontCome, 10, 0);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Fail);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumBetsOnTable() == 1);

        // Place same bet twice
        auto b4 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 8);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumBetsOnTable() == 2);

        // Add a bet, remember it
        auto b5 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t->addBet(b5, ep) == Gen::ReturnCode::Success);
        // Ensure table finds it as a bet.
        CHECK(t->haveBet(*b5));
        CHECK(t->getNumBetsOnTable() == 3);

        // Reject DontPass if there's a point
        t->testSetState(4,6,6);
        auto b6 = make_shared<CrapsBet>(p1.getUuid(), BetName::DontPass, 10, 0);
        CHECK(t->addBet(b6, ep) == Gen::ReturnCode::Fail);

        // OK to bet PassLine if there's a point, pivot will be coerced to point
        auto b7 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 5);
        CHECK(t->addBet(b7, ep) == Gen::ReturnCode::Success);
        CHECK(b7->pivot() == 4);
        CHECK(t->getNumBetsOnTable() == 4);

        // Again, this time with pivot = zero.
        auto b8 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        CHECK(t->addBet(b8, ep) == Gen::ReturnCode::Success);
        CHECK(b8->pivot() == 4);
        CHECK(t->getNumBetsOnTable() == 5);
        // std::cout << ep.diag << std::endl;

        // Add a bet outside the table limits
        auto b9 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,
                                        t->getMinLineBet() - 1, 0);
        CHECK(t->addBet(b9, ep) == Gen::ReturnCode::Fail);

        auto b10 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,
                                        t->getMaxLineBet() + 1, 0);
        CHECK(t->addBet(b10, ep) == Gen::ReturnCode::Fail);
    }

    SUBCASE("changeBets")
    {
        CrapsTable t("Table-1");
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        CHECK(t.addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        // Change contract bet after on table, no point yet.
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t.addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t.isComeOutRoll() == true);
        CHECK(b1->setContractAmount(20, ep) == Gen::ReturnCode::Success);
        CHECK(b1->contractAmount() == 20);

        // Reduce contract amount, no point yet
        CHECK(b1->setContractAmount(15, ep) == Gen::ReturnCode::Success);
        CHECK(b1->contractAmount() == 15);

        // Change contract bet to zero
        CHECK(b1->setContractAmount(0, ep) == Gen::ReturnCode::Fail);
        CHECK(b1->contractAmount() == 15);

        // Change line bet contract amount, after point
        t.testSetState(4,6,6);
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 4);
        REQUIRE(b2 != nullptr);
        CHECK(t.addBet(b2, ep) == Gen::ReturnCode::Success);
        // Increase pass line bet - OK
        CHECK(b2->setContractAmount(15, ep) == Gen::ReturnCode::Success);

        // Decrease pass line bet - Not allowed
        CHECK(b2->setContractAmount(10, ep) == Gen::ReturnCode::Fail);

        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::DontPass, 10, 4);
        REQUIRE(b2 != nullptr);
        // Can't bet DontPass on table after point is known
        CHECK(t.addBet(b3, ep) == Gen::ReturnCode::Fail);

        // TODO: add more Pass/Dont tests when table establishes a point.
        // std::cout << ep.diag << std::endl;
        // t.testSetState(4, 6, 6);
    }

    SUBCASE("amountOnTable")
    {
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 10);

        CHECK(b1->setContractAmount(20, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 20);

        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 30);
        CHECK(b2->setContractAmount(5, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 25);
    }

    SUBCASE("removeBet")
    {
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getAmountOnTable() == 10);
        REQUIRE(t->getNumBetsOnTable() == 1);
        CHECK(t->removeBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);

        // Remove a bet that was never on the table
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t->removeBet(b2, ep) == Gen::ReturnCode::Fail);
        // std::cout << ep.diag << "\n";

        // Can't remove PassLine bet after point established
        t->testSetState(4, 6, 6);
        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        REQUIRE(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 10);
        CHECK(t->getNumBetsOnTable() == 1);
        CHECK(t->removeBet(b3, ep) == Gen::ReturnCode::Fail);
        // std::cout << ep.diag << "\n";
        CHECK(t->getNumBetsOnTable() == 1);
    }

    SUBCASE("removePlayerOutstandingBets")
    {
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,  10, 0);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addBet(b2, ep) == Gen::ReturnCode::Success);

        CHECK(t->getAmountOnTable() == 20);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(t->removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
    }

    SUBCASE("oddsBet")
    {
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Gen::ErrorPass ep;
        Player p1("p1", 1000);

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        // Add odds bet, during come out roll - fail
        REQUIRE(t->isComeOutRoll());
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,  10, 0);
        // First add base PassLine bet
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 10);
        CHECK(t->getNumBetsOnTable() == 1);
        // Now add odds bet, should fail
        CHECK(b1->setOddsAmount(20, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getAmountOnTable() == 10);
        CHECK(t->getNumBetsOnTable() == 1);

        // Reset to 0 bets
        CHECK(t->removeBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 0);

        // Establish a point
        t->testSetState(4, 6, 6);

        // Create a bet not on table, table rejects odds bet
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,  10, 0);
        REQUIRE(!t->haveBet(*b2));
        CHECK(b2->setOddsAmount(20, ep) == Gen::ReturnCode::Fail);
        CHECK(b2->oddsAmount() == 0);
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);

        // Add a PassLine bet. pivot is focused on point 4.
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        // Make an odds bet with zero amount, does nothing, but allowed
        CHECK(b2->setOddsAmount(0, ep) == Gen::ReturnCode::Success);

        // Make a valid PassLine odds bet 
        CHECK(b2->setOddsAmount(10, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 20);

        // Remove odds bet
        CHECK(b2->setOddsAmount(0, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 10);

        // Add odds back in again
        CHECK(b2->setOddsAmount(10, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 20);

        // TODO. Do same for dont side.
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
        auto* t = CrapsTable::fromConfig("Table-1");
        std::unique_ptr<CrapsTable> tt(t);  // For auto cleanup
        Gbl::pTable = t;
        Gen::ErrorPass ep;

        // First roll, no players, it's a come out roll.
        CHECK(t->isComeOutRoll());
        t->testRollDice(6,6);       // roll a 12, success is no crash
        CHECK(t->isComeOutRoll());  // still coming out
        CHECK(t->getCurRoll().value() == 12);
        CHECK(t->getCurRoll().d1() == 6);
        CHECK(t->getCurRoll().d2() == 6);

        // Create and add a couple of players
        Ctrl::PlayerManager::PlayerPtr john = Gbl::pPlayerMgr->createPlayer("John");
        Ctrl::PlayerManager::PlayerPtr jane = Gbl::pPlayerMgr->createPlayer("Jane");
        CHECK(john->joinTable(ep) == Gen::ReturnCode::Success);
        CHECK(jane->joinTable(ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 2);
        Gen::Money johnBalance = john->getBalance();
        Gen::Money janeBalance = jane->getBalance();

        // come out roll, roll a 7, pass line win, dont pass lose
        auto johnBet1 = john->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet1 = jane->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet1 != nullptr);
        REQUIRE(janeBet1 != nullptr);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";
        CHECK(t->addBet(johnBet1, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(janeBet1, ep) == Gen::ReturnCode::Success);

        t->testRollDice(3,4);
        CHECK(john->getBalance() == (johnBalance + 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());

        // come out roll, roll a 11, pass line win, dont pass lose
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        auto johnBet2 = john->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet2 = jane->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet2 != nullptr);
        REQUIRE(janeBet2 != nullptr);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";
        CHECK(t->addBet(johnBet2, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(janeBet2, ep) == Gen::ReturnCode::Success);

        t->testRollDice(6,5);
        CHECK(john->getBalance() == (johnBalance + 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());

        // come out roll, roll a 2, pass line lose, dont pass win
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        auto johnBet3 = john->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet3 = jane->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet3 != nullptr);
        REQUIRE(janeBet3 != nullptr);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";
        CHECK(t->addBet(johnBet3, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(janeBet3, ep) == Gen::ReturnCode::Success);

        t->testRollDice(1,1);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance + 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());

        // come out roll, roll a 3, pass line lose, dont pass win
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        auto johnBet4 = john->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet4 = jane->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet4 != nullptr);
        REQUIRE(janeBet4 != nullptr);
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";
        CHECK(t->addBet(johnBet4, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(janeBet4, ep) == Gen::ReturnCode::Success);

        t->testRollDice(1,2);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == (janeBalance + 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());
        
        // come out roll, roll a 12, pass line lose, dont pass push
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        auto johnBet5 = john->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet5 = jane->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet5 != nullptr);
        REQUIRE(janeBet5 != nullptr);
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";
        CHECK(t->addBet(johnBet5, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(janeBet5, ep) == Gen::ReturnCode::Success);

        t->testRollDice(6,6);
        CHECK(john->getBalance() == (johnBalance - 10));
        CHECK(jane->getBalance() == janeBalance - 10);
        CHECK(t->getAmountOnTable() == 10);
        CHECK(t->getNumBetsOnTable() == 1);
        CHECK(john->getNumBetsOnTable() == 0);
        CHECK(jane->getNumBetsOnTable() == 1);
        CHECK(john->getAmountOnTable() == 0);
        CHECK(jane->getAmountOnTable() == 10);
        CHECK(t->isComeOutRoll());

        // Remove bet, allowed
        jane->removeBet(BetName::DontPass, 0, ep);
        CHECK(jane->getNumBetsOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);

        // come out roll, roll a 4, pass line kepp, dont pass keep
        johnBalance = john->getBalance();  // reset
        janeBalance = jane->getBalance();  // reset
        auto johnBet6 = john->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet6 = jane->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet6 != nullptr);
        REQUIRE(janeBet6 != nullptr);
        CHECK(john->getBalance() == johnBalance - 10);
        CHECK(jane->getBalance() == janeBalance - 10);
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";
        CHECK(t->addBet(johnBet6, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(janeBet6, ep) == Gen::ReturnCode::Success);

        t->testRollDice(2,2);
        CHECK(john->getBalance() == johnBalance - 10);
        CHECK(jane->getBalance() == janeBalance - 10);
        CHECK(t->getAmountOnTable() == 20);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(john->getNumBetsOnTable() == 1);
        CHECK(jane->getNumBetsOnTable() == 1);
        CHECK(john->getAmountOnTable() == 10);
        CHECK(jane->getAmountOnTable() == 10);
        CHECK(!t->isComeOutRoll());
        CHECK(t->getPoint() == 4);
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
