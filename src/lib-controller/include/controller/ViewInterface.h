//----------------------------------------------------------------
//
// File: ViewInterface.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <controller/GameEvent.h>

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
    virtual void showErrorDialog(
        EventType              orgEventType,
        uint64_t               correlationId,
        const std::string&     diag) = 0;
    virtual void showMakeBetSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId) = 0;
    virtual void showMakeOddsBetSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId) = 0;
    virtual void showMakeBetAutoSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId) = 0;
    virtual void showMakeBetAutoError(
        EventType              orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag) = 0;
    virtual void showRollDice() = 0;
    virtual void showProgramExit() = 0;
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
