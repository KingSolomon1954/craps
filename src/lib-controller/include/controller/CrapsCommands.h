//----------------------------------------------------------------
//
// File: CrapsCommands.h
//
// The UI uses this API to interact with the game.
// It enqueues GameEvents, actually UI events, to the GameController.
// These are events that change the state of game.
//
//----------------------------------------------------------------

#pragma once

#include <craps/EnumBetName.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtils.h>
#include <gen/ReturnCode.h>

namespace Ctrl {

class CrapsCommands
{
public:    
    CrapsCommands() = delete;  // Pure static class, no instantiation

    // TODO Eliminate return arg from function signature

    // Player related
    static uint64_t cmdPlayerJoinTable(
        const Craps::PlayerId& playerId,
        const Craps::TableId& tableId);
    static uint64_t cmdPlayerLeaveTable(
        const Craps::PlayerId& playerId,
        const Craps::TableId& tableId);
    static uint64_t cmdMakeBet(
        const Craps::PlayerId& playerId,
        BetName betName,
        Gen::Money contractAmount,
        size_t pivot);
    static uint64_t cmdMakeBetAuto(
        const Craps::PlayerId& playerId,
        BetName betName,
        Gen::Money contractAmount,
        size_t pivot);

    // Bet related
    static uint64_t cmdBetSetContractAmount(
        const Craps::BetId& betId,
        Gen::Money contractAmount);
    static Gen::ReturnCode cmdBetSetOddsAmount(
        const Craps::BetId& betId,
        Gen::Money oddsAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdBetRemove(
        const Craps::BetId& betid,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdBetSetOffComeOutRoll(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdBetSetOnComeOutRoll(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdBetSetHardwayOff(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdBetSetHardwayOn(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    
    // Table related
    static Gen::ReturnCode cmdRollDice(
        const Craps::TableId& tableId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode undoLastBet(
        Gen::ErrorPass& ep);

    // Program control
    static void gameTerminate();
    
    // Auto Fill
    struct AutoFillEntry
    {
        BetName     betName;
        size_t      pivot;
        bool        oddsBet;
        Gen::Money  amount;
    };

    static Gen::ReturnCode cmdSetAutoFill(
            const AutoFillEntry& entry,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdDeleteAutoFill(
            const AutoFillEntry& entry,
            Gen::ErrorPass& ep);

    // Quick Bet
    struct QuickBetEntry
    {
        BetName     betName;
        size_t      pivot;
        bool        oddsBet;
        Gen::Money  amount;
    };
    
    static Gen::ReturnCode cmdSetQuickBet(
            const QuickBetEntry& entry,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdApplyQuickBet(
            size_t index,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdDeleteQuickBet(
            const QuickBetEntry& entry,
            BetName betName,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode cmdDeleteQuickBet(
            size_t index,
            Gen::ErrorPass& ep);

private:
    // Private helpers
    static std::string diagPrefix(
        const std::string& funcName,
        const std::string& unableToWhat);
};

/*-----------------------------------------------------------*//**

@class CrapsCommands

@brief UI builds and enqueues an Event to perform against the model

A collection of functions that builds and enqueues GameEvents to 
perform a command on the model. These are commands that
change the game state.

See class CrapsInterfaceReader for read-only retrieval
of information from the model.

Both GUI and CUI go through this interface.

*/    

} // namespace Ctrl

//----------------------------------------------------------------


