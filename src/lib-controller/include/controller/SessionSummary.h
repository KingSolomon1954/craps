//----------------------------------------------------------------
//
// File: SessionSummary.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/Globals.h>
#include <craps/CrapsTable.h>
#include <gen/Timepoint.h>

namespace Ctrl {

class SessionSummary
{
public:
    Craps::TableId tableId;
    Gen::Timepoint sessionStart;
    Gen::Timepoint sessionEnd;
    Gbl::Money totAmtWin;   // Players win
    Gbl::Money totAmtLose;  // Players lose
    unsigned numRolls;
    unsigned numBetsMade;
    unsigned numPlayers;
    
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

@brief Maintain a brief record about a session

*/

} // namespace Ctrl

std::ostream& operator<< (std::ostream& os, const Ctrl::SessionSummary& ss);

//----------------------------------------------------------------
