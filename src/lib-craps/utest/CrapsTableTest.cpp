//----------------------------------------------------------------
//
// File: CrapsTableTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>

#include <iostream>
#include <vector>
#include <craps/CrapsTable.h>
#include <craps/EventManager.h>
#include <craps/Player.h>

using namespace Craps;

std::string getTableYamlStringUtest();

//----------------------------------------------------------------

struct CrapsTableFixture
{
    Craps::EventManager em;
    Craps::TableConfig  config;
    
    Player* p1 = nullptr;
    Player* p2 = nullptr;
    Player* p3 = nullptr;
    Player* p4 = nullptr;
    Player* p5 = nullptr;
    Player* p6 = nullptr;
    Player* p7 = nullptr;

    CrapsTableFixture()
    {
        PlayerConfig playerConfig { "playerFilesDirectory" };
        p1 = Player::createPlayer("Player1", playerConfig, em);
        p2 = Player::createPlayer("Player2", playerConfig, em);
        p3 = Player::createPlayer("Player3", playerConfig, em);
        p4 = Player::createPlayer("Player4", playerConfig, em);
        p5 = Player::createPlayer("Player5", playerConfig, em);
        p6 = Player::createPlayer("Player6", playerConfig, em);
        p7 = Player::createPlayer("Player7", playerConfig, em);
        
        config.maxSessions = 50;
        config.maxRecentRolls = 25;
        config.tablePath = "/work/craps/assets/tables/las-vegas-strip.yaml";
        // Unit tests won't overwrite YAML file. CrapsTable dtor
        // does not trigger save to file. Would have to call 
        // prepareToShutdown() or close() to save YAML file.
        // Therefore one TableConfig can be used in all unit tests.
    }

   ~CrapsTableFixture()
    {
        delete p1;
        delete p2;
        delete p3;
        delete p4;
        delete p5;
        delete p6;
        delete p7;
    }
};

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:ctor")
{
    SUBCASE("normalCtor")
    {
        CrapsTable t("Table-1", config, em);
        CHECK(t.getPoint() == 0);
        CHECK(t.getCurrentRoll().value() == 12);
        CHECK(t.isComeOutRoll());
        CHECK(t.isBettingOpen());
        CHECK(t.getCurrentShooter() == nullptr);
        CHECK(t.getNumPlayers() == 0);
    }

    SUBCASE("fromString")
    {
        std::string yaml = getTableYamlStringUtest();
        std::unique_ptr<CrapsTable> t(CrapsTable::fromString(yaml, "Table-1", config, em));
        REQUIRE(t != nullptr);
        CHECK(t->getPoint() == 0);
        CHECK(t->getCurrentRoll().value() == 12);
        CHECK(t->isComeOutRoll());
        CHECK(t->isBettingOpen());
        CHECK(t->getCurrentShooter() == nullptr);
        CHECK(t->getNumPlayers() == 0);
    }

    SUBCASE("fromFile:exists")
    {
        std::unique_ptr<CrapsTable> t(CrapsTable::fromFile("Table-1", config, em));
        REQUIRE(t != nullptr);
        CHECK(t->getPoint() == 0);
        CHECK(t->getCurrentRoll().value() == 12);
        CHECK(t->isComeOutRoll());
        CHECK(t->isBettingOpen());
        CHECK(t->getCurrentShooter() == nullptr);
        CHECK(t->getNumPlayers() == 0);
    }

    SUBCASE("fromFile:missing")
    {
        // Clobber path. Use bad tableId/path so file won't be found.
        CrapsTableFixture::config.tablePath = "missing/FakeTable-1";
        CHECK_THROWS_AS(CrapsTable::fromFile("FakeTable-1", config, em),
                        std::runtime_error);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:players")
{
    SUBCASE("addRemove")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
        Gen::ErrorPass ep;

        CHECK(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 1);
        // Add same player again - error
        CHECK(t->addPlayer(p1, ep) == Gen::ReturnCode::Fail);

        // Add different player
        CHECK(t->addPlayer(p2, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 2);
        
        // Add a bunch
        CHECK(t->addPlayer(p3, ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p4, ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p5, ep) == Gen::ReturnCode::Success);
        CHECK(t->addPlayer(p6, ep) == Gen::ReturnCode::Success);

        // Six players max at a table
        CHECK(t->addPlayer(p7, ep) == Gen::ReturnCode::Fail);

        // Remove player unknown to the table
        CHECK(t->removePlayer(p7, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumPlayers() == 6);

        // Remove from middle
        CHECK(t->removePlayer(p3, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 5);

        // Remove from front
        CHECK(t->removePlayer(p1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 4);

        // Remove from tail
        CHECK(t->removePlayer(p6, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 3);

        // Able to add after removals
        CHECK(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 4);

        // Remove all and still able to add
        CHECK(t->removePlayer(p1, ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p2, ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p3, ep) == Gen::ReturnCode::Fail);
        CHECK(t->removePlayer(p4, ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p5, ep) == Gen::ReturnCode::Success);
        CHECK(t->removePlayer(p6, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumPlayers() == 0);
        CHECK(t->addPlayer(p3, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 1);
        CHECK(t->removePlayer(p3, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumPlayers() == 0);

        // Remove a player with active bets
        // First add the player
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        // Load up several bets
        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field,   10, 0);
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::Hardway, 10, 4);
        auto b3 = std::make_shared<CrapsBet>(p1, BetName::Hardway, 10, 6);
        auto b4 = std::make_shared<CrapsBet>(p1, BetName::CandE,   10, 0);
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
        CHECK(t->removePlayer(p1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 0);
    }

    SUBCASE("playerList")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
        Gen::ErrorPass ep;
        
        REQUIRE(t->getNumPlayers() == 0);

        // Get player list, get back an empty vector
        std::vector<Player*> v;
        v = t->getPlayers();
        CHECK(v.size() == 0);

        // Load up some players
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p3, ep) == Gen::ReturnCode::Success);
        
        // Get back non-empty vector
        v = t->getPlayers();
        CHECK(v.size() == 3);
        CHECK(v[0] == p1);
        CHECK(v[1] == p2);
        CHECK(v[2] == p3);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:placingBets")
{
    Gen::ErrorPass ep;
    
    SUBCASE("general")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));

        // Place a bet but player hasn't yet joined the table
        CHECK(t->getNumPlayers() == 0);

        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Fail);

        // Add two players
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getNumPlayers() == 2);

        // Place a good bet
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 1);

        // Place same bet twice same player
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::Hardway, 10, 8);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumBetsOnTable() == 2);

        // Place same bet type different players
        auto b3 = std::make_shared<CrapsBet>(p1, BetName::Hardway, 10, 10);
        auto b4 = std::make_shared<CrapsBet>(p1, BetName::Hardway, 10, 10);
        REQUIRE(b3 != nullptr);
        REQUIRE(b4 != nullptr);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);

        // Test positive haveBet, use a previous bet
        CHECK(t->haveBet(*b4));
        
        // Test negative haveBet, use a new bet
        auto b5 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
        REQUIRE(b5 != nullptr);
        CHECK(!t->haveBet(*b5));
    }

    SUBCASE("betTiming")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
        
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->isComeOutRoll());

        // Make a Place bet, legal on come out roll
        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Place, 10, 10);
        REQUIRE(b1 != nullptr);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        
        // Make a Buy bet, legal on come out roll
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::Buy, 10, 10);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        
        // Make a Lay bet, legal on come out roll
        auto b3 = std::make_shared<CrapsBet>(p1, BetName::Buy, 10, 10);
        REQUIRE(b3 != nullptr);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        
        // Make a Field bet, legal on come out roll
        auto b4 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        REQUIRE(b4 != nullptr);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);
        
        // Make a Hardway bet, legal on come out roll
        auto b5 = std::make_shared<CrapsBet>(p1, BetName::Hardway, 10, 4);
        REQUIRE(b5 != nullptr);
        CHECK(t->addBet(b5, ep) == Gen::ReturnCode::Success);
        
        // Make a Horn bet, legal on come out roll
        auto b6 = std::make_shared<CrapsBet>(p1, BetName::Horn, 12, 0);
        REQUIRE(b6 != nullptr);
        CHECK(t->addBet(b6, ep) == Gen::ReturnCode::Success);
        
        // Make a C&E bet, legal on come out roll
        auto b7 = std::make_shared<CrapsBet>(p1, BetName::CandE, 10, 0);
        REQUIRE(b7 != nullptr);
        CHECK(t->addBet(b7, ep) == Gen::ReturnCode::Success);
        
        // Make a Come/DontCome bet but wrong time for it
        auto b8 = std::make_shared<CrapsBet>(p1, BetName::Come,     10, 0);
        auto b9 = std::make_shared<CrapsBet>(p1, BetName::DontCome, 10, 0);
        REQUIRE(b8 != nullptr);
        REQUIRE(b9 != nullptr);
        CHECK(t->addBet(b8, ep) == Gen::ReturnCode::Fail);
        CHECK(t->addBet(b9, ep) == Gen::ReturnCode::Fail);

        // Establish a point
        t->testSetState(4,6,6);  // point 4, d1, d2

        // Making PassLine bet after point is established - is legal
        // PassLine bet, pivot will be coerced to point
        auto b10 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
        REQUIRE(b10 != nullptr);
        CHECK(t->addBet(b10, ep) == Gen::ReturnCode::Success);
        CHECK(b10->pivot() == 4);

        // Again but specify a non-zero pivot, also coerced
        auto b11 = std::make_shared<CrapsBet>(p2, BetName::PassLine, 10, 5);
        REQUIRE(b11 != nullptr);
        CHECK(t->addBet(b11, ep) == Gen::ReturnCode::Success);
        CHECK(b11->pivot() == 4);

        // Make a DontPass bet but wrong time for it
        auto b12 = std::make_shared<CrapsBet>(p2, BetName::DontPass, 10, 0);
        REQUIRE(b12 != nullptr);
        CHECK(t->addBet(b12, ep) == Gen::ReturnCode::Fail);

        // Make a Place bet, legal on point roll
        auto b13 = std::make_shared<CrapsBet>(p2, BetName::Place, 10, 10);
        REQUIRE(b13 != nullptr);
        CHECK(t->addBet(b13, ep) == Gen::ReturnCode::Success);
        
        // Make a Buy bet, legal on point roll
        auto b14 = std::make_shared<CrapsBet>(p2, BetName::Buy, 10, 10);
        REQUIRE(b14 != nullptr);
        CHECK(t->addBet(b14, ep) == Gen::ReturnCode::Success);
        
        // Make a Lay bet, legal on point roll
        auto b15 = std::make_shared<CrapsBet>(p2, BetName::Buy, 10, 10);
        REQUIRE(b15 != nullptr);
        CHECK(t->addBet(b15, ep) == Gen::ReturnCode::Success);
        
        // Make a Field bet, legal on point roll
        auto b16 = std::make_shared<CrapsBet>(p2, BetName::Field, 10, 0);
        REQUIRE(b16 != nullptr);
        CHECK(t->addBet(b16, ep) == Gen::ReturnCode::Success);
        
        // Make a Hardway bet, legal on point roll
        auto b17 = std::make_shared<CrapsBet>(p2, BetName::Hardway, 10, 4);
        REQUIRE(b17 != nullptr);
        CHECK(t->addBet(b17, ep) == Gen::ReturnCode::Success);
        
        // Make a Horn bet, legal on point roll
        auto b18 = std::make_shared<CrapsBet>(p2, BetName::Horn, 12, 0);
        REQUIRE(b18 != nullptr);
        CHECK(t->addBet(b18, ep) == Gen::ReturnCode::Success);
        
        // Make a C&E bet, legal on point roll
        auto b19 = std::make_shared<CrapsBet>(p2, BetName::CandE, 10, 0);
        REQUIRE(b19 != nullptr);
        CHECK(t->addBet(b19, ep) == Gen::ReturnCode::Success);
        
        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("minMaxLimits")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
        
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->isComeOutRoll());

        // LineBet too small
        auto b1 = std::make_shared<CrapsBet>(p1, BetName::PassLine,
                                        t->getMinLineBet() - 1, 0);
        REQUIRE(b1 != nullptr);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Fail);
        
        // LineBet too big
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::PassLine,
                                        t->getMaxLineBet() + 1, 0);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Fail);

        // Horn bet too small
        auto b3 = std::make_shared<CrapsBet>(p1, BetName::Horn, 3, 0);
        REQUIRE(b3 != nullptr);
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Fail);

        // Horn bet min is 4, good
        auto b4 = std::make_shared<CrapsBet>(p1, BetName::Horn, 4, 0);
        REQUIRE(b4 != nullptr);
        CHECK(t->addBet(b4, ep) == Gen::ReturnCode::Success);

        // Horn bet must be a multiple of 4
        auto b5 = std::make_shared<CrapsBet>(p1, BetName::Horn, 5, 0);
        REQUIRE(b5 != nullptr);
        CHECK(t->addBet(b5, ep) == Gen::ReturnCode::Fail);

        // Horn bet must be a multiple of 4
        auto b6 = std::make_shared<CrapsBet>(p2, BetName::Horn, 12, 0);
        REQUIRE(b6 != nullptr);
        CHECK(t->addBet(b6, ep) == Gen::ReturnCode::Success);

        // TODO Horn max bet 
        // auto b7 = std::make_shared<CrapsBet>(p2, BetName::Horn,
        //                                 t->getMaxHorn() + 1, 0);
        // REQUIRE(b7 != nullptr);
        // CHECK(t->addBet(b7, ep) == Gen::ReturnCode::Fail);

        // C&E bet too small
        auto b8 = std::make_shared<CrapsBet>(p1, BetName::CandE, 1, 0);
        REQUIRE(b8 != nullptr);
        CHECK(t->addBet(b8, ep) == Gen::ReturnCode::Fail);

        // C&E bet min, good
        auto b9 = std::make_shared<CrapsBet>(p1, BetName::CandE, 2, 0);
        REQUIRE(b9 != nullptr);
        CHECK(t->addBet(b9, ep) == Gen::ReturnCode::Success);

        // C&E bet must be a multiple of 2
        auto b10 = std::make_shared<CrapsBet>(p1, BetName::CandE, 13, 0);
        REQUIRE(b10 != nullptr);
        CHECK(t->addBet(b10, ep) == Gen::ReturnCode::Fail);

        // TODO C&E max bet 
        // auto b11 = std::make_shared<CrapsBet>(p2, BetName::CandE,
        //                                 t->getMaxCandE() + 1, 0);
        // REQUIRE(b11 != nullptr);
        // CHECK(t->addBet(b11, ep) == Gen::ReturnCode::Fail);
        
        // std::cout << ep.diag << std::endl;
    }

    SUBCASE("setOdds")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));

        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addPlayer(p2, ep) == Gen::ReturnCode::Success);

        // Bad type of bet for odds bet
        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->setOddsAmount(b1, 10, ep) == Gen::ReturnCode::Fail);
        
        // Can't set odds if bet is not on the table
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
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
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
        
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);

        // Change contract bet after on table, no point yet.
        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
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
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 4);
        REQUIRE(b2 != nullptr);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        // Increase pass line bet - OK
        CHECK(t->setContractAmount(b2, 15, ep) == Gen::ReturnCode::Success);

        // Decrease pass line bet - Not allowed
        CHECK(t->setContractAmount(b2, 10, ep) == Gen::ReturnCode::Fail);

        auto b3 = std::make_shared<CrapsBet>(p1, BetName::DontPass, 10, 4);
        REQUIRE(b2 != nullptr);
        // Can't bet DontPass on table after point is known
        CHECK(t->addBet(b3, ep) == Gen::ReturnCode::Fail);

        // TODO: add more Pass/Dont tests when table establishes a point.
        // std::cout << ep.diag << std::endl;
        // t->testSetState(4, 6, 6);
    }

    SUBCASE("amountOnTable")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);

        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        CHECK(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 10);

        CHECK(t->setContractAmount(b1, 20, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 20);

        auto b2 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
        CHECK(t->addBet(b2, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 30);
        CHECK(t->setContractAmount(b2, 5, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 25);
    }

    SUBCASE("removeBet")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);

        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getAmountOnTable() == 10);
        REQUIRE(t->getNumBetsOnTable() == 1);
        CHECK(t->removeBet(b1, ep) == Gen::ReturnCode::Success);
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);

        // Remove a bet that was never on the table
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        CHECK(t->removeBet(b2, ep) == Gen::ReturnCode::Fail);
        // std::cout << ep.diag << "\n";

        // Can remove any bet before it participates in a roll
        t->testSetState(4, 6, 6);
        auto b3 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
        REQUIRE(t->addBet(b3, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getAmountOnTable() == 10);
        REQUIRE(t->getNumBetsOnTable() == 1);
        CHECK(t->removeBet(b3, ep) == Gen::ReturnCode::Success);
        // std::cout << ep.diag << "\n";
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(t->getAmountOnTable() == 0);

        // Can't remove passline bet if it participated in a roll
        Gen::ErrorPass ep;
        t->testSetState(0, 6, 6);
        auto b4 = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
        REQUIRE(b4 != nullptr);
        REQUIRE(t->addBet(b4, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getAmountOnTable() == 10);
        REQUIRE(t->getNumBetsOnTable() == 1);
        t->testSetState(10, 6, 6);
        b4->testSetPivot(10);  // avoid dice roll, setup bet to have a point
        b4->testSetDistance(1);// set bet to have participated in a roll
        CHECK(t->removeBet(b4, ep) == Gen::ReturnCode::Fail);
        CHECK(t->getNumBetsOnTable() == 1);
        CHECK(t->getAmountOnTable() == 10);
    }

    SUBCASE("removePlayerActiveBets")
    {
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));

        REQUIRE(t->getAmountOnTable() == 0);
        REQUIRE(t->addPlayer(p1, ep) == Gen::ReturnCode::Success);
        auto b1 = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
        auto b2 = std::make_shared<CrapsBet>(p1, BetName::PassLine,  10, 0);
        REQUIRE(t->addBet(b1, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->addBet(b2, ep) == Gen::ReturnCode::Success);

        CHECK(t->getAmountOnTable() == 20);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(t->removePlayer(p1, ep) == Gen::ReturnCode::Success);
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
        std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
        Gen::ErrorPass ep;
        
        // First roll, no players, it's a come out roll.
        CHECK(t->isComeOutRoll());
        t->testRollDice(6,6);       // roll a 12, success is no crash
        CHECK(t->isComeOutRoll());  // still coming out
        CHECK(t->getCurrentRoll().value() == 12);
        CHECK(t->getCurrentRoll().d1() == 6);
        CHECK(t->getCurrentRoll().d2() == 6);

        REQUIRE(p1->joinTable(t.get(), ep) == Gen::ReturnCode::Success);
        REQUIRE(p2->joinTable(t.get(), ep) == Gen::ReturnCode::Success);
        REQUIRE(t->getNumPlayers() == 2);
        Gen::Money johnBalance = p1->getBalance();
        Gen::Money janeBalance = p2->getBalance();

        // come out roll, roll a 7, pass line win, dont pass lose
        auto johnBet1 = p1->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet1 = p2->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet1 != nullptr);
        REQUIRE(janeBet1 != nullptr);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";

        t->testRollDice(3,4);
        CHECK(p1->getBalance() == (johnBalance + 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p2->getNumBetsOnTable() == 0);
        CHECK(p2->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());

        // come out roll, roll a 11, pass line win, dont pass lose
        johnBalance = p1->getBalance();  // reset
        janeBalance = p2->getBalance();  // reset
        auto johnBet2 = p1->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet2 = p2->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet2 != nullptr);
        REQUIRE(janeBet2 != nullptr);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";

        t->testRollDice(6,5);
        CHECK(p1->getBalance() == (johnBalance + 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p2->getNumBetsOnTable() == 0);
        CHECK(p2->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());

        // come out roll, roll a 2, pass line lose, dont pass win
        johnBalance = p1->getBalance();  // reset
        janeBalance = p2->getBalance();  // reset
        auto johnBet3 = p1->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet3 = p2->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet3 != nullptr);
        REQUIRE(janeBet3 != nullptr);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";

        t->testRollDice(1,1);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance + 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p2->getNumBetsOnTable() == 0);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p2->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());

        // come out roll, roll a 3, pass line lose, dont pass win
        johnBalance = p1->getBalance();  // reset
        janeBalance = p2->getBalance();  // reset
        auto johnBet4 = p1->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet4 = p2->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet4 != nullptr);
        REQUIRE(janeBet4 != nullptr);
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";

        t->testRollDice(1,2);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance + 10));
        CHECK(t->getAmountOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p2->getNumBetsOnTable() == 0);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p2->getAmountOnTable() == 0);
        CHECK(t->isComeOutRoll());
        
        // come out roll, roll a 12, pass line lose, dont pass push
        johnBalance = p1->getBalance();  // reset
        janeBalance = p2->getBalance();  // reset
        auto johnBet5 = p1->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet5 = p2->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet5 != nullptr);
        REQUIRE(janeBet5 != nullptr);
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";

        t->testRollDice(6,6);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        CHECK(t->getAmountOnTable() == 10);
        CHECK(t->getNumBetsOnTable() == 1);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p2->getNumBetsOnTable() == 1);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p2->getAmountOnTable() == 10);
        CHECK(t->isComeOutRoll());

        // Remove bet, allowed
        p2->removeBet(BetName::DontPass, 0, ep);
        CHECK(p2->getNumBetsOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);

        // come out roll, roll a 4, pass line keep, dont pass keep
        johnBalance = p1->getBalance();  // reset
        janeBalance = p2->getBalance();  // reset
        auto johnBet6 = p1->makeBet(BetName::PassLine, 10, 0, ep);
        auto janeBet6 = p2->makeBet(BetName::DontPass, 10, 0, ep);
        REQUIRE(johnBet6 != nullptr);
        REQUIRE(janeBet6 != nullptr);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        std::cout << "John makes PassLine bet\n";
        std::cout << "Jane makes DontPass bet\n";

        t->testRollDice(2,2);
        CHECK(p1->getBalance() == (johnBalance - 10));
        CHECK(p2->getBalance() == (janeBalance - 10));
        CHECK(t->getAmountOnTable() == 20);
        CHECK(t->getNumBetsOnTable() == 2);
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(p2->getNumBetsOnTable() == 1);
        CHECK(p1->getAmountOnTable() == 10);
        CHECK(p2->getAmountOnTable() == 10);
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
    Craps::BetPtr lastBet;
    
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
    std::unique_ptr<CrapsTable> t(new CrapsTable("Table-1", config, em));
    Gen::ErrorPass ep;
    REQUIRE(p1->joinTable(t.get(), ep) == Gen::ReturnCode::Success);
    
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

        Gen::Money balance = p1->getBalance();

        autoBetLoop(rolls, *t, *p1);
        CHECK(t->getAmountOnTable() ==  0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getBalance() == balance + 30 + 18);
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

        Gen::Money balance = p1->getBalance();

        autoBetLoop(rolls, *t, *p1);
        CHECK(t->getAmountOnTable() ==  60);
        CHECK(t->getNumBetsOnTable() == 3);
        CHECK(p1->getBalance() == balance - 80 + 10 - 60 + 10 - 10);
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

        Gen::Money balance = p1->getBalance();

        constexpr unsigned loops = 100000;
        for (int i = 0; i < loops; i++)
        {
            autoBetLoop(rolls, *t, *p1);
        }
        CHECK(t->getAmountOnTable() ==  0);
        CHECK(t->getNumBetsOnTable() == 0);
        int deltaOneLoop = -40 + 20 + 10 + 12;
        CHECK(p1->getBalance() == balance + (loops * deltaOneLoop));
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(CrapsTableFixture, "CrapsTable:recentRolls")
{
    CrapsTable t("Table-1", config, em);
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

std::string
getTableYamlStringUtest()
{
    std::string yaml = R"(
tableId: Table-1
tableName: Las Vegas Strip
shortDescription: A typical table with 5x odds, min $10, max $2,000.
fullDescription: To be supplied. 5% commission on buy bets, taken only after winning.
Rules:
  maxOdds: 5
  minLineBet: 10
  maxLineBet: 2000
  minPlaceBet: 5
  maxPlaceBet: 2000
  minFieldBet: 1
  maxFieldBet: 2000
  minCandEBet: 2
  maxCandEBet: 2000
  minHornBet: 4
  maxHornBet: 2000
Bank:
  originalStartBalance: 3000000
  sessionStartBalance: 3000000
  refillThreshold: 1500000
  refillAmount: 2000000
  bankStats:
    numDeposits: 0
    amtDeposited: 0
    numWithdrawals: 0
    amtWithdrawn: 0
    numRefills: 0
    amtRefilled: 0
    maxAmtDepositedSession: 0
    maxAmtWithdrawnSession: 0
    maxAmtDepositedSessionDate: 2025-06-25T14:30:00.000000000
    maxAmtWithdrawnSessionDate: 2025-06-25T14:30:00.000000000
BetStats:
  totNumBetsAllBets: 0
  totNumWinsAllBets: 0
  totNumLoseAllBets: 0
  totNumKeepAllBets: 0
  totAmtAllBets: 0
  totAmtWinsAllBets: 0
  totAmtLoseAllBets: 0
  totAmtKeepAllBets: 0
  maxAmtBetOneBet: 0
  maxAmtWinOneBet: 0
  maxAmtLoseOneBet: 0
  maxAmtKeepOneBet: 0
  betTypeStats:
    wins:
      AnyCraps:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      AnySeven:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      CandE:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Field:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Horn:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
    lose:
      AnyCraps:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      AnySeven:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Buy9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      CandE:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Come9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontCome9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      DontPass9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Field:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Hard8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Horn:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Lay9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      PassLine9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place10:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place4:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place5:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place6:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place8:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
      Place9:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
  numBetsOneRoll:
    total: 0
    max: 0
  numBetsWinOneRoll:
    total: 0
    max: 0
  numBetsLoseOneRoll:
    total: 0
    max: 0
  numBetsKeepOneRoll:
    total: 0
    max: 0
  amtBetsOneRoll:
    total: 0
    max: 0
  amtBetsWinOneRoll:
    total: 0
    max: 0
  amtBetsLoseOneRoll:
    total: 0
    max: 0
  amtBetsKeepOneRoll:
    total: 0
    max: 0
RollStats:
  numberCounts:
    2:
      count: 0
      maxRepeats: 0
    3:
      count: 0
      maxRepeats: 0
    4:
      count: 0
      maxRepeats: 0
    5:
      count: 0
      maxRepeats: 0
    6:
      count: 0
      maxRepeats: 0
    7:
      count: 0
      maxRepeats: 0
    8:
      count: 0
      maxRepeats: 0
    9:
      count: 0
      maxRepeats: 0
    10:
      count: 998
      maxRepeats: 0
    11:
      count: 0
      maxRepeats: 0
    12:
      count: 0
      maxRepeats: 0
  anyEstPntCnts:
    4:
      count: 0
      maxRepeats: 0
    5:
      count: 0
      maxRepeats: 0
    6:
      count: 0
      maxRepeats: 0
    8:
      count: 0
      maxRepeats: 0
    9:
      count: 0
      maxRepeats: 0
    10:
      count: 0
      maxRepeats: 0
  passPntCnts:
    4:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    5:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    6:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    8:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    9:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    10:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
  dontPassPntCnts:
    4:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    5:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    6:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    8:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    9:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    10:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
  comePntCnts:
    4:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    5:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    6:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    8:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    9:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    10:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
  dontComePntCnts:
    4:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    5:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    6:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    8:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    9:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    10:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
  hardwayCounts:
    4:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    6:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    8:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
    10:
      wins:
        count: 0
        maxRepeats: 0
      lose:
        count: 0
        maxRepeats: 0
  comeOutRolls:
    count: 0
    maxRepeats: 0
  pointRolls:
    count: 0
    maxRepeats: 0
  passWinsComeOut:
    count: 0
    maxRepeats: 0
  passLoseComeOut:
    count: 0
    maxRepeats: 0
  comeWinsComeOut:
    count: 0
    maxRepeats: 0
  comeLoseComeOut:
    count: 0
    maxRepeats: 0
  dontPassWinsComeOut:
    count: 0
    maxRepeats: 0
  dontPassLoseComeOut:
    count: 0
    maxRepeats: 0
  dontComeWinsComeOut:
    count: 0
    maxRepeats: 0
  dontComeLoseComeOut:
    count: 0
    maxRepeats: 0
  fieldBetWins:
    count: 0
    maxRepeats: 0
  fieldBetLose:
    count: 0
    maxRepeats: 0
  sevenOuts:
    count: 0
    maxRepeats: 0
  shooterCounts:
    count: 0
    maxRepeats: 0
  twosOnComeOutRoll:
    count: 0
    maxRepeats: 0
  threesOnComeOutRoll:
    count: 0
    maxRepeats: 0
  sevensOnComeOutRoll:
    count: 0
    maxRepeats: 0
  elevensOnComeOutRoll:
    count: 0
    maxRepeats: 0
  twelvesOnComeOutRoll:
    count: 0
    maxRepeats: 0
  crapsOnComeOutRoll:
    count: 0
    maxRepeats: 0
SessionStats:
  numSessionsAlltime: 25
  firstSessionDate: 2025-06-25T14:30:00.000000000
  longestSessionAlltime: 0d 00:01:04
  history:
    - numBets: 0
      amtIntake: 0
      amtPayout: 0
      numPlayers: 0
      date: 2025-06-25T14:30:00.000000000
      duration: 0d 00:00:08
    - numBets: 0
      amtIntake: 0
      amtPayout: 0
      numPlayers: 0
      date: 2025-07-30T20:37:47.150580981
      duration: 0d 00:00:03
    - numBets: 0
      amtIntake: 0
      amtPayout: 0
      numPlayers: 0
      date: 2025-07-31T06:07:39.571833914
      duration: 0d 00:00:07
    - numBets: 0
      amtIntake: 0
      amtPayout: 0
      numPlayers: 0
      date: 2025-07-31T06:30:08.195767625
      duration: 0d 00:00:02
    - numBets: 0
      amtIntake: 0
      amtPayout: 0
      numPlayers: 0
      date: 2025-07-31T18:46:18.040842656
      duration: 0d 00:00:08
    - numBets: 0
      amtIntake: 0
      amtPayout: 0
      numPlayers: 0
      date: 2025-07-31T18:56:55.378804027
      duration: 0d 00:00:05
)";

    return yaml;
}

//----------------------------------------------------------------
