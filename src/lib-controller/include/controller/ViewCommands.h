//----------------------------------------------------------------
//
// File: ViewCommands.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>

namespace Ctrl {

class GameEvent;  // fwd
    
class ViewCommands
{
public:
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    // Commands issued from the Model to UI
    static void emitViewErrorDialog(
        EventType              orgEventType,
        uint64_t               correlationId,
        const std::string&     diag);
    static void emitViewMakeBetSuccess(
        uint64_t               correlationId,
        Craps::BetId           betId);
    static void emitViewMakeOddsBetSuccess(
        uint64_t               correlationId,
        Craps::BetId           betId);
    static void emitViewMakeBetAutoSuccess(
        Craps::BetId           betId,
        uint64_t               correlationId);
    static void emitViewMakeBetAutoError(
        EventType              orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag);
    static void emitViewSuccess(
        uint64_t               correlationId);
    static void emitViewRollDice();
    static void emitViewProgramExit();
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ViewCommands

@brief Model uses these function to build UI Events

A collection of functions that builds and enqueues GameEvents to inform
the UI that there are results from a previous request or that the UI
needs to show something.
*/

} // namespace Ctrl

//----------------------------------------------------------------
