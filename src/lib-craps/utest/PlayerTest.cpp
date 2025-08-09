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

    PlayerFixture()
    {
        TableConfig tableConfig;
        tableConfig.maxSessions = 50;
        tableConfig.maxRecentRolls = 25;
        tableConfig.tablePath = "tmp/dontcare.yaml";

        t = new CrapsTable("Table-1", tableConfig, em);
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

TEST_CASE_FIXTURE(PlayerFixture, "Player:joinTable")
{
    SUBCASE("joinTable")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1 != nullptr);

        // nullptr
        CHECK(p1->joinTable(nullptr, ep) == Gen::ReturnCode::Fail);

        // Join table success
        CHECK(p1->joinTable(t, ep) == Gen::ReturnCode::Success);

        // Join same table twice
        CHECK(p1->joinTable(t, ep) == Gen::ReturnCode::Fail);
    }
    
    SUBCASE("leaveTable")
    {
        // TODO
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

        // Remove bet, any bet allowed can be removed before its first roll
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

TEST_CASE_FIXTURE(PlayerFixture, "Player:setOddsAmount")
{
    SUBCASE("goodBet")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->isComeOutRoll());
        Gen::Money bal = p1->getBalance();
        
        auto pBet = p1->makeBet(BetName::PassLine, 100, 0, ep);
        REQUIRE(pBet != nullptr);
        REQUIRE(pBet->oddsAmount() == 0);
        t->testRollDice(5,5); // roll a 10, point is now 10
        CHECK(pBet->oddsAmount() == 0);
        CHECK(p1->setOddsAmount(pBet, 200, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getBalance() == bal - 300);
        CHECK(p1->getNumBetsOnTable() == 1);
    }
    
    SUBCASE("badBets")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id, config, em));

        // nullptr
        CHECK(p1->setOddsAmount(nullptr, 100, ep) == Gen::ReturnCode::Fail);

        // Set odds on a bet that doesn't belong to player
        auto pBet2 = std::make_shared<CrapsBet>(p2.get(), BetName::Place, 100, 6);
        REQUIRE(pBet2 != nullptr);
        CHECK(p1->setOddsAmount(pBet2, 100, ep) == Gen::ReturnCode::Fail);

        // Set odds on a bet that belongs to player, but not joined, programmer error
        auto pBet1 = std::make_shared<CrapsBet>(p1.get(), BetName::Place, 100, 6);
        REQUIRE(pBet1 != nullptr);
        CHECK(p1->setOddsAmount(pBet1, 100, ep) == Gen::ReturnCode::Fail);

        // Bad bet type for setOdds
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        REQUIRE(p1->getNumBetsOnTable() == 0);
        auto pBet3 = p1->makeBet(BetName::Place, 100, 6, ep);
        REQUIRE(pBet3 != nullptr);
        CHECK(p1->setOddsAmount(pBet3, 100, ep) == Gen::ReturnCode::Fail);
        
        // Insufficient funds
        REQUIRE(t->isComeOutRoll());
        auto pBet4 = p1->makeBet(BetName::PassLine, 100, 0, ep);
        REQUIRE(pBet4 != nullptr);
        t->testRollDice(5,5); // roll a 10, point is now 10 
        Gen::Money bal = p1->getBalance();
        CHECK(p1->setOddsAmount(pBet4, bal + 100, ep) == Gen::ReturnCode::Fail);

        // Table rejects odds bet due to table limit
        Gen::Money tooMuch = (t->getMaxOdds() +1) * 100;
        CHECK(p1->setOddsAmount(pBet4, tooMuch, ep) == Gen::ReturnCode::Fail);
        CHECK(pBet4->oddsAmount() == 0);
    }
        
    SUBCASE("changeAmount")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        REQUIRE(t->isComeOutRoll());
        Gen::Money bal = p1->getBalance();

        // Establish a good passline bet with $200 odds.
        auto pBet = p1->makeBet(BetName::PassLine, 100, 0, ep);
        REQUIRE(pBet != nullptr);
        REQUIRE(pBet->oddsAmount() == 0);
        t->testRollDice(5,5); // roll a 10, point is now 10
        REQUIRE(pBet->oddsAmount() == 0);
        REQUIRE(p1->setOddsAmount(pBet, 200, ep) == Gen::ReturnCode::Success);
        REQUIRE(p1->getBalance() == bal - 300);
        REQUIRE(p1->getNumBetsOnTable() == 1);

        // Change odds amount to 0
        CHECK(p1->setOddsAmount(pBet, 0, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getBalance() == bal - 100);        

        // Change odds amount to 400
        CHECK(p1->setOddsAmount(pBet, 400, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getBalance() == bal - 500);        

        // Change odds amount to 100
        CHECK(p1->setOddsAmount(pBet, 100, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getBalance() == bal - 200);        

        // Change odds amount to 1
        CHECK(p1->setOddsAmount(pBet, 1, ep) == Gen::ReturnCode::Success);
        CHECK(p1->getBalance() == bal - 101);        
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:decisions")
{
    SUBCASE("processWin")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        Gen::Money bal = p1->getBalance();
        auto b1 = p1->makeBet(BetName::Place, 120, 6, ep);
        REQUIRE(p1->getNumBetsOnTable() == 1);
        DecisionRecord r1{b1.get(), true, false, 140, 0, 0, 0};
        p1->processWin(r1);
        CHECK(p1->getBalance() == bal + 140);
        CHECK(p1->getNumBetsOnTable() == 0);
    }

    SUBCASE("processLose")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        Gen::Money bal = p1->getBalance();
        auto b1 = p1->makeBet(BetName::Place, 120, 6, ep);
        REQUIRE(p1->getNumBetsOnTable() == 1);
        DecisionRecord r1{b1.get(), true, false, 0, 140, 0, 0};
        p1->processLose(r1);
        CHECK(p1->getBalance() == bal - 120);
        CHECK(p1->getNumBetsOnTable() == 0);
    }

    SUBCASE("processKeep")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        Gen::Money bal = p1->getBalance();
        auto b1 = p1->makeBet(BetName::PassLine, 100, 0, ep);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id, config, em));
        REQUIRE(p1->joinTable(t, ep) == Gen::ReturnCode::Success);
        Gen::Money bal = p1->getBalance();
        auto b1 = p1->makeBet(BetName::Buy, 100, 4, ep);
        REQUIRE(p1->getNumBetsOnTable() == 1);
        DecisionRecord r1{b1.get(), true, false, 195, 0, 0, 5};
        p1->processWin(r1);
        CHECK(p1->getBalance() == bal + 195);
        CHECK(p1->getNumBetsOnTable() == 0);

        // Come bets return odds money, make point then roll 7
        bal = p1->getBalance();
        t->testSetState(4, 5, 5);  // point 0, d1=5, d2=5
        auto b2 = p1->makeBet(BetName::Come, 100, 0, ep);
        REQUIRE(p1->getNumBetsOnTable() == 1);
        b2->testSetPivot(10);  // avoid dice roll, setup bet to have a point
        REQUIRE(p1->setOddsAmount(b2, 200, ep) == Gen::ReturnCode::Success);
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

