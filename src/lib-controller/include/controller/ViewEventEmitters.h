//----------------------------------------------------------------
//
// File: ViewEventEmitters.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>

namespace Ctrl {

class GameEvent;  // fwd
    
class ViewEventEmitters
{
public:
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    static void emitViewErrorDialog(
        EventType              orgEventType,
        uint64_t               correlationId,
        const std::string&     diag);
    static void emitViewMakeBetSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId);
    static void emitViewMakeOddsBetSuccess();
    static void emitViewAutomationMakeBetSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId);
    static void emitViewAutomationMakeBetError(
        EventType              orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag);
    static void emitViewRollDice();
    static void emitSignalProgramExit();
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
