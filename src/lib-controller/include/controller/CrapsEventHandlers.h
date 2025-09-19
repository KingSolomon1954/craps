//----------------------------------------------------------------
//
// File: CrapsEventHandlers.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>

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
    static void onUserMakeBet    (GameEvent* pBase);
    static void onUserMakeOddsBet(GameEvent* pBase);
    static void onUserRollDice   (GameEvent* pBase);
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class CrapsEventHandlers

@brief Handlers for processing events from the model -> UI.

These are called by GameController::dispatchEvent().

*/

} // namespace Ctrl

//----------------------------------------------------------------
