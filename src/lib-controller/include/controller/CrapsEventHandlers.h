//----------------------------------------------------------------
//
// File: CrapsEventHandlers.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>
#include <string>

namespace Ctrl {

class GameEvent;  // fwd
    
class CrapsEventHandlers
{
public:
    /// @name Lifecycle
    /// @{
    /// @}
    /// @}

    /// @name Modifiers
    /// @{
    // Player Related
    static void onCmdPlayerJoinTable (GameEvent* pBase);
    static void onCmdPlayerLeaveTable(GameEvent* pBase);
    static void onCmdMakeBet         (GameEvent* pBase);
    static void onCmdMakeBetAuto     (GameEvent* pBase);
    
    // Bet Related
    static void onCmdBetSetContractAmount(GameEvent* pBase);
    static void onCmdBetSetOddsAmount    (GameEvent* pBase);
    static void onCmdBetSetOffComeOutRoll(GameEvent* pBase);
    static void onCmdBetSetOnComeOutRoll (GameEvent* pBase);
    static void onCmdBetSetHardwayOff    (GameEvent* pBase);
    static void onCmdBetSetHardwayOn     (GameEvent* pBase);

    // Table Related
    static void onCmdRollDice(GameEvent* pBase);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    static std::string diagPrefix(const std::string& funcName,
                                  const std::string& unableToWhat);
    
};

/*-----------------------------------------------------------*//**

@class CrapsEventHandlers

@brief Handlers for processing events from the model -> UI.

These are called by GameController::dispatchEvent().

*/

} // namespace Ctrl

//----------------------------------------------------------------
