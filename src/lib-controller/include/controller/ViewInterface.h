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
    virtual void onViewErrorDialog(
        EventType              orgEventType,
        uint64_t               correlationId,
        const std::string&     diag) = 0;
    virtual void onViewMakeBetSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId) = 0;
    virtual void onViewMakeOddsBetSuccess() = 0;
    virtual void onViewAutomationMakeBetSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId) = 0;
    virtual void onViewAutomationMakeBetError(
        EventType              orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag) = 0;
    virtual void onViewRollDice()           = 0;
    virtual void onSignalProgramExit()      = 0;
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
