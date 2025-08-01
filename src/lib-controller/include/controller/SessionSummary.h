//----------------------------------------------------------------
//
// File: SessionSummary.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/Timepoint.h>

namespace Ctrl {

class SessionSummary
{
public:
    std::string name;             // Player or Table name
    Gen::Timepoint sessionStart;
    Gen::Timepoint sessionEnd;
    Gbl::Money totAmtWin  = 0;    // Players win
    Gbl::Money totAmtLose = 0;    // Players lose
    unsigned numRolls     = 0;
    unsigned numBetsMade  = 0;
    unsigned numPlayers   = 0;    // Filled out only if Table summary
    
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class SessionSummary

@brief A record about a player or table session

Structure suitable for UI presentation.

Translates from Craps table session history or from Player
session history for UI handling.

*/

} // namespace Ctrl

std::ostream& operator<< (std::ostream& os, const Ctrl::SessionSummary& ss);

//----------------------------------------------------------------
