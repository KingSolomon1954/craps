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
    static void onCmdMakeBet        (GameEvent* pBase);
    static void onCmdMakeBetAuto    (GameEvent* pBase);
    static void onCmdMakeOddsBet    (GameEvent* pBase);
    static void onCmdMakeOddsBetAuto(GameEvent* pBase);
    static void onCmdRollDice       (GameEvent* pBase);
    static void onCmdPlayerJoinTable(GameEvent* pBase);
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
