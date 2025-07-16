//----------------------------------------------------------------
//
// File: SessionHistoryTest.cpp
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <craps/SessionHistory.h>
#include <chrono>
#include <thread>

#if 0
#define private public
#define protected public
    // Added this include so that private functions
    // in the following classes can be tested
    #include <craps/SessionHistory.h>
#undef private
#undef protected
#endif

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("SessionHistory:ctor")
{
    SessionHistory sh;

    // ctor & initialization
    CHECK(sh.getNumSessionsAlltime() == 0);
    CHECK(sh.getLongestSessionAlltime().count() == 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    Gen::Timepoint now;
    CHECK(sh.getFirstSessionDate()    < now);
    CHECK(sh.getCurSessionStartTime() < now);

    auto ss = sh.getSessions();
    CHECK(ss.size() == 0);

    // copy operator
    SessionHistory sh2 = sh;

    // Equals operator
    CHECK(sh2 == sh);    
}

//----------------------------------------------------------------

TEST_CASE("SessionHistory:yaml roundtrip")
{
    SessionHistory sh;
    auto ss = sh.getSessions();
    CHECK(ss.size() == 0);
    
    unsigned numPlayers      = 6;
    unsigned numBets         = 100;
    Gen::Money amtDeposited  = 10000;
    Gen::Money amtWithdrawn  = 12000;
    sh.addSessionSummary(numPlayers, numBets, amtDeposited, amtWithdrawn);
    
    ss = sh.getSessions();
    CHECK(ss.size() == 1);
    
#if 0
    unsigned numPlayers,
    unsigned numBets,
    Gen::Money amtDeposited,
    Gen::Money amtWithdrawn)
#endif
    }

//----------------------------------------------------------------
