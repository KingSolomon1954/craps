//----------------------------------------------------------------
//
// File: TableStatsPart6Test.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <filesystem>
#include <controller/Globals.h>
#include <gen/Logger.h>

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
    Gen::Logger log;
    Gbl::pLogger = &log;

    TableStats ts("LasVegas");

    // Set a stat or two in there to something non zero.
    ts.betStats.totNumBetsAllBets = 999;
    ts.rollStats.numberCounts[10].count_ = 998;

    fs::create_directories("/tmp/utest");
    ts.saveFile("/tmp/utest");  // Save stats

    // Read file back in ts2
    TableStats ts2("LasVegas");
    ts2.loadFile("/tmp/utest");

//  Want to look at file before clobbering it
//  ts2.tableId = "id2";         // This changes name of file when saving
//  ts2.saveFile("/tmp/utest");  // Save it to a file named with "id2"
//  ts2.tableId = "LasVegas";    // Restore original name
    
    // Compare some equal fields
    CHECK(ts2.betStats.totNumBetsAllBets == 999);
    CHECK(ts2.rollStats.numberCounts[10].count_ == 998);

    // Can't use operator == because currentSessionStartTime will differ.
    // CHECK(ts == ts2);
}

//----------------------------------------------------------------
