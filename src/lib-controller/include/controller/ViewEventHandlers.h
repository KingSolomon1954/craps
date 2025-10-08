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
    static void onViewSuccess           (GameEvent* pBase);
    static void onViewErrorDialog       (GameEvent* pBase);
    static void onViewMakeBetSuccess    (GameEvent* pBase);
    static void onViewMakeBetAutoSuccess(GameEvent* pBase);
    static void onViewMakeBetAutoError  (GameEvent* pBase);
    static void onViewMakeOddsBetSuccess(GameEvent* pBase);
    static void onViewRollDiceCountDown (GameEvent* pBase);
    static void onViewRollDiceAnimation (GameEvent* pBase);
    static void onViewProgramExit       (GameEvent* pBase);
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
