//----------------------------------------------------------------
//
// File: RollLogTest.cpp
//
//----------------------------------------------------------------

// doctest first: ncurses (via the dialog header) #defines timeout(),
// which clashes with doctest's timeout decorator.
#include <doctest/doctest.h>
#include <cui/core/RollLog.h>
#include <cui/dialogs/DialogDeepRollHistory.h>

using namespace Cui;
using Deep = DialogDeepRollHistory;

//----------------------------------------------------------------

TEST_CASE("RollLog: newest first, flags land on the newest roll")
{
    auto& log = RollLog::instance();
    log.clear();

    log.markSevenOut();                 // Nothing yet: must not crash
    CHECK(log.rolls().empty());

    log.add(3, 4);
    log.add(2, 2);
    log.markPoint();
    REQUIRE(log.rolls().size() == 2);
    CHECK(log.rolls()[0].d1 == 2);
    CHECK(log.rolls()[0].point);
    CHECK_FALSE(log.rolls()[1].point);

    log.markSevenOut();
    log.markPassLineWinner();
    CHECK(log.rolls()[0].sevenOut);
    CHECK(log.rolls()[0].passLineWinner);
    log.clear();
}

//----------------------------------------------------------------

TEST_CASE("RollLog: keeps only the most recent Capacity rolls")
{
    auto& log = RollLog::instance();
    log.clear();

    for (unsigned i = 0; i < RollLog::Capacity + 10; i++)
    {
        log.add(1 + (i % 6), 1);
    }
    CHECK(log.rolls().size() == RollLog::Capacity);
    CHECK(log.rolls().front().d1 == 1 + ((RollLog::Capacity + 9) % 6));
    log.clear();
}

//----------------------------------------------------------------

TEST_CASE("DialogDeepRollHistory: cell text is right justified")
{
    CHECK(Deep::cellText(RollEntry{3, 4}) == " 7(3,4)");
    CHECK(Deep::cellText(RollEntry{3, 4}).size() == 7);
    CHECK(Deep::cellText(RollEntry{6, 5}) == "11(6,5)");
    CHECK(Deep::cellText(RollEntry{1, 1}) == " 2(1,1)");
    CHECK(Deep::cellText(RollEntry{6, 6}) == "12(6,6)");
    CHECK(Deep::cellText(RollEntry{6, 4}) == "10(6,4)");
}

//----------------------------------------------------------------

TEST_CASE("DialogDeepRollHistory: grid reads down each column in turn")
{
    CHECK(Deep::indexAt(0, 0)  == 0);
    CHECK(Deep::indexAt(1, 0)  == 1);
    CHECK(Deep::indexAt(19, 0) == 19);
    CHECK(Deep::indexAt(0, 1)  == 20);
    CHECK(Deep::indexAt(19, 5) == 119);
}

//----------------------------------------------------------------
