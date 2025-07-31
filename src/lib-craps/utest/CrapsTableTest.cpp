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
    Craps::TableConfig config;

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
        
        config.maxSessions = 50;
        config.maxRecentRolls = 25;
        config.tablePath = "/work/craps/assets/tables/Table-1.yaml";
        // Unit tests won't overwrite YAML file. CrapsTable dtor
        // does not trigger save to filel Would have to call 
        // prepareToShutdown() or close() to save YAML file.
        // Therefore same TableConfig can be used everywhere.
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
        CrapsTable t("Table-1", CrapsTableFixture::config);
        CHECK(t.getPoint() == 0);
        CHECK(t.getCurRoll().value() == 12);
        CHECK(t.isComeOutRoll());
        CHECK(t.isBettingOpen());
        CHECK(t.getShooterId().empty());
        CHECK(t.getNumPlayers() == 0);
    }

    SUBCASE("fromConfig")
    {
        auto* t = CrapsTable::fromConfig("Table-1", CrapsTableFixture::config);
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
        CrapsTable* p = CrapsTable::fromFile("Table-1", CrapsTableFixture::config);
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
        // Clobber path. Use bad tableId/path so file won't be found.
        CrapsTableFixture::config.tablePath = "missing/FakeTable-1";
        CHECK_THROWS_AS(CrapsTable::fromFile(
                        "FakeTable-1", CrapsTableFixture::config),
                        std::runtime_error);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:players")
{
    SUBCASE("addRemove")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        Player p1("p1", 1000);
        Player p2("p2", 1000);
        Player p3("p3", 1000);
        Player p4("p4", 1000);
        Player p5("p5", 1000);
        Player p6("p6", 1000);
        Player p7("p7", 1000);
        Gen::ErrorPass ep;

        CHECK(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 1);
        // Add same player again - error
        CHECK(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Fail);

        // Add different player
        CHECK(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 2);
        
        // Add a bunch
        CHECK(t->addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p4.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p5.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p6.getUuid(), ep) == Gen::ReturnCode::Success);

        // Six players max at a table
        CHECK(t->addPlayer(p7.getUuid(), ep) == Gen::ReturnCode::Fail);

        // Remove player unknown to the table
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
        CHECK(t->removePlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 0);

        // Remove a player with active bets
        // First add the player
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        // Load up several bets
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field,   10, 0);
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 4);
        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 6);
        auto b4 = make_shared<CrapsBet>(p1.getUuid(), BetName::CandE,   10, 0);
        REQUIRE(b1 != nullptr);
        REQUIRE(b2 != nullptr);
        REQUIRE(b3 != nullptr);
        REQUIRE(b4 != nullptr);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addBet(b4, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getNumBetsOnTable() == 4);
        // Remove player
        CHECK(t->removePlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 0);
    }

    SUBCASE("playerList")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        Player p1("p1", 1000);
        Player p2("p2", 1000);
        Player p3("p3", 1000);
        Gen::ErrorPass ep;
        
        REQUIRE(t->getNumPlayers() == 0);

        // Get player list, get back an empty vector
        std::vector<Gen::Uuid> v;
        v = t->getPlayers();
        CHECK(v.size() == 0);

        // Load up some players
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p3.getUuid(), ep) == Gen::ReturnCode::Success);
        
        // Get back non-empty vector
        v = t->getPlayers();
        CHECK(v.size() == 3);
        CHECK(v[0] == p1.getUuid());
        CHECK(v[1] == p2.getUuid());
        CHECK(v[2] == p3.getUuid());
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:placingBets")
{
    Player p1("p1", 1001);
    Player p2("p2", 1002);
    Gen::ErrorPass ep;
    
    SUBCASE("general")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));

        // Place a bet but player hasn't yet joined the table
        CHECK(t->getNumPlayers() == 0);

        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Fail);

        // Add two players
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getNumPlayers() == 2);

        // Place a good bet
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 1);

        // Place same bet twice same player
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 8);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumBetsOnTable() == 2);

        // Place same bet type different players
        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 10);
        auto b4 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 10);
        REQUIRE(b3 != nullptr);
        REQUIRE(b4 != nullptr);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);

        // Test positive haveBet, use a previous bet
        CHECK(t->haveBet(*b4));
        
        // Test negative haveBet, use a new bet
        auto b5 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        REQUIRE(b5 != nullptr);
        CHECK(!t->haveBet(*b5));
    }

    SUBCASE("betTiming")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->isComeOutRoll());

        // Make a Place bet, legal on come out roll
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Place, 10, 10);
        REQUIRE(b1 != nullptr);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        
        // Make a Buy bet, legal on come out roll
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::Buy, 10, 10);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        
        // Make a Lay bet, legal on come out roll
        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::Buy, 10, 10);
        REQUIRE(b3 != nullptr);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        
        // Make a Field bet, legal on come out roll
        auto b4 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        REQUIRE(b4 != nullptr);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);
        
        // Make a Hardway bet, legal on come out roll
        auto b5 = make_shared<CrapsBet>(p1.getUuid(), BetName::Hardway, 10, 4);
        REQUIRE(b5 != nullptr);
        CHECK(t->addBet(b5, ep) == Gen::ReturnCode::Success);
        
        // Make a Horn bet, legal on come out roll
        auto b6 = make_shared<CrapsBet>(p1.getUuid(), BetName::Horn, 12, 0);
        REQUIRE(b6 != nullptr);
        CHECK(t->addBet(b6, ep) == Gen::ReturnCode::Success);
        
        // Make a C&E bet, legal on come out roll
        auto b7 = make_shared<CrapsBet>(p1.getUuid(), BetName::CandE, 10, 0);
        REQUIRE(b7 != nullptr);
        CHECK(t->addBet(b7, ep) == Gen::ReturnCode::Success);
        
        // Make a Come/DontCome bet but wrong time for it
        auto b8 = make_shared<CrapsBet>(p1.getUuid(), BetName::Come,     10, 0);
        auto b9 = make_shared<CrapsBet>(p1.getUuid(), BetName::DontCome, 10, 0);
        REQUIRE(b8 != nullptr);
        REQUIRE(b9 != nullptr);
        CHECK(t->addBet(b8, ep) == Gen::ReturnCode::Fail);
        CHECK(t->addBet(b9, ep) == Gen::ReturnCode::Fail);

        // Establish a point
        t->testSetState(4,6,6);  // point 4, d1, d2

        // Making PassLine bet after point is established - is legal
        // PassLine bet, pivot will be coerced to point
        auto b10 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        REQUIRE(b10 != nullptr);
        CHECK(t->addBet(b10, ep) == Gen::ReturnCode::Success);
        CHECK(b10->pivot() == 4);

        // Again but specify a non-zero pivot, also coerced
        auto b11 = make_shared<CrapsBet>(p2.getUuid(), BetName::PassLine, 10, 5);
        REQUIRE(b11 != nullptr);
        CHECK(t->addBet(b11, ep) == Gen::ReturnCode::Success);
        CHECK(b11->pivot() == 4);

        // Make a DontPass bet but wrong time for it
        auto b12 = make_shared<CrapsBet>(p2.getUuid(), BetName::DontPass, 10, 0);
        REQUIRE(b12 != nullptr);
        CHECK(t->addBet(b12, ep) == Gen::ReturnCode::Fail);

        // Make a Place bet, legal on point roll
        auto b13 = make_shared<CrapsBet>(p2.getUuid(), BetName::Place, 10, 10);
        REQUIRE(b13 != nullptr);
        CHECK(t->addBet(b13, ep) == Gen::ReturnCode::Success);
        
        // Make a Buy bet, legal on point roll
        auto b14 = make_shared<CrapsBet>(p2.getUuid(), BetName::Buy, 10, 10);
        REQUIRE(b14 != nullptr);
        CHECK(t->addBet(b14, ep) == Gen::ReturnCode::Success);
        
        // Make a Lay bet, legal on point roll
        auto b15 = make_shared<CrapsBet>(p2.getUuid(), BetName::Buy, 10, 10);
        REQUIRE(b15 != nullptr);
        CHECK(t->addBet(b15, ep) == Gen::ReturnCode::Success);
        
        // Make a Field bet, legal on point roll
        auto b16 = make_shared<CrapsBet>(p2.getUuid(), BetName::Field, 10, 0);
        REQUIRE(b16 != nullptr);
        CHECK(t->addBet(b16, ep) == Gen::ReturnCode::Success);
        
        // Make a Hardway bet, legal on point roll
        auto b17 = make_shared<CrapsBet>(p2.getUuid(), BetName::Hardway, 10, 4);
        REQUIRE(b17 != nullptr);
        CHECK(t->addBet(b17, ep) == Gen::ReturnCode::Success);
        
        // Make a Horn bet, legal on point roll
        auto b18 = make_shared<CrapsBet>(p2.getUuid(), BetName::Horn, 12, 0);
        REQUIRE(b18 != nullptr);
        CHECK(t->addBet(b18, ep) == Gen::ReturnCode::Success);
        
        // Make a C&E bet, legal on point roll
        auto b19 = make_shared<CrapsBet>(p2.getUuid(), BetName::CandE, 10, 0);
        REQUIRE(b19 != nullptr);
        CHECK(t->addBet(b19, ep) == Gen::ReturnCode::Success);
        
        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("minMaxLimits")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->isComeOutRoll());

        // LineBet too small
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,
                                        t->getMinLineBet() - 1, 0);
        REQUIRE(b1 != nullptr);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Fail);
        
        // LineBet too big
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine,
                                        t->getMaxLineBet() + 1, 0);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Fail);

        // Horn bet too small
        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::Horn, 3, 0);
        REQUIRE(b3 != nullptr);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Fail);

        // Horn bet min is 4, good
        auto b4 = make_shared<CrapsBet>(p1.getUuid(), BetName::Horn, 4, 0);
        REQUIRE(b4 != nullptr);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);

        // Horn bet must be a multiple of 4
        auto b5 = make_shared<CrapsBet>(p1.getUuid(), BetName::Horn, 5, 0);
        REQUIRE(b5 != nullptr);
        CHECK(t->addBet(b5, ep) == Gen::ReturnCode::Fail);

        // Horn bet must be a multiple of 4
        auto b6 = make_shared<CrapsBet>(p2.getUuid(), BetName::Horn, 12, 0);
        REQUIRE(b6 != nullptr);
        CHECK(t->addBet(b6, ep) == Gen::ReturnCode::Success);

        // TODO Horn max bet 
        // auto b7 = make_shared<CrapsBet>(p2.getUuid(), BetName::Horn,
        //                                 t->getMaxHorn() + 1, 0);
        // REQUIRE(b7 != nullptr);
        // CHECK(t->addBet(b7, ep) == Gen::ReturnCode::Fail);

        // C&E bet too small
        auto b8 = make_shared<CrapsBet>(p1.getUuid(), BetName::CandE, 1, 0);
        REQUIRE(b8 != nullptr);
        CHECK(t->addBet(b8, ep) == Gen::ReturnCode::Fail);

        // C&E bet min, good
        auto b9 = make_shared<CrapsBet>(p1.getUuid(), BetName::CandE, 2, 0);
        REQUIRE(b9 != nullptr);
        CHECK(t->addBet(b9, ep) == Gen::ReturnCode::Success);

        // C&E bet must be a multiple of 2
        auto b10 = make_shared<CrapsBet>(p1.getUuid(), BetName::CandE, 13, 0);
        REQUIRE(b10 != nullptr);
        CHECK(t->addBet(b10, ep) == Gen::ReturnCode::Fail);

        // TODO C&E max bet 
        // auto b11 = make_shared<CrapsBet>(p2.getUuid(), BetName::CandE,
        //                                 t->getMaxCandE() + 1, 0);
        // REQUIRE(b11 != nullptr);
        // CHECK(t->addBet(b11, ep) == Gen::ReturnCode::Fail);
        
        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("setOdds")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2.getUuid(), ep) == Gen::ReturnCode::Success);

        // Bad type of bet for odds bet
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->setOddsAmount(b1, 10, ep) == Gen::ReturnCode::Fail);
        
        // Can't set odds if bet is not on the table
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        REQUIRE(b2 != nullptr);
        CHECK(t->setOddsAmount(b2, 10, ep) == Gen::ReturnCode::Fail);

        // Bet is on table, no point established, odds bet disallowed
        REQUIRE(t->isComeOutRoll());
        REQUIRE(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        CHECK(t->setOddsAmount(b2, 10, ep) == Gen::ReturnCode::Fail);

        // Establish a point
        t->testSetState(4,6,6);  // point 4, d1, d2
        
        // OK now to set min odds, $0 is allowed
        b2->testSetPivot(4);  // not rolling dice, so setup bet properly
        CHECK(t->setOddsAmount(b2, 0, ep) == Gen::ReturnCode::Success);

        // OK now to set min odds, $1 is allowed
        CHECK(t->setOddsAmount(b2, 1, ep) == Gen::ReturnCode::Success);

        // Odds bet too big ($10 contract, point is 4)
        unsigned tooMuch = (t->getMaxOdds() * 10) + 1;
        CHECK(t->setOddsAmount(b2, tooMuch, ep) == Gen::ReturnCode::Fail);

        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("modifyBets")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        // Change contract bet after on table, no point yet.
        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->isComeOutRoll() == true);
        CHECK(t->setContractAmount(b1, 20, ep) == Gen::ReturnCode::Success);
        CHECK(b1->contractAmount() == 20);

        // Reduce contract amount, no point yet
        CHECK(t->setContractAmount(b1, 15, ep) == Gen::ReturnCode::Success);
        CHECK(b1->contractAmount() == 15);

        // Change contract bet to zero
        CHECK(t->setContractAmount(b1, 0, ep) == Gen::ReturnCode::Fail);
        CHECK(b1->contractAmount() == 15);

        // Change line bet contract amount, after point
        t->testSetState(4,6,6);
        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 4);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        // Increase pass line bet - OK
        CHECK(t->setContractAmount(b2, 15, ep) == Gen::ReturnCode::Success);

        // Decrease pass line bet - Not allowed
        CHECK(t->setContractAmount(b2, 10, ep) == Gen::ReturnCode::Fail);

        auto b3 = make_shared<CrapsBet>(p1.getUuid(), BetName::DontPass, 10, 4);
        REQUIRE(b2 != nullptr);
        // Can't bet DontPass on table after point is known
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Fail);

        // TODO: add more Pass/Dont tests when table establishes a point.
        // std::cout << ep.diag << std::endl;
        // t->testSetState(4, 6, 6);
    }

    SUBCASE("amountOnTable")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1.getUuid(), ep) == Gen::ReturnCode::Success);

        auto b1 = make_shared<CrapsBet>(p1.getUuid(), BetName::Field, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 10);

        CHECK(t->setContractAmount(b1, 20, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 20);

        auto b2 = make_shared<CrapsBet>(p1.getUuid(), BetName::PassLine, 10, 0);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 30);
        CHECK(t->setContractAmount(b2, 5, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 25);
    }

    SUBCASE("removeBet")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
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
        REQUIRE(t->getAmountOnTable() == 10);
        REQUIRE(t->getNumBetsOnTable() == 1);
        CHECK(t->removeBet(b3, ep) == Gen::ReturnCode::Fail);
        // std::cout << ep.diag << "\n";
        CHECK(t->getNumBetsOnTable() == 1);
        CHECK(t->getAmountOnTable() == 10);
    }

    SUBCASE("removePlayerActiveBets")
    {
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));

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
        std::unique_ptr<CrapsTable> t(
            CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
        Gbl::pTable = t.get();
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

        // come out roll, roll a 4, pass line keep, dont pass keep
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

enum class Action
{
    RollDice,
    MakeBet,
    SetOdds
};

struct AutoBet
{
    Action action;  // MakeBet, JustRoll, SetOdds
    unsigned d1;
    unsigned d2;
    BetName betName;
    unsigned pivot;
    Gen::Money amount;
};

using AutoRolls = std::vector<AutoBet>;

void autoBetLoop(AutoRolls& rolls, CrapsTable& t, Player& player)
{
    Gen::ErrorPass ep;
    CrapsBet::BetPtr lastBet;
    
    for (const auto& r : rolls)
    {
        if (r.action == Action::RollDice)
        {
            t.testRollDice(r.d1, r.d2);
        }
        if (r.action == Action::MakeBet)
        {
            auto bet = player.makeBet(
                r.betName, r.amount, r.pivot, ep);
            lastBet = bet;
        }
        if (r.action == Action::SetOdds)
        {
            REQUIRE(player.setOddsAmount(lastBet, r.amount, ep) == Gen::ReturnCode::Success);
        }
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:rolls")
{
    std::unique_ptr<CrapsTable> t(
        CrapsTable::fromConfig("Table-1", CrapsTableFixture::config));
    Gbl::pTable = t.get();
    Gen::ErrorPass ep;
    Ctrl::PlayerManager::PlayerPtr sam = Gbl::pPlayerMgr->createPlayer("Sam");
    REQUIRE(sam->joinTable(ep) == Gen::ReturnCode::Success);
    
    SUBCASE("shortRoll")
    {
        AutoRolls rolls =
        {
            AutoBet{Action::MakeBet,  0, 0, BetName::PassLine, 0, 10},
            AutoBet{Action::RollDice, 2, 2, BetName::Invalid,  0,  0},
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10},
            AutoBet{Action::MakeBet,  0, 0, BetName::Place,    4, 10},
            AutoBet{Action::RollDice, 2, 2, BetName::Invalid,  0,  0},
        };

        Gen::Money balance = sam->getBalance();

        autoBetLoop(rolls, *t, *sam);
        CHECK(t->getAmountOnTable() ==  0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(sam->getBalance() == balance + 30 + 18);
    }

    SUBCASE("ascendingRoll")
    {
        AutoRolls rolls =  // 2,3,4,5,6,7,8,9,10,11,12
        {
            AutoBet{Action::MakeBet,  0, 0, BetName::PassLine, 0, 10}, 
            AutoBet{Action::RollDice, 1, 1, BetName::Invalid,  0,  0}, // lose 10
            AutoBet{Action::MakeBet,  0, 0, BetName::PassLine, 0, 10},
            AutoBet{Action::RollDice, 1, 2, BetName::Invalid,  0,  0}, // lose 10
            AutoBet{Action::MakeBet,  0, 0, BetName::PassLine, 0, 10}, // 
            AutoBet{Action::RollDice, 2, 2, BetName::Invalid,  0,  0}, // point 4
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 2, 3, BetName::Invalid,  0,  0}, // come 5
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 3, 3, BetName::Invalid,  0,  0}, // come 6
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 3, 4, BetName::Invalid,  0,  0}, // 7 out lose 80, win 20
            AutoBet{Action::MakeBet,  0, 0, BetName::PassLine, 0, 10}, 
            AutoBet{Action::RollDice, 4, 4, BetName::Invalid,  0,  0}, // point 8
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 4, 5, BetName::Invalid,  0,  0}, // come 9
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 5, 5, BetName::Invalid,  0,  0}, // come 10
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 5, 6, BetName::Invalid,  0,  0}, // lose 10
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come
            AutoBet{Action::RollDice, 6, 6, BetName::Invalid,  0,  0}, // lose 10
        };

        Gen::Money balance = sam->getBalance();

        autoBetLoop(rolls, *t, *sam);
        CHECK(t->getAmountOnTable() ==  60);
        CHECK(t->getNumBetsOnTable() == 3);
        CHECK(sam->getBalance() == balance - 80 + 10 - 60 + 10 - 10);
    }

    SUBCASE("superLong")
    {
        AutoRolls rolls =  // 6,8,8,7 repeat 100,000 times
        {
            AutoBet{Action::MakeBet,  0, 0, BetName::PassLine, 0, 10}, 
            AutoBet{Action::RollDice, 3, 3, BetName::Invalid,  0,  0}, // point 6
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds 6
            AutoBet{Action::MakeBet,  0, 0, BetName::Come,     0, 10}, // come bet
            AutoBet{Action::RollDice, 4, 4, BetName::Invalid,  0,  0}, // come 8
            AutoBet{Action::SetOdds,  0, 0, BetName::Invalid,  0, 10}, // odds 8
            AutoBet{Action::RollDice, 4, 4, BetName::Invalid,  0,  0}, // win  8
            AutoBet{Action::RollDice, 3, 4, BetName::Invalid,  0,  0}, // 7 out lost 6
        };

        Gen::Money balance = sam->getBalance();

        constexpr unsigned loops = 100000;
        for (int i = 0; i < loops; i++)
        {
            autoBetLoop(rolls, *t, *sam);
        }
        CHECK(t->getAmountOnTable() ==  0);
        CHECK(t->getNumBetsOnTable() == 0);
        int deltaOneLoop = -40 + 20 + 10 + 12;
        CHECK(sam->getBalance() == balance + (loops * deltaOneLoop));
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:recentRolls")
{
    CrapsTable t("Table-1", CrapsTableFixture::config);
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
