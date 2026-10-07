//----------------------------------------------------------------
//
// File: PlayerTest.cpp
//
//----------------------------------------------------------------

#include <craps/Player.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTable.h>
#include <craps/CrapsTypes.h>
#include <craps/TableConfig.h>
#include <doctest/doctest.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <zeus/expected.hpp>
#include <iostream>
#include <memory>

using namespace Craps;

std::string getPlayerYamlStringUtest();

//----------------------------------------------------------------

struct PlayerFixture
{
    PlayerConfig config { "/work/craps/assets/players/Player-1.yaml" };
    PlayerId p1Id { "uuid1" };
    PlayerId p2Id { "uuid2" };
    CrapsTable* t;

    PlayerFixture()
    {
        TableConfig tableConfig;
        tableConfig.maxSessions = 50;
        tableConfig.maxRecentRolls = 25;
        tableConfig.tablePath = "tmp/dontcare.yaml";

        t = new CrapsTable("Table-1", tableConfig);
        REQUIRE(t != nullptr);
    }

   ~PlayerFixture()
    {
        delete t;
    }
};

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:ctor")
{
    SUBCASE("via createplayer()")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id, config));
        CHECK(p1->getName() == "uuid1");
        CHECK(p2->getName() == "uuid2");
    }

    SUBCASE("via fromString()")
    {
        std::string yaml = getPlayerYamlStringUtest();
        std::unique_ptr<Player> p1(Player::fromString(yaml, p1Id, config));
        CHECK(p1->getPlayerId() == "uuid1");
        CHECK(p1->getBalance() == 30000);
        // TODO check more fields to matching YAML
    }
    
    SUBCASE("via fromFile()")
    {
        PlayerConfig pc { "/work/craps/assets/players/nathan.yaml" };

        // Good load
        std::string nathanUuid("550e8400-e29b-41d4-a716-446655440000");
        std::unique_ptr<Player> p1(Player::fromFile(nathanUuid, pc));
        CHECK(p1->getPlayerId() == nathanUuid);
        CHECK(p1->getName() == "Nathan");
        CHECK(p1->getBalance() == 30000);

        // Mismatched player ID
        CHECK_THROWS_AS(Player::fromFile(p2Id, pc), std::runtime_error);
    }

    SUBCASE("fromFile:missing")
    {
        // Clobber path. Use bad playerId/path so file won't be found.
        PlayerFixture::config.playerPath = "missing/FakePlayer-1";
        CHECK_THROWS_AS(Player::fromFile(p1Id, config), std::runtime_error);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:joinTable")
{
    SUBCASE("joinTable")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        REQUIRE(p1 != nullptr);

        // Join table success
        auto result = p1->joinTable(*t); CHECK(result);

        // Join same table twice
        result = p1->joinTable(*t); CHECK(!result);
    }
    
    SUBCASE("leaveTable")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        REQUIRE(p1 != nullptr);

        // Leave table without ever joining
        auto result = p1->leaveTable(); CHECK(result);

        // Join, then immediately leave, no intervening activity
        result = p1->joinTable(*t); CHECK(result);
        result = p1->leaveTable();  CHECK(result);
        
        // Leave table, with outstanding bets
        result = p1->joinTable(*t); REQUIRE(result);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->makeBet(BetName::Place,    100, 6); REQUIRE(result2);
        result2 =      p1->makeBet(BetName::Place,    100, 8); REQUIRE(result2);
        result2 =      p1->makeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
        result2 =      p1->makeBet(BetName::Hardway,  100, 4); REQUIRE(result2);
        REQUIRE(p1->getNumBetsOnTable() == 4);
        REQUIRE(p1->getAmountOnTable() == 400);
        REQUIRE(p1->getBalance() == bal - 400);
        result = p1->leaveTable(); CHECK(result);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p1->getBalance() == bal);

        // Leave, with PassLine bets, not allowed to remove normally
        bal = p1->getBalance();
        result = p1->joinTable(*t); CHECK(result);
        REQUIRE(t->getNumBetsOnTable() == 0);
        REQUIRE(p1->getNumBetsOnTable() == 0);
        t->testSetState(0, 5, 5);  // point 0, d1=5, d2=5
        
        // Put down a pass line bet, coming out
        result2 = p1->makeBet(BetName::PassLine, 100, 0);
        BetPtr pBet = result2.value();
        REQUIRE(pBet != nullptr);
        
        t->testRollDice(5,5); // roll a 10, point is 10 
        // Confirm can't remove passline 10 normally
        result = p1->removeBet(pBet->betId()); CHECK(!result);
        REQUIRE(p1->getBalance() == bal - 100);
        result = p1->leaveTable(); CHECK(result);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(p1->getAmountOnTable() == 0);
        CHECK(p1->getBalance() == bal);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:makeBet")
{
    SUBCASE("badBets")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));

        // Must join table first
        auto result = p1->makeBet(BetName::Place, 100, 6); CHECK(!result);

        // Join table
        auto result2 = p1->joinTable(*t); REQUIRE(result2);

        // Insufficient funds
        Gen::Money bal = p1->getBalance();
        result = p1->makeBet(BetName::Place, bal + 100, 6); CHECK(!result);

        // Bad bet, missing pivot for a place bet
        result = p1->makeBet(BetName::Place, 100, 0); CHECK(!result);

        // Bad bet, zero dollar bet
        result = p1->makeBet(BetName::Place, 0, 6); CHECK(!result);
    }
        
    SUBCASE("goodBets")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result2 = p1->joinTable(*t); REQUIRE(result2);

        Gen::Money bal = p1->getBalance();
        auto result = p1->makeBet(BetName::Place,    100, 6); CHECK(result);
             result = p1->makeBet(BetName::Place,    100, 8); CHECK(result);
             result = p1->makeBet(BetName::PassLine, 100, 0); CHECK(result);
             result = p1->makeBet(BetName::Hardway,  100, 4); CHECK(result);
        CHECK(p1->getNumBetsOnTable() == 4);
        CHECK(p1->getAmountOnTable() == 400);
        CHECK(p1->getBalance() == bal - 400);
    }

    SUBCASE("removeBet")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);

        Gen::Money bal = p1->getBalance();
        auto result2 = p1->makeBet(BetName::Place, 100, 6); REQUIRE(result2);
        BetPtr pBet = result2.value();
        REQUIRE(pBet != nullptr);
        REQUIRE(p1->getNumBetsOnTable() == 1);
        REQUIRE(t->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 100);        
        result1 = p1->removeBet(pBet->betId()); CHECK(result1);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getBalance() == bal);

        // Remove bet that doesn't exist
        result1 = p1->removeBet(9999); CHECK(!result1);

        // Remove bet, any bet allowed can be removed before its first roll
        // Force table state to be point rolls
        t->testSetState(4, 5, 5);  // point 4, d1=5, d2=5
        // Put down a pass line bet after point already established
        result2 = p1->makeBet(BetName::PassLine, 100, 4); REQUIRE(result2);
        BetPtr pBet2 = result2.value();
        REQUIRE(pBet2 != nullptr);
        // OK to remove, has not yet participated in a roll
        result1 = p1->removeBet(pBet2->betId()); CHECK(result1);
        CHECK(p1->getBalance() == bal);

        // Remove bet that is not allowed to be removed, fail
        REQUIRE(t->getNumBetsOnTable() == 0);
        REQUIRE(p1->getNumBetsOnTable() == 0);
        t->testSetState(0, 5, 5);  // point 0, d1=5, d2=5
        // Put down a pass line bet, coming out

        result2 = p1->makeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
        BetPtr pBet3 = result2.value();
        REQUIRE(pBet3 != nullptr);
        t->testRollDice(5,5); // roll a 10, point is 10 
        // Error to remove, can't remove passline 10 any more.
        result1 = p1->removeBet(pBet3->betId()); CHECK(!result1);
        CHECK(p1->getBalance() == bal - 100);
// std::cout << ep.diag << std::endl;
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:setOddsAmount")
{
    SUBCASE("goodBet")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        REQUIRE(t->isComeOutRoll());
        Gen::Money bal = p1->getBalance();

        auto result2 = p1->makeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
        auto pBet = result2.value();
        REQUIRE(pBet != nullptr);
        REQUIRE(pBet->oddsAmount() == 0);
        t->testRollDice(5,5); // roll a 10, point is now 10
        CHECK(pBet->oddsAmount() == 0);
        result1 = p1->setOddsAmount(pBet, 200); CHECK(result1);
        CHECK(p1->getBalance() == bal - 300);
        CHECK(p1->getNumBetsOnTable() == 1);
    }
    
    SUBCASE("badBets")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id, config));

        // nullptr
        auto result1 = p1->setOddsAmount(nullptr, 100); CHECK(!result1);

        // Set odds on a bet that doesn't belong to player
        auto pBet2 = std::make_shared<CrapsBet>(p2.get(), BetName::Place, 100, 6);
        REQUIRE(pBet2 != nullptr);
        result1 = p1->setOddsAmount(pBet2, 100); CHECK(!result1);

        // Set odds on a bet that belongs to player, but not joined, programmer error
        auto pBet1 = std::make_shared<CrapsBet>(p1.get(), BetName::Place, 100, 6);
        REQUIRE(pBet1 != nullptr);
        result1 = p1->setOddsAmount(pBet1, 100); CHECK(!result1);

        // Bad bet type for setOdds
        result1 = p1->joinTable(*t); REQUIRE(result1);
        REQUIRE(p1->getNumBetsOnTable() == 0);
        auto result2 = p1->makeBet(BetName::Place, 100, 6); REQUIRE(result2);
        auto pBet3 = result2.value();
        REQUIRE(pBet3 != nullptr);
        result1 = p1->setOddsAmount(pBet3, 100); CHECK(!result1);
        
        // Insufficient funds
        REQUIRE(t->isComeOutRoll());
        result2 = p1->makeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
        auto pBet4 = result2.value();
        REQUIRE(pBet4 != nullptr);
        t->testRollDice(5,5); // roll a 10, point is now 10 
        Gen::Money bal = p1->getBalance();
        result1 = p1->setOddsAmount(pBet4, bal + 100); CHECK(!result1);

        // Table rejects odds bet due to table limit
        Gen::Money tooMuch = (t->getMaxOdds() +1) * 100;
        result1 = p1->setOddsAmount(pBet4, tooMuch); CHECK(!result1);
        CHECK(pBet4->oddsAmount() == 0);
    }
        
    SUBCASE("changeAmount")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        REQUIRE(t->isComeOutRoll());
        Gen::Money bal = p1->getBalance();

        // Establish a good passline bet with $200 odds.
        auto result2 = p1->makeBet(BetName::PassLine, 100, 0);
        auto pBet = result2.value();
        REQUIRE(pBet != nullptr);
        REQUIRE(pBet->oddsAmount() == 0);
        t->testRollDice(5,5); // roll a 10, point is now 10
        REQUIRE(pBet->oddsAmount() == 0);
        result1 = p1->setOddsAmount(pBet, 200); REQUIRE(result1);
        REQUIRE(p1->getBalance() == bal - 300);
        REQUIRE(p1->getNumBetsOnTable() == 1);

        // Change odds amount to 0
        result1 = p1->setOddsAmount(pBet, 0); CHECK(result1);
        CHECK(p1->getBalance() == bal - 100);        

        // Change odds amount to 400
        result1 = p1->setOddsAmount(pBet, 400); CHECK(result1);
        CHECK(p1->getBalance() == bal - 500);        

        // Change odds amount to 100
        result1 = p1->setOddsAmount(pBet, 100); CHECK(result1);
        CHECK(p1->getBalance() == bal - 200);        

        // Change odds amount to 1
        result1 = p1->setOddsAmount(pBet, 1); CHECK(result1);
        CHECK(p1->getBalance() == bal - 101);        
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:decisions")
{
    SUBCASE("processWin")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->makeBet(BetName::Place, 120, 6); REQUIRE(result2);
        auto b1 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        p1->bettingClosed();
        DecisionRecord r1{b1.get(), true, false, 140, 0, 0, 0};
        p1->processWin(r1);
        CHECK(p1->getBalance() == bal + 140);
        CHECK(p1->getNumBetsOnTable() == 0);

        // check player stats
        auto stats = p1->getCurrentStats();
        CHECK(stats.betStats.totNumBetsAllBets == 1);
        CHECK(stats.betStats.totAmtAllBets == 120);
        CHECK(stats.betStats.totNumWinsAllBets == 1);
        CHECK(stats.betStats.totNumLoseAllBets == 0);
        CHECK(stats.betStats.amtBetsWinOneRoll.total == 140);
        
        // Check lastRollStats
        CHECK(p1->getLastRollStats().amountOnTable == 120);
        CHECK(p1->getLastRollStats().amountWin == 140);
        CHECK(p1->getLastRollStats().amountLose == 0);
        CHECK(p1->getLastRollStats().numBetsOnTable == 1);
        CHECK(p1->getLastRollStats().numBetsWin == 1);
        CHECK(p1->getLastRollStats().numBetsLose == 0);
    }

    SUBCASE("processLose")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->makeBet(BetName::Place, 120, 6); REQUIRE(result2);
        auto b1 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        p1->bettingClosed();
        DecisionRecord r1{b1.get(), true, false, 0, 120, 0, 0};
        p1->processLose(r1);
        CHECK(p1->getBalance() == bal - 120);
        CHECK(p1->getNumBetsOnTable() == 0);

        // check player stats
        auto stats = p1->getCurrentStats();
        CHECK(stats.betStats.totNumBetsAllBets == 1);
        CHECK(stats.betStats.totAmtAllBets == 120);
        CHECK(stats.betStats.totNumWinsAllBets == 0);
        CHECK(stats.betStats.totNumLoseAllBets == 1);
        CHECK(stats.betStats.amtBetsLoseOneRoll.total == 120);

        // Check lastRollStats
        CHECK(p1->getLastRollStats().amountOnTable == 120);
        CHECK(p1->getLastRollStats().amountWin == 0);
        CHECK(p1->getLastRollStats().amountLose == 120);
        CHECK(p1->getLastRollStats().numBetsOnTable == 1);
        CHECK(p1->getLastRollStats().numBetsWin == 0);
        CHECK(p1->getLastRollStats().numBetsLose == 1);
    }

    SUBCASE("processKeep")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->makeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
        auto b1 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        DecisionRecord r1{b1.get(), false, true, 0, 0, 0, 0};
        p1->processKeep(r1);
        CHECK(p1->getBalance() == bal - 100);
        CHECK(p1->getNumBetsOnTable() == 1);
    }

    SUBCASE("commission")
    {
        // Buy bet subtracts commission from win
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->makeBet(BetName::Buy, 100, 4); REQUIRE(result2);
        auto b1 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        DecisionRecord r1{b1.get(), true, false, 195, 0, 0, 5};
        p1->processWin(r1);
        CHECK(p1->getBalance() == bal + 195);
        CHECK(p1->getNumBetsOnTable() == 0);

        // Come bets return odds money, make point then roll 7
        bal = p1->getBalance();
        t->testSetState(4, 5, 5);  // point 0, d1=5, d2=5
        result2 = p1->makeBet(BetName::Come, 100, 0); REQUIRE(result2);
        auto b2 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        b2->testSetPivot(10);  // avoid dice roll, setup bet to have a point
        result1 = p1->setOddsAmount(b2, 200); REQUIRE(result1);
        DecisionRecord r2{b2.get(), true, false, 100, 0, 200, 0};
        p1->processWin(r2);
        CHECK(p1->getBalance() == bal + 100 + 200);
        CHECK(p1->getNumBetsOnTable() == 0);
    }
}

//----------------------------------------------------------------

std::string
getPlayerYamlStringUtest()
{
    std::string yaml = R"(
playerId: uuid1
playerName: Elvis
shortDescription: User can be this player
fullDescription: Always plays Pass and automated Come.
Bank:
  originalStartBalance: 30000
  sessionStartBalance: 30000
  refillThreshold: 15000
  refillAmount: 20000
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
  betTypeStats:
      wins:
        AnyCraps:
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
)";

    return yaml;
}

//----------------------------------------------------------------

