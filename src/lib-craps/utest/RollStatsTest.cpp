//----------------------------------------------------------------
//
// File: RollStatsTest.cpp
//
// Tests for RollStats / TableStats dice roll statistics added with the
// bet statistics work (hardway loss cause, point-phase number counts).
//
//----------------------------------------------------------------

#include <craps/Dice.h>
#include <craps/RollStats.h>
#include <craps/TableStats.h>
#include <doctest/doctest.h>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("TableStats:hardway simulated on come out, loss cause split")
{
    TableStats ts;
    Dice dice;

    dice.set(4, 4);  ts.recordDiceRoll(0, dice);   // Hard 8, come out
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);

    dice.set(2, 4);  ts.recordDiceRoll(6, dice);   // Easy 6
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 1);

    dice.set(3, 4);  ts.recordDiceRoll(0, dice);   // Seven on come out
    for (unsigned n : {4U, 6U, 8U, 10U})
        CHECK(ts.rollStats.hardwayCounts[n].lose.count() >= 1);

    const auto six = ts.rollStats.simulatedHardwayLosses(6);
    CHECK(six.easy  == 1);
    CHECK(six.seven == 1);
    const auto four = ts.rollStats.simulatedHardwayLosses(4);
    CHECK(four.easy  == 0);
    CHECK(four.seven == 1);

    const auto sim = ts.rollStats.simulatedCounts(BetName::Hardway, 6);
    CHECK(sim.wins   == 0);
    CHECK(sim.losses == 2);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:place buy lay simulated use point rolls only")
{
    TableStats ts;
    Dice dice;

    dice.set(3, 4);  ts.recordDiceRoll(0, dice);   // Seven on come out
    dice.set(3, 3);  ts.recordDiceRoll(0, dice);   // Point 6 established
    dice.set(3, 3);  ts.recordDiceRoll(6, dice);   // Point rolled
    dice.set(2, 4);  ts.recordDiceRoll(6, dice);   // Another 6
    dice.set(3, 4);  ts.recordDiceRoll(6, dice);   // Seven out
    CHECK(ts.rollStats.numberCounts[7].count()      == 2);
    CHECK(ts.rollStats.pointNumberCounts[7].count() == 1);
    CHECK(ts.rollStats.pointNumberCounts[6].count() == 2);

    const auto place = ts.rollStats.simulatedCounts(BetName::Place, 6);
    CHECK(place.wins   == 2);
    CHECK(place.losses == 1);
    const auto buy = ts.rollStats.simulatedCounts(BetName::Buy, 6);
    CHECK(buy.wins   == 2);
    CHECK(buy.losses == 1);

    // Lay is the opposite of Buy, it wins on the seven.
    const auto lay = ts.rollStats.simulatedCounts(BetName::Lay, 6);
    CHECK(lay.wins   == 1);
    CHECK(lay.losses == 2);
}

//----------------------------------------------------------------

TEST_CASE("RollStats:new counters yaml and merge")
{
    RollStats a;
    a.pointNumberCounts[6].bump();
    a.hardwayEasy[8].bump();
    a.hardwaySeven[8].bump();
    a.numberCounts[12].bump();
    RollStats b;
    b.fromYAML(a.toYAML());
    CHECK(b.pointNumberCounts[6].count() == 1);
    CHECK(b.hardwayEasy[8].count()       == 1);
    CHECK(b.hardwaySeven[8].count()      == 1);

    a.merge(a);
    CHECK(a.numberCounts[12].count()      == 2);   // 12 used to be skipped
    CHECK(a.pointNumberCounts[6].count()  == 2);
    a.reset();
    CHECK(a.numberCounts[12].count()      == 0);
    CHECK(a.pointNumberCounts[6].count()  == 0);
}

//----------------------------------------------------------------

TEST_CASE("RollStats:field lose yaml is not the field wins")
{
    RollStats a;
    a.fieldBetWins.bump();
    a.fieldBetWins.bump();
    a.fieldBetLose.bump();
    const auto node = a.toYAML();
    CHECK(node["fieldBetWins"]["count"].as<unsigned>() == 2);
    CHECK(node["fieldBetLose"]["count"].as<unsigned>() == 1);
}
