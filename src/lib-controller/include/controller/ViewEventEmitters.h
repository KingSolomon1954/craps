//----------------------------------------------------------------
//
// File: ViewEventEmitters.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>

namespace Ctrl {

class GameEvent;  // fwd
    
class ViewEventEmitters
{
public:
    /// @name Lifecycle
    /// @{
    /// @}
    /// @}

    /// @name Modifiers
    /// @{
    static void emitViewErrorDialog();
    static void emitViewMakeBetSuccess();
    static void emitViewMakeOddsBetSuccess();
    static void emitViewRollDice();
    static void emitSignalProgramExit();
    /// @}
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ViewEventEmitters

@brief Model uses these function to build UI Events

A collection of functions that builds and enqueues GameEvents to inform
the UI that there are results from a previous request or that the UI
needs to show something.
*/

} // namespace Ctrl

//----------------------------------------------------------------
