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
#include <string>
#include <yaml-cpp/yaml.h>

using namespace Craps;

std::string getPlayerYamlStringUtest();

//----------------------------------------------------------------

struct PlayerFixture
{
    PlayerId p1Id { "uuid1" };
    PlayerId p2Id { "uuid2" };
    CrapsTable* t;

    PlayerFixture()
    {
        TableConfig tableConfig;
        tableConfig.maxSessions = 50;
        tableConfig.maxRecentRolls = 25;

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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id));
        CHECK(p1->getName() == "uuid1");
        CHECK(p2->getName() == "uuid2");
    }

    SUBCASE("via fromString()")
    {
        std::string yaml = getPlayerYamlStringUtest();
        std::unique_ptr<Player> p1(Player::fromString(yaml, p1Id));
        CHECK(p1->getPlayerId() == "uuid1");
        CHECK(p1->getBalance() == 30000);
        // TODO check more fields to matching YAML
    }
    
    SUBCASE("fromString:mismatched playerId")
    {
        // YAML says uuid1; asking for uuid2 must be refused.
        std::string yaml = getPlayerYamlStringUtest();
        CHECK_THROWS_AS(Player::fromString(yaml, p2Id), std::runtime_error);
    }

    SUBCASE("fromString:toYAML keeps identity")
    {
        std::string yaml = getPlayerYamlStringUtest();
        std::unique_ptr<Player> p1(Player::fromString(yaml, p1Id));
        const YAML::Node out = p1->toYAML();
        CHECK(out["playerId"].as<std::string>() == "uuid1");
        CHECK(out["Bank"].IsDefined());
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:joinTable")
{
    SUBCASE("joinTable")
    {
        Gen::ErrorPass ep;
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        REQUIRE(p1 != nullptr);

        // Join table success
        auto result = p1->joinTable(*t); CHECK(result);

        // Join same table twice
        result = p1->joinTable(*t); CHECK(!result);
    }
    
    SUBCASE("leaveTable")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        REQUIRE(p1 != nullptr);

        // Leave table without ever joining
        REQUIRE((*t).getNumPlayers() == 0);
        auto result = p1->leaveTable(); CHECK(!result);

        // Join, then immediately leave, no intervening activity
        result = p1->joinTable(*t); CHECK(result);
        result = p1->leaveTable();  CHECK(result);
        
        // Leave table, with outstanding bets
        result = p1->joinTable(*t); REQUIRE(result);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->testMakeBet(BetName::Place,    100, 6); REQUIRE(result2);
        result2 =      p1->testMakeBet(BetName::Place,    100, 8); REQUIRE(result2);
        result2 =      p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
        result2 =      p1->testMakeBet(BetName::Hardway,  100, 4); REQUIRE(result2);
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
        t->testSetState(4, 5, 5);  // establish a point so odds can be set  // point 0, d1=5, d2=5
        
        // Put down a pass line bet, coming out
        result2 = p1->testMakeBet(BetName::PassLine, 100, 0);
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

TEST_CASE_FIXTURE(PlayerFixture, "Player:after leaving the table")
{
    std::unique_ptr<Player> p1(Player::createPlayer(p1Id));

    auto joined = p1->joinTable(*t); REQUIRE(joined);
    auto bet = p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(bet);

    auto left = p1->leaveTable(); REQUIRE(left);

    // Not at a table any more, so no betting, until joining again
    bet = p1->testMakeBet(BetName::PassLine, 100, 0); CHECK(!bet);

    joined = p1->joinTable(*t); REQUIRE(joined);
    bet = p1->testMakeBet(BetName::PassLine, 100, 0); CHECK(bet);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:endSession then join starts a new session")
{
    std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
    auto joined = p1->joinTable(*t); REQUIRE(joined);

    // The session being played is counted when the player is saved.
    auto numSessions = [&]()
    {
        const YAML::Node out = p1->toYAML();
        return out["SessionStats"]["numSessionsAlltime"].as<int>();
    };
    const int        n0   = numSessions();
    const Gen::Money bal  = p1->getBalance();
    const size_t     hist = p1->getSessionHistory().size();

    p1->endSession();
    CHECK_FALSE(t->havePlayer(p1.get()));              // Left the table
    CHECK(p1->getSessionHistory().size() == hist + 1); // Session recorded
    CHECK(numSessions() == n0);                        // Nothing new yet
    CHECK(p1->getBalance() == bal);

    joined = p1->joinTable(*t); REQUIRE(joined);
    CHECK(numSessions() == n0 + 1);                    // Second session
    CHECK(p1->getBalance() == bal);                    // Balance carries over
    CHECK(p1->getSessionNet() == 0);                   // Fresh session

    p1->endSession();
    CHECK(p1->getSessionHistory().size() == hist + 2);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "CrapsTable:shooter leaves the table")
{
    std::unique_ptr<Player> a(Player::createPlayer("a"));
    std::unique_ptr<Player> b(Player::createPlayer("b"));
    std::unique_ptr<Player> c(Player::createPlayer("c"));
    REQUIRE(a->joinTable(*t));
    REQUIRE(b->joinTable(*t));
    REQUIRE(c->joinTable(*t));

    SUBCASE("someone else leaves, shooter is unchanged")
    {
        t->setShooter(*a);
        REQUIRE(b->leaveTable());
        CHECK(t->getCurrentShooter() == a.get());
    }

    SUBCASE("shooter leaves, dice go to the next player, then wrap around")
    {
        t->setShooter(*b);
        REQUIRE(b->leaveTable());
        CHECK(t->getCurrentShooter() == c.get());
        REQUIRE(c->leaveTable());
        CHECK(t->getCurrentShooter() == a.get());
        REQUIRE(a->leaveTable());
        CHECK(t->getCurrentShooter() == nullptr);       // nobody left
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "CrapsTable:replacePlayer")
{
    std::unique_ptr<Player> a(Player::createPlayer("a"));
    std::unique_ptr<Player> b(Player::createPlayer("b"));
    std::unique_ptr<Player> c(Player::createPlayer("c"));
    std::unique_ptr<Player> d(Player::createPlayer("d"));   // not at table
    std::unique_ptr<Player> e(Player::createPlayer("e"));   // not at table
    REQUIRE(a->joinTable(*t));
    REQUIRE(b->joinTable(*t));
    REQUIRE(c->joinTable(*t));
    t->setShooter(*a);

    const std::vector<Player*> before { a.get(), b.get(), c.get() };

    SUBCASE("new player takes the seat")
    {
        auto r = t->replacePlayer(*b, *d); REQUIRE(r);
        CHECK_FALSE(t->havePlayer(b.get()));
        CHECK(t->havePlayer(d.get()));
        CHECK(t->getNumPlayers() == 3);
        const std::vector<Player*> expect { a.get(), d.get(), c.get() };
        CHECK(t->getPlayers() == expect);               // Same seat
        CHECK(t->getCurrentShooter() == a.get());       // Unaffected
    }

    SUBCASE("shooter leaves, dice go to the new player")
    {
        t->setShooter(*b);
        auto r = t->replacePlayer(*b, *d); REQUIRE(r);
        CHECK(t->getCurrentShooter() == d.get());
    }

    SUBCASE("leaving player's bets come off the table and go back to him")
    {
        const Gen::Money bal = b->getBalance();
        auto bet = b->testMakeBet(BetName::Place, 100, 6); REQUIRE(bet);
        REQUIRE(b->getNumBetsOnTable() == 1);
        auto r = t->replacePlayer(*b, *d); REQUIRE(r);
        CHECK(b->getNumBetsOnTable() == 0);
        CHECK(b->getBalance() == bal);
    }

    SUBCASE("leaving player's session ends")
    {
        const size_t hist = b->getSessionHistory().size();
        auto r = t->replacePlayer(*b, *d); REQUIRE(r);
        CHECK(b->getSessionHistory().size() == hist + 1);
    }

    SUBCASE("full table")
    {
        std::vector<std::unique_ptr<Player>> more;
        while (t->getNumPlayers() < 6)
        {
            more.emplace_back(Player::createPlayer("x"));
            REQUIRE(more.back()->joinTable(*t));
        }
        // Table is full, so a plain join fails but a swap works.
        auto j = d->joinTable(*t); CHECK(!j);
        auto r = t->replacePlayer(*b, *d); CHECK(r);
        CHECK(t->getNumPlayers() == 6);
        CHECK(t->havePlayer(d.get()));
    }

    SUBCASE("player to leave is not at the table, nothing changes")
    {
        auto r = t->replacePlayer(*d, *e); CHECK(!r);
        CHECK_FALSE(t->havePlayer(e.get()));
        CHECK(t->getPlayers() == before);
    }

    SUBCASE("player to join is already at the table, nothing changes")
    {
        t->setShooter(*b);
        auto r = t->replacePlayer(*b, *c); CHECK(!r);
        CHECK(t->getPlayers() == before);
        CHECK(t->getCurrentShooter() == b.get());
    }

    SUBCASE("replacing a player with himself, nothing changes")
    {
        auto r = t->replacePlayer(*b, *b); CHECK(!r);
        CHECK(t->getPlayers() == before);
    }

    SUBCASE("a replaced player can come back later")
    {
        REQUIRE(t->replacePlayer(*b, *d));
        auto r = t->replacePlayer(*d, *b); REQUIRE(r);
        CHECK(t->havePlayer(b.get()));
        CHECK_FALSE(t->havePlayer(d.get()));
        CHECK(t->getPlayers() == before);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:findBet")
{
    std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
    CHECK(p1->findBet(BetName::PassLine, 0) == nullptr);     // no bets at all
    auto joined = p1->joinTable(*t); REQUIRE(joined);

    auto pass   = p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(pass);
    auto place6 = p1->testMakeBet(BetName::Place,    100, 6); REQUIRE(place6);
    auto field  = p1->testMakeBet(BetName::Field,    100, 0); REQUIRE(field);

    CHECK(p1->findBet(BetName::PassLine, 0) == pass.value());
    CHECK(p1->findBet(BetName::Place,    6) == place6.value());
    CHECK(p1->findBet(BetName::Place,    8) == nullptr);     // another number
    CHECK(p1->findBet(BetName::DontPass, 0) == nullptr);
    CHECK(p1->findBet(BetName::Field,    5) == field.value()); // no pivot

    // Once there is a point, a PassLine bet's pivot is the point. Asking
    // with the point or without is the same bet.
    t->testRollDice(5, 5);                                   // point is 10
    CHECK(p1->findBet(BetName::PassLine, 0)  == pass.value());
    CHECK(p1->findBet(BetName::PassLine, 10) == pass.value());
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:makeOrChangeBet")
{
    std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
    auto joined = p1->joinTable(*t); REQUIRE(joined);
    const Gen::Money bal = p1->getBalance();

    SUBCASE("first time makes the bet")
    {
        auto r = p1->makeOrChangeBet(BetName::PassLine, 100, 0); REQUIRE(r);
        CHECK_FALSE(r.value().changed);
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 100);
    }

    SUBCASE("same bet again changes the amount, it is not a second bet")
    {
        auto first = p1->makeOrChangeBet(BetName::PassLine, 100, 0); REQUIRE(first);
        auto up    = p1->makeOrChangeBet(BetName::PassLine, 300, 0); REQUIRE(up);
        CHECK(up.value().changed);
        CHECK(up.value().previousAmount == 100);
        CHECK(up.value().bet == first.value().bet);          // same bet
        CHECK(first.value().bet->contractAmount() == 300);
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 300);                // not 400

        // Down again; fine before the point. Repeating is the same result.
        auto down = p1->makeOrChangeBet(BetName::PassLine, 200, 0); REQUIRE(down);
        CHECK(down.value().previousAmount == 300);
        auto same = p1->makeOrChangeBet(BetName::PassLine, 200, 0); REQUIRE(same);
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 200);
    }

    SUBCASE("same amount is a successful no-op")
    {
        auto first = p1->makeOrChangeBet(BetName::PassLine, 100, 0); REQUIRE(first);
        auto again = p1->makeOrChangeBet(BetName::PassLine, 100, 0); REQUIRE(again);
        CHECK(again.value().changed);
        CHECK(again.value().previousAmount == 100);
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 100);
    }

    SUBCASE("different spots are different bets")
    {
        auto a = p1->makeOrChangeBet(BetName::PassLine, 100, 0); REQUIRE(a);
        auto b = p1->makeOrChangeBet(BetName::Place,    100, 6); REQUIRE(b);
        auto c = p1->makeOrChangeBet(BetName::Place,    100, 8); REQUIRE(c);
        CHECK_FALSE(b.value().changed);
        CHECK_FALSE(c.value().changed);
        CHECK(p1->getNumBetsOnTable() == 3);
    }

    SUBCASE("a change that cannot be made leaves the bet alone")
    {
        auto first = p1->makeOrChangeBet(BetName::PassLine, 100, 0); REQUIRE(first);

        auto tooMuch = p1->makeOrChangeBet(BetName::PassLine, bal + 1000, 0);
        CHECK(!tooMuch);

        // Zero is not a way to take a bet down
        auto zero = p1->makeOrChangeBet(BetName::PassLine, 0, 0);
        CHECK(!zero);

        CHECK(first.value().bet->contractAmount() == 100);
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(p1->getBalance() == bal - 100);
    }

    SUBCASE("after the point, PassLine can be increased but not reduced")
    {
        auto first = p1->makeOrChangeBet(BetName::PassLine, 200, 0); REQUIRE(first);
        t->testRollDice(5, 5);                               // point is 10

        auto up = p1->makeOrChangeBet(BetName::PassLine, 300, 0); REQUIRE(up);
        CHECK(up.value().changed);
        CHECK(first.value().bet->contractAmount() == 300);

        auto down = p1->makeOrChangeBet(BetName::PassLine, 100, 0);
        CHECK(!down);
        CHECK(first.value().bet->contractAmount() == 300);
        CHECK(p1->getNumBetsOnTable() == 1);
    }

    SUBCASE("after the point, DontPass can be reduced but not increased")
    {
        auto first = p1->makeOrChangeBet(BetName::DontPass, 200, 0); REQUIRE(first);
        t->testRollDice(5, 5);                               // point is 10

        auto up = p1->makeOrChangeBet(BetName::DontPass, 300, 0);
        CHECK(!up);
        CHECK(first.value().bet->contractAmount() == 200);

        auto down = p1->makeOrChangeBet(BetName::DontPass, 100, 0); REQUIRE(down);
        CHECK(down.value().changed);
        CHECK(first.value().bet->contractAmount() == 100);
        CHECK(p1->getNumBetsOnTable() == 1);
    }
}

//----------------------------------------------------------------

//----------------------------------------------------------------

TEST_CASE_FIXTURE(PlayerFixture, "Player:makeBet")
{
    SUBCASE("badBets")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));

        // Must join table first
        auto result = p1->testMakeBet(BetName::Place, 100, 6); CHECK(!result);

        // Join table
        auto result2 = p1->joinTable(*t); REQUIRE(result2);

        // Insufficient funds
        Gen::Money bal = p1->getBalance();
        result = p1->testMakeBet(BetName::Place, bal + 100, 6); CHECK(!result);

        // Bad bet, missing pivot for a place bet
        result = p1->testMakeBet(BetName::Place, 100, 0); CHECK(!result);

        // Bad bet, zero dollar bet
        result = p1->testMakeBet(BetName::Place, 0, 6); CHECK(!result);
    }
        
    SUBCASE("goodBets")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result2 = p1->joinTable(*t); REQUIRE(result2);

        Gen::Money bal = p1->getBalance();
        auto result = p1->testMakeBet(BetName::Place,    100, 6); CHECK(result);
             result = p1->testMakeBet(BetName::Place,    100, 8); CHECK(result);
             result = p1->testMakeBet(BetName::PassLine, 100, 0); CHECK(result);
             result = p1->testMakeBet(BetName::Hardway,  100, 4); CHECK(result);
        CHECK(p1->getNumBetsOnTable() == 4);
        CHECK(p1->getAmountOnTable() == 400);
        CHECK(p1->getBalance() == bal - 400);
    }

    SUBCASE("removeBet")
    {
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);

        Gen::Money bal = p1->getBalance();
        auto result2 = p1->testMakeBet(BetName::Place, 100, 6); REQUIRE(result2);
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
        result2 = p1->testMakeBet(BetName::PassLine, 100, 4); REQUIRE(result2);
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

        result2 = p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        REQUIRE(t->isComeOutRoll());
        Gen::Money bal = p1->getBalance();

        auto result2 = p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id));

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
        auto result2 = p1->testMakeBet(BetName::Place, 100, 6); REQUIRE(result2);
        auto pBet3 = result2.value();
        REQUIRE(pBet3 != nullptr);
        result1 = p1->setOddsAmount(pBet3, 100); CHECK(!result1);
        
        // Insufficient funds
        REQUIRE(t->isComeOutRoll());
        result2 = p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        REQUIRE(t->isComeOutRoll());
        Gen::Money bal = p1->getBalance();

        // Establish a good passline bet with $200 odds.
        auto result2 = p1->testMakeBet(BetName::PassLine, 100, 0);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->testMakeBet(BetName::Place, 120, 6); REQUIRE(result2);
        auto b1 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        p1->bettingClosed();
        DecisionRecord r1{b1.get(), true, false, 140, 0, 0, 0};
        r1.removeBet = false;  // A winning Place wager remains active.
        p1->processWin(r1);
        CHECK(p1->getBalance() == bal + 20);  // wager remains on table
        CHECK(p1->getNumBetsOnTable() == 1);
        CHECK(t->getNumBetsOnTable() == 1);

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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->testMakeBet(BetName::Place, 120, 6); REQUIRE(result2);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->testMakeBet(BetName::PassLine, 100, 0); REQUIRE(result2);
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
        std::unique_ptr<Player> p1(Player::createPlayer(p1Id));
        auto result1 = p1->joinTable(*t); REQUIRE(result1);
        Gen::Money bal = p1->getBalance();
        auto result2 = p1->testMakeBet(BetName::Buy, 100, 4); REQUIRE(result2);
        auto b1 = result2.value();
        REQUIRE(p1->getNumBetsOnTable() == 1);
        DecisionRecord r1{b1.get(), true, false, 195, 0, 0, 5};
        r1.removeBet = false;  // Buy wagers also remain active after a win.
        p1->processWin(r1);
        CHECK(p1->getBalance() == bal + 95);  // $100 wager remains on table
        CHECK(p1->getNumBetsOnTable() == 1);

        // Come bet hits its number on the come-out roll with odds off.
        // Use a separate player because the Buy wager above remains active.
        std::unique_ptr<Player> p2(Player::createPlayer(p2Id));
        result1 = p2->joinTable(*t); REQUIRE(result1);
        bal = p2->getBalance();
        t->testSetState(6, 5, 5);  // establish a point so odds can be set
        result2 = p2->testMakeBet(BetName::Come, 100, 0); REQUIRE(result2);
        auto b2 = result2.value();
        REQUIRE(p2->getNumBetsOnTable() == 1);
        b2->testSetPivot(10);  // simulate an existing Come 10 bet
        result1 = p2->setOddsAmount(b2, 200); REQUIRE(result1);
        DecisionRecord r2{b2.get(), true, false, 100, 0, 200, 0};
        p2->processWin(r2);
        // The odds stake is represented by returnToPlayer, so settlement
        // must not also return it as part of the ordinary stake refund.
        CHECK(p2->getBalance() == bal + 100);
        CHECK(p2->getNumBetsOnTable() == 0);
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
  overall:
    bets:
      count: 0
      wins: 0
      losses: 0
      kept: 0
    amounts:
      bet: 0
      won: 0
      lost: 0
      kept: 0
    maximums:
      bet: 0
      win: 0
      loss: 0
      kept: 0
  byBet:
    AnyCraps:
      wins:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
        oddsBet: 0
        oddsAmount: 0
      losses:
        count: 0
        totDistance: 0
        amount: 0
        amountBet: 0
        oddsBet: 0
        oddsAmount: 0
  rollActivity:
    betsEvaluated:
      allRolls: 0
      busiestRoll: 0
    betsWon:
      allRolls: 0
      busiestRoll: 0
    betsLost:
      allRolls: 0
      busiestRoll: 0
    betsKept:
      allRolls: 0
      busiestRoll: 0
    amounts:
      evaluated:
        allRolls: 0
        busiestRoll: 0
      won:
        allRolls: 0
        busiestRoll: 0
      lost:
        allRolls: 0
        busiestRoll: 0
      kept:
        allRolls: 0
        busiestRoll: 0
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

