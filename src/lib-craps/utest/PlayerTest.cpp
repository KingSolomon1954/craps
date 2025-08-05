//----------------------------------------------------------------
//
// File: PlayerTest.cpp
//
//----------------------------------------------------------------

#include <memory>
#include <craps/Player.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTypes.h>
#include <craps/EventManager.h>
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

    PlayerFixture()
    {
    }

   ~PlayerFixture()
    {
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

