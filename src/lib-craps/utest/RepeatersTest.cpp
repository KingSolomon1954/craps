//----------------------------------------------------------------
//
// File: RepeatersTest.cpp
//
// Tests for the Repeaters class and the table level point and place
// repeaters recorded by TableStats.
//
//----------------------------------------------------------------

#include <craps/Dice.h>
#include <craps/PlayerStats.h>
#include <craps/Repeaters.h>
#include <craps/RollStats.h>
#include <craps/TableStats.h>
#include <doctest/doctest.h>
#include <stdexcept>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("Repeaters:hits are bucketed when the run ends")
{
    Repeaters r(NonSevenNums);

    r.hit(4); r.hit(4); r.hit(4);
    r.hit(6);
    r.hit(7);                       // Not tracked, ignored
    CHECK(r.running(4) == 3);
    CHECK(r.running(6) == 1);
    CHECK(r.tracks(7)  == false);

    r.endRun();
    CHECK(r.times(4, 3) == 1);
    CHECK(r.times(6, 1) == 1);
    CHECK(r.times(5, 0) == 0);      // Zero hits are not recorded
    CHECK(r.running(4)  == 0);

    r.hit(4);
    r.endRun();
    CHECK(r.times(4, 1) == 1);
    CHECK(r.times(4, 3) == 1);
}

//----------------------------------------------------------------

TEST_CASE("Repeaters:buckets stay in ascending order of hits")
{
    Repeaters r(PointNums);
    for (unsigned hits : {3U, 0U, 1U})
    {
        for (unsigned i = 0; i < hits; ++i)
            r.hit(4);
        r.endRun(true);
    }
    unsigned last = 0;
    bool first = true;
    for (const auto& [hits, times] : r.buckets(4))
    {
        CHECK(times == 1);
        if (!first)
            CHECK(hits > last);
        last  = hits;
        first = false;
    }
    CHECK(r.buckets(4).size() == 3);
}

//----------------------------------------------------------------

TEST_CASE("Repeaters:zero hit runs recorded on request")
{
    Repeaters r(PointNums);
    r.hit(6);
    r.endRun(true);
    CHECK(r.times(6, 1) == 1);
    CHECK(r.times(4, 0) == 1);
    CHECK(r.times(5, 0) == 1);
    CHECK(r.times(6, 0) == 0);
}

//----------------------------------------------------------------

TEST_CASE("Repeaters:merge and reset")
{
    Repeaters a(PointNums);
    Repeaters b(PointNums);
    a.hit(4); a.endRun();
    b.hit(4); b.hit(4); b.endRun();
    b.hit(4); b.endRun();

    a.merge(b);
    CHECK(a.times(4, 1) == 2);
    CHECK(a.times(4, 2) == 1);

    a.reset();
    CHECK(a.buckets(4).empty());
    CHECK(a.tracks(4));
}

//----------------------------------------------------------------

TEST_CASE("Repeaters:yaml round trip drops the running tally")
{
    Repeaters a(NonSevenNums);
    a.hit(4); a.hit(4); a.endRun();
    a.hit(12); a.endRun();
    a.hit(9);                       // Run not finished

    const YAML::Node node = a.toYAML();
    CHECK(node["4"]["2"].as<unsigned>()  == 1);
    CHECK(node["12"]["1"].as<unsigned>() == 1);
    CHECK(node["5"].IsMap());
    CHECK(node["5"].size() == 0);

    Repeaters b(NonSevenNums);
    b.fromYAML(node);
    CHECK(b == a);
    CHECK(b.running(9) == 0);
}

//----------------------------------------------------------------

TEST_CASE("Repeaters:yaml is strict about missing numbers")
{
    Repeaters a(PointNums);
    YAML::Node node = a.toYAML();
    node.remove("8");
    Repeaters b(PointNums);
    CHECK_THROWS_AS(b.fromYAML(node), std::runtime_error);

    CHECK_THROWS_AS(b.fromYAML(YAML::Node()), std::runtime_error);
}

//----------------------------------------------------------------

namespace
{
void roll(TableStats& ts, unsigned point, unsigned d1, unsigned d2)
{
    Dice dice;
    dice.set(d1, d2);
    ts.recordDiceRoll(point, dice);
}
}

//----------------------------------------------------------------

TEST_CASE("TableStats:repeaters one point cycle, seven out")
{
    TableStats ts;
    roll(ts, 0, 1, 3);   // 4, point established
    roll(ts, 4, 1, 3);   // 4 made
    roll(ts, 4, 2, 4);   // 6
    roll(ts, 4, 1, 3);   // 4 again
    roll(ts, 4, 3, 4);   // seven out

    const auto& rs = ts.rollStats;
    CHECK(rs.pointRepeaters.times(4, 3) == 1);      // establishing roll counts
    CHECK(rs.pointRepeaters.times(6, 1) == 1);
    CHECK(rs.pointRepeaters.times(5, 0) == 0);

    CHECK(rs.placeRepeaters.times(4, 2) == 1);      // establishing roll does not
    CHECK(rs.placeRepeaters.times(6, 1) == 1);
    CHECK(rs.placeRepeaters.times(5, 0) == 1);      // zero hit runs recorded
    CHECK(rs.placeRepeaters.times(10, 0) == 1);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:repeaters come out seven ends point run only")
{
    TableStats ts;
    roll(ts, 0, 1, 3);   // 4, point established
    roll(ts, 4, 1, 3);   // 4 made
    roll(ts, 0, 3, 4);   // come out natural
    roll(ts, 0, 1, 3);   // 4, new point
    roll(ts, 4, 3, 4);   // seven out

    const auto& rs = ts.rollStats;
    CHECK(rs.pointRepeaters.times(4, 2) == 1);      // Ended by the natural
    CHECK(rs.pointRepeaters.times(4, 1) == 1);      // Ended by seven out

    // Place: the natural leaves the run alone; one hit, then the seven out
    CHECK(rs.placeRepeaters.times(4, 1) == 1);
    CHECK(rs.placeRepeaters.times(4, 2) == 0);
    CHECK(rs.placeRepeaters.times(5, 0) == 1);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:repeaters cover 2 3 11 12 but place does not")
{
    TableStats ts;
    roll(ts, 0, 6, 6);   // 12 on come out
    roll(ts, 0, 6, 6);   // 12 again
    roll(ts, 0, 5, 6);   // 11
    roll(ts, 0, 3, 4);   // 7

    const auto& rs = ts.rollStats;
    CHECK(rs.pointRepeaters.times(12, 2) == 1);
    CHECK(rs.pointRepeaters.times(11, 1) == 1);
    CHECK(rs.placeRepeaters.tracks(12)   == false);
    CHECK(rs.placeRepeaters.buckets(4).empty());    // Come out 7, no seven out
}

//----------------------------------------------------------------

TEST_CASE("RollStats:repeaters merge reset and yaml")
{
    TableStats ts;
    roll(ts, 0, 1, 3);
    roll(ts, 4, 1, 3);
    roll(ts, 4, 3, 4);

    RollStats total;
    total.merge(ts.rollStats);
    total.merge(ts.rollStats);
    CHECK(total.pointRepeaters.times(4, 2) == 2);
    CHECK(total.placeRepeaters.times(4, 1) == 2);

    RollStats loaded;
    loaded.fromYAML(total.toYAML());
    CHECK(loaded.pointRepeaters == total.pointRepeaters);
    CHECK(loaded.placeRepeaters == total.placeRepeaters);

    total.reset();
    CHECK(total.pointRepeaters.buckets(4).empty());
    CHECK(total.placeRepeaters.buckets(4).empty());
}

//----------------------------------------------------------------

//----------------------------------------------------------------

TEST_CASE("Repeaters:closeRun ends only the given numbers")
{
    Repeaters r(PointNums);
    r.hit(4); r.hit(5); r.hit(5);
    r.closeRun({5, 6}, true);       // 6 had no hits, recorded as zero
    CHECK(r.times(5, 2) == 1);
    CHECK(r.times(6, 0) == 1);
    CHECK(r.running(4)  == 1);      // Still counting
    CHECK(r.running(5)  == 0);
}

//----------------------------------------------------------------

TEST_CASE("PlayerStats:repeaters merge reset and yaml")
{
    PlayerStats a;
    a.pointRepeaters.hit(4); a.pointRepeaters.endRun();
    a.placeRepeaters.closeRun({6}, true);
    a.buyRepeaters.hit(10); a.buyRepeaters.endRun();

    PlayerStats total;
    total.merge(a);
    total.merge(a);
    CHECK(total.pointRepeaters.times(4, 1)  == 2);
    CHECK(total.placeRepeaters.times(6, 0)  == 2);
    CHECK(total.buyRepeaters.times(10, 1)   == 2);

    YAML::Node node;
    total.toYAML(node);
    PlayerStats loaded;
    loaded.fromYAML(node);
    CHECK(loaded.pointRepeaters == total.pointRepeaters);
    CHECK(loaded.placeRepeaters == total.placeRepeaters);
    CHECK(loaded.buyRepeaters   == total.buyRepeaters);

    total.reset();
    CHECK(total.pointRepeaters.buckets(4).empty());
    CHECK(total.buyRepeaters.buckets(10).empty());
}

//----------------------------------------------------------------
