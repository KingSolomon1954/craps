//----------------------------------------------------------------
//
// File: TableStatsPart6Test.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <filesystem>

#define private public
#define protected public
    // Added this include so that private functions
    // in the following classes can be tested
    #include <craps/TableStats.h>
#undef private
#undef protected

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("TableStats:savefile")
{
    namespace fs = std::filesystem;
    
    TableStats ts("LasVegas");

    // Set a stat or two in there to something non zero.
    ts.betStats.totNumBetsAllBets = 999;
    ts.rollStats.numberCounts[10].count_ = 998;
    
    fs::create_directories("/tmp/utest");

    // Save stats
    ts.saveFile("/tmp/utest");

    // Read it back and compare equals
    TableStats ts2("LasVegas");
    ts2.loadFile("/tmp/utest");

//  Wanted to look at file before clobbering it     
//  ts2.tableId = "id2";         // Change name of file
//  ts2.saveFile("/tmp/utest");  // Save it
//  ts2.tableId = "LasVegas";    // Restore name
    CHECK(ts2.betStats.totNumBetsAllBets == 999);
    CHECK(ts2.rollStats.numberCounts[10].count_ == 998);

    // Force session timestamps to agree.
    Gen::Timepoint mark;
    ts.sessionStart = mark;
    ts.sessionEnd = mark;
    ts2.sessionStart = mark;
    ts2.sessionEnd = mark;

    CHECK(ts == ts2);
}

//----------------------------------------------------------------

