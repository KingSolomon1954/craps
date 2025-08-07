//----------------------------------------------------------------
//
// File: PlayerTest.cpp
//
//----------------------------------------------------------------

#include <iostream>
#include <memory>
#include <craps/Player.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTable.h>
#include <craps/CrapsTypes.h>
#include <craps/EventManager.h>
#include <craps/TableConfig.h>
#include <doctest/doctest.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

using namespace Craps;

std::string getPlayerYamlStringUtest();

//----------------------------------------------------------------

struct PlayerFixture
{
    EventManager em;
    PlayerConfig config { "/work/craps/assets/players/Player-1.yaml" };
    PlayerId p1Id { "uuid1" };
    PlayerId p2Id { "uuid2" };
    CrapsTable* t;
    CrapsTable* pf1;

    PlayerFixture()
    {
        TableConfig tableConfig;
        tableConfig.maxSessions = 50;
        tableConfig.maxRecentRolls = 25;
        tableConfig.tablePath = "/dontcare.yaml";

        t = new CrapsTable("Table-1", tableConfig, em);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id, config, em));
        CHECK(p1->getName() == "uuid1");
        CHECK(p2->getName() == "uuid2");
    }

    SUBCASE("via fromString()")
    {
        std::string yaml = getPlayerYamlStringUtest();
        std::unique_ptr<Player> p1(Player::fromString(yaml, p1Id, config, em));
        std::unique_ptr<Player> p2(Player::fromString(yaml, p2Id, config, em));
        CHECK(p1->getPlayerId() == "uuid1");
        CHECK(p2->getPlayerId() == "uuid2");
        // TODO check fields to matching YAML
    }
    
    SUBCASE("via fromFile()")
    {
#if 0        
        // TODO - need a player.yaml file
        std::unique_ptr<Player> p1(Player::fromFile(p1Id, config, em));
        std::unique_ptr<Player> p2(Player::fromFile(p2Id, config, em));
        CHECK(p1->getPlayerId() == "uuid1");
        CHECK(p2->getPlayerId() == "uuid2");
#endif        
    }

    SUBCASE("fromFile:missing")
    {
        // Clobber path. Use bad playerId/path so file won't be found.
        PlayerFixture::config.playerPath = "missing/FakePlayer-1";
        CHECK_THROWS_AS(Player::fromFile("uuid1", config, em),
                        std::runtime_error);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:makeBet")
{
    SUBCASE("badBets")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));

        // Must join table first
        CHECK(p1->makeBet(BetName::Place, 100, 6, ep) == nullptr);

        // Join table
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);

        // Insufficient funds
        Gen::Money bal = p1->getBalance();
        CHECK(p1->makeBet(BetName::Place, bal + 100, 6, ep) == nullptr);

        // Bad bet, missing pivot for a place bet
        CHECK(p1->makeBet(BetName::Place, 100, 0, ep) == nullptr);

        // Bad bet, zero dollar bet
        CHECK(p1->makeBet(BetName::Place, 0, 6, ep) == nullptr);
    }
        
    SUBCASE("goodBets")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);

        Gen::Money bal = p1->getBalance();
        CHECK(p1->makeBet(BetName::Place,    100, 6, ep) != nullptr);
        CHECK(p1->makeBet(BetName::Place,    100, 8, ep) != nullptr);
        CHECK(p1->makeBet(BetName::PassLine, 100, 0, ep) != nullptr);
        CHECK(p1->makeBet(BetName::Hardway,  100, 4, ep) != nullptr);
        CHECK(p1->getNumBetsOnTable() == 4);
        CHECK(p1->getAmountOnTable() == 400);
        CHECK(p1->getBalance() == bal - 400);
    }

    SUBCASE("removeBet")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);

        Gen::Money bal = p1->getBalance();
        REQUIRE(p1->makeBet(BetName::Place, 100, 6, ep) != nullptr);
        REQUIRE(p1->getNumBetsOnTable() == 1);
        REQUIRE(t->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 100);        
        CHECK(p1->removeBet(BetName::Place, 6, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getNumBetsOnTable() == 0);
        CHECK(t->getNumBetsOnTable() == 0);
        CHECK(p1->getBalance() == bal);

        // Remove bet that doesn't exist
        CHECK(p1->removeBet(BetName::Place, 6, ep) == Gen::ReturnCode::Fail);

        // Remove bet, allowed to be removed before any roll
        // Force table state to be point rolls
        t->testSetState(4, 5, 5);  // point 4, d1=5, d2=5
        // Put down a pass line bet after point already established
        REQUIRE(p1->makeBet(BetName::PassLine, 100, 4, ep) != nullptr);
        // OK to remove, has not yet participated in a roll
        CHECK(p1->removeBet(BetName::PassLine, 4, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getBalance() == bal);

        // Remove bet that is not allowed to be removed, fail
        REQUIRE(t->getNumBetsOnTable() == 0);
        REQUIRE(p1->getNumBetsOnTable() == 0);
        t->testSetState(0, 5, 5);  // point 0, d1=5, d2=5
        // Put down a pass line bet, coming out
        REQUIRE(p1->makeBet(BetName::PassLine, 100, 0, ep) != nullptr);
        t->testRollDice(5,5); // roll a 10, point is 10 
        // Error to remove, can't remove passline 10 any more.
        CHECK(p1->removeBet(BetName::PassLine, 10, ep) == Gen::ReturnCode::Fail);
        CHECK(p1->getBalance() == bal - 100);
// std::cout << ep.diag << std::endl;
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:decisions")
{
    SUBCASE("processWins")
    {
        Gen::ErrorPass ep;
//      REQUIRE(t->getNumBetsOnTable() == 0);
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        // 
//        DecisionRecord dr { };
//        p1->processWin()
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
longDescription: Always plays Pass and automated Come.
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

