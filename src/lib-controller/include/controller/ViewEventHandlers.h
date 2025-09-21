//----------------------------------------------------------------
//
// File: ViewEventHandlers.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>

namespace Ctrl {

class GameEvent;  // fwd
    
class ViewEventHandlers
{
public:
    /// @name Lifecycle
    /// @{
    /// @}
    /// @}

    /// @name Modifiers
    /// @{
    static void onViewErrorDialog             (GameEvent* pBase);
    static void onViewMakeBetSuccess          (GameEvent* pBase);
    static void onViewAutomationMakeBetSuccess(GameEvent* pBase);
    static void onViewAutomationMakeBetError  (GameEvent* pBase);
    static void onViewMakeOddsBetSuccess      (GameEvent* pBase);
    static void onViewRollDice                (GameEvent* pBase);
    static void onViewRollDiceCountDown       (GameEvent* pBase);
    static void onSignalProgramExit           (GameEvent* pBase);
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ViewEventHandlers

@brief Handlers for processing events from the model -> UI.

These are called by GameController::dispatchEvent().

*/

} // namespace Ctrl

//----------------------------------------------------------------
