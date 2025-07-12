//----------------------------------------------------------------
//
// File: SessionHistory.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <controller/Globals.h>
#include <gen/Timepoint.h>

namespace Craps {

class SessionHistory
{
public:
    struct Summary
    {
        unsigned numBets     = 0;
        Gbl::Money amtIntake = 0;
        Gbl::Money amtPayout = 0;
        unsigned numPlayers  = 0;
        Gen::Timepoint date; 
        Gen::Timepoint::Duration duration;
        
        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
    };

    using Sessions = std::vector<Summary>;

    void addNewSummary(unsigned numPlayers,
                       Gbl::Money amtDeposited,
                       Gbl::Money amtWithdrawn,
                       const std::string& sessionStartStr,
                       Gen::Timepoint::Duration duration);
    void saveFile(const std::string& dir, const std::string& tableId) const;
    void loadFile(const std::string& dir, const std::string& tableId);
    const Sessions& getSessionHistory() const;
    
private:
    Sessions sessions_;
    
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
};

/*-----------------------------------------------------------*//**

@class SessionHistory

@brief Capture session summaries in a history file

Goal is to make possible a user display similar to this:

@code
Session History
---------------

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
