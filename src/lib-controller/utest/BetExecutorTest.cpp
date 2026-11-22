//----------------------------------------------------------------
//
// File: BetExecutorTest.cpp
//
//----------------------------------------------------------------

#include <controller/BetExecutor.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>
#include <craps/TableConfig.h>
#include <doctest/doctest.h>
#include <memory>

using namespace Ctrl;
using Status = BetExecutor::Status;

namespace {

BetSpec
spec(BetName name, unsigned pivot, Gen::Money amount, bool odds = false)
{
    BetSpec s;
    s.betName = name;
    s.pivot   = pivot;
    s.amount  = amount;
    s.isOdds  = odds;
    return s;
}

struct ExecutorFixture
{
    ExecutorFixture()
    {
        Craps::TableConfig tableConfig;
        tableConfig.maxSessions    = 50;
        tableConfig.maxRecentRolls = 25;
        table  = std::make_unique<Craps::CrapsTable>("Table-1", tableConfig);
        player.reset(Craps::Player::createPlayer("exec-player"));
        REQUIRE(player->joinTable(*table));
        balance = player->getBalance();
        undo.clear();
    }
   ~ExecutorFixture() { undo.clear(); }

    std::unique_ptr<Craps::CrapsTable> table;
    std::unique_ptr<Craps::Player>     player;
    UndoManager&                       undo = UndoManager::instance();
    Gen::Money                         balance = 0;

    BetExecutor::Result apply(const std::vector<BetSpec>& specs,
                              LastFill* lf = nullptr)
    {
        return BetExecutor::apply(*player, specs, undo, lf);
    }
};

} // namespace

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:places every bet, one undo step")
{
    const auto r = apply({spec(BetName::PassLine, 0, 100),
                          spec(BetName::Place, 6, 100),
                          spec(BetName::Field, 0, 100)});

    CHECK(r.count(Status::Placed) == 3);
    CHECK(r.count(Status::Skipped) == 0);
    CHECK(r.headline() == "3 placed");
    CHECK(player->getNumBetsOnTable() == 3);
    CHECK(player->getBalance() == balance - 300);
    CHECK(undo.depth(player->getPlayerId()) == 1);   // One step for all three

    std::string what;
    Gen::ErrorPass ep;
    REQUIRE(undo.undo(player->getPlayerId(), what, ep) == Gen::ReturnCode::Success);
    CHECK(player->getNumBetsOnTable() == 0);
    CHECK(player->getBalance() == balance);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:sets the amount of a bet already there")
{
    REQUIRE(player->makeOrChangeBet(BetName::Field, 100, 0));

    SUBCASE("a different amount changes it")
    {
        const auto r = apply({spec(BetName::Field, 0, 200)});
        CHECK(r.count(Status::Changed) == 1);
        CHECK(player->getNumBetsOnTable() == 1);          // Not a second bet
        CHECK(player->getBalance() == balance - 200);
        CHECK(undo.depth(player->getPlayerId()) == 1);

        std::string what;
        Gen::ErrorPass ep;
        REQUIRE(undo.undo(player->getPlayerId(), what, ep) == Gen::ReturnCode::Success);
        CHECK(player->getBalance() == balance - 100);     // Back to $100
    }

    SUBCASE("the same amount changes nothing and records nothing")
    {
        const auto r = apply({spec(BetName::Field, 0, 100)});
        CHECK(r.count(Status::AlreadyInPlace) == 1);
        CHECK_FALSE(r.anyApplied());
        CHECK(r.headline() == "1 already in place");
        CHECK(undo.depth(player->getPlayerId()) == 0);
    }
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:best effort, a failure does not stop the rest")
{
    const Gen::Money tooMuch = balance + 1000;
    const auto r = apply({spec(BetName::Field, 0, 100),
                          spec(BetName::Place, 6, tooMuch),
                          spec(BetName::AnyCraps, 0, 100)});

    REQUIRE(r.outcomes.size() == 3);
    CHECK(r.outcomes[0].status == Status::Placed);
    CHECK(r.outcomes[1].status == Status::Skipped);
    CHECK_FALSE(r.outcomes[1].reason.empty());
    CHECK(r.outcomes[2].status == Status::Placed);
    CHECK(r.headline() == "2 placed, 1 skipped");
    CHECK(r.skippedDetail().find("Place 6") != std::string::npos);
    CHECK(player->getNumBetsOnTable() == 2);
    CHECK(undo.depth(player->getPlayerId()) == 1);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:nothing applied records no undo step")
{
    const auto r = apply({spec(BetName::Field, 0, balance + 1000),
                          spec(BetName::Invalid, 0, 5),
                          spec(BetName::Field, 0, 0)});

    CHECK(r.count(Status::Skipped) == 3);
    CHECK_FALSE(r.anyApplied());
    CHECK(undo.depth(player->getPlayerId()) == 0);
    CHECK(player->getBalance() == balance);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:contract bets are applied before odds")
{
    // Odds listed first. The contract bet still goes first.
    const auto r = apply({spec(BetName::PassLine, 0, 100, true),
                          spec(BetName::PassLine, 0, 100)});

    REQUIRE(r.outcomes.size() == 2);
    CHECK_FALSE(r.outcomes[0].spec.isOdds);
    CHECK(r.outcomes[0].status == Status::Placed);
    CHECK(r.outcomes[1].spec.isOdds);
    // No point yet, so the table is expected to refuse the odds. Either
    // way the contract bet is on the table and one step covers what was done.
    CHECK(player->findBet(BetName::PassLine, 0) != nullptr);
    CHECK(undo.depth(player->getPlayerId()) == 1);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:odds with no bet to sit on are skipped")
{
    const auto r = apply({spec(BetName::DontPass, 0, 100, true)});
    REQUIRE(r.outcomes.size() == 1);
    CHECK(r.outcomes[0].status == Status::Skipped);
    CHECK(r.outcomes[0].reason.find("DontPass") != std::string::npos);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:remembers last fills like a hand-placed bet")
{
    LastFill lf;
    apply({spec(BetName::Field, 0, 100), spec(BetName::Place, 6, 100)}, &lf);

    LastFill::LastFillEntry e;
    e.betName = BetName::Place;
    e.pivot   = 6;
    REQUIRE(lf.getLastFill(e) == Gen::ReturnCode::Success);
    CHECK(e.amount == 100u);
    CHECK(lf.getLastFills().size() == 2);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(ExecutorFixture, "BetExecutor:a second application is its own undo step")
{
    apply({spec(BetName::Field, 0, 100)});
    apply({spec(BetName::AnyCraps, 0, 100)});
    CHECK(undo.depth(player->getPlayerId()) == 2);
}

//----------------------------------------------------------------
