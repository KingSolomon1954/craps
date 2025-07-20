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

TEST_CASE("SessionHistory:addSessionSummary")
{
    SessionHistory sh;
    auto ss = sh.getSessions();
    CHECK(sh.getSessionCount() == 0);
    CHECK(ss.size() == 0);
    
    unsigned numPlayers      = 6;
    unsigned numBets         = 100;
    Gen::Money amtDeposited  = 10000;
    Gen::Money amtWithdrawn  = 12000;
    sh.addSessionSummary(numPlayers, numBets, amtDeposited, amtWithdrawn);
    
    CHECK(sh.getSessionCount() == 1);
}

//----------------------------------------------------------------

TEST_CASE("SessionHistory:yaml roundtrip")
{
    using namespace Gen;

    SessionHistory original;
    original.addSessionSummary(1, 100, 150, 200);
    original.addSessionSummary(2, 200, 250, 300);

    YAML::Node yaml = original.toYAML();

    SessionHistory loaded;
    loaded.fromYAML(yaml);

    const SessionHistory::Sessions& ld = loaded.getSessions();
    const SessionHistory::Sessions& og = loaded.getSessions();
    
    CHECK(loaded.getSessionCount() == original.getSessionCount());
    CHECK(ld.size() == og.size());
    
    for (unsigned i = 0; i < original.getSessionCount(); ++i)
    {
        CHECK(ld[i].numPlayers == og[i].numPlayers);
        CHECK(ld[i].numBets    == og[i].numBets);
        CHECK(ld[i].amtIntake  == og[i].amtIntake);
        CHECK(ld[i].amtPayout  == og[i].amtPayout);
    }
}

//----------------------------------------------------------------

TEST_CASE("SessionHistory:max sessions")
{
    // Simulate loading a YAML node with more than 50 sessions, and
    // verify that:
    // 
    // Only the most recent 50 sessions were retained by
    // SessionHistory::fromYAML().
    // 
    // The oldest 10 sessions were discarded (i.e. the first 10 entries
    // in the YAML input string).
    
    SessionHistory history;
    const size_t totalSessions = history.getMaxSessions() + 10;
    const size_t maxSessions   = history.getMaxSessions();
    const size_t dropped       = totalSessions - maxSessions;

    // Build YAML string with 60 sessions
    std::ostringstream yamlStream;
    yamlStream << "  numSessionsAlltime: 12\n"
               << "  firstSessionDate: 2025-06-25T14:30:00.000000000\n"
               << "  longestSessionAlltime: 0d 00:01:04\n"
               << "  history:\n";
    for (unsigned i = 1; i <= totalSessions; ++i)
    {
        yamlStream << "    - numPlayers: " << i                   << "\n"
                   << "      numBets: "    << i * 100             << "\n"
                   << "      amtIntake: "  << i * 100 + 50        << "\n"
                   << "      amtPayout: "  << i * 100 + 75        << "\n"
                   << "      amtPayout: "  << i * 100 + 75        << "\n"
                   << "      date: 2025-06-25T14:30:00.000000000" << "\n"
                   << "      duration: 0d 00:00:08"               << "\n";
    }
    
    YAML::Node root = YAML::Load(yamlStream.str());

    history.fromYAML(root);

    // Confirm it's capped at max
    CHECK(history.getSessionCount() == maxSessions);

    const SessionHistory::Sessions& sh = history.getSessions();

    // Confirm the first `dropped` entries were eliminated
    for (size_t i = 0; i < maxSessions; ++i)
    {
        const auto& entry = sh[i];
        unsigned expectedNumPlayers = static_cast<unsigned>(dropped + i + 1);
        CHECK(entry.numPlayers == expectedNumPlayers);
    }
}

//----------------------------------------------------------------
