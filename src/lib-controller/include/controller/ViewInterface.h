//----------------------------------------------------------------
//
// File: ViewInterface.h
//
//----------------------------------------------------------------

#pragma once

namespace Ctrl {

class ViewInterface
{
public:
    /// @name Lifecycle
    /// @{
    virtual void init() = 0;
    virtual void prepareForShutdown() = 0;
    /// @}
    /// @}

    /// @name Modifiers
    /// @{
    virtual void emitViewErrorDialog()        = 0;
    virtual void emitViewMakeBetSuccess()     = 0;
    virtual void emitViewMakeOddsBetSuccess() = 0;
    virtual void emitViewRollDice()           = 0;
    virtual void emitSignalProgramExit()      = 0;
    /// @}
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ViewInterface

@brief Model uses these function to build UI Events

A collection of functions that builds and enqueues GameEvents to inform
the UI that there are results from a previous request or that the UI
needs to show something.
*/

} // namespace Ctrl

//----------------------------------------------------------------
