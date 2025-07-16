//----------------------------------------------------------------
//
// File: SessionHistory.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <gen/MoneyUtil.h>
#include <gen/Timepoint.h>

namespace Craps {

class SessionHistory
{
public:
    struct Summary
    {
        unsigned numPlayers  = 0;
        unsigned numBets     = 0;
        Gen::Money amtIntake = 0;
        Gen::Money amtPayout = 0;
        Gen::Timepoint date; 
        Gen::Timepoint::Duration duration;
        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
        bool operator==(const Summary&) const = default;  // Only generates ==
    };

    // Oldest summary in first position. Most recent summary in last.
    using Sessions = std::vector<Summary>;

    // Getters
    unsigned                 getNumSessionsAlltime()    const;
    Gen::Timepoint           getFirstSessionDate()      const;
    Gen::Timepoint::Duration getLongestSessionAlltime() const;
    Gen::Timepoint           getCurSessionStartTime()   const;
    const Sessions&          getSessions()              const;

    void addSessionSummary(
        unsigned numPlayers,
        unsigned numBets,
        Gen::Money amtDeposited,
        Gen::Money amtWithdrawn);
    
    // YAML operations
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const SessionHistory&) const = default;  // Only generates ==
    
private:
    unsigned numSessionsAlltime_ = 0;
    Gen::Timepoint curSessionStartTime_;
    Gen::Timepoint firstSessionDate_;
    Gen::Timepoint::Duration longestSessionAlltime_ = std::chrono::seconds::zero();
    Sessions sessions_;
};

/*-----------------------------------------------------------*//**

@class SessionHistory

@brief Capture session summaries in a history file

Goal is to make possible a user display similar to this:

@code
Las Vegas Table Session History   or Lefty the Gambler Session History
-------------------------------      ---------------------------------

Date                Duration    Bets   Intake   Payout  Balance Players
-----------------   ----------- ------ -------- ------- ------- -------
2025-07-12T09:29:56 0d 01:33:08 112    246222   334577  +88355  6
2025-05-12T08:05:44 0d 00:22:11 24     1009     950     +59     6
@endcode

Responsibilities:

@li SessionHistory file is YAML
@li Located same place as (Table/Player)Stats file
@li Write current session summary to file
@li Read SessionHistory file
@li Make available a vector of session summaries

*/

} // namespace Craps

//----------------------------------------------------------------
