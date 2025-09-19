//----------------------------------------------------------------
//
// File: CrapsEventEmitters.h
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

class CrapsEventEmitters
{
public:    
    CrapsEventEmitters() = delete;  // Pure static class, no instantiation

    // TODO Eliminate return arg from function signature

    // Player related
    static Gen::ReturnCode playerJoinTable(
        const Craps::PlayerId& playerId,
        const Craps::TableId& tableId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerLeaveTable(
        const Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerMakeBet(
        const Craps::PlayerId& playerId,
        BetName betName,
        Gen::Money contractAmount,
        unsigned pivot,
        Craps::BetId& betId,
        Gen::ErrorPass& ep);

    // Bet related
    static Gen::ReturnCode betSetContractAmount(
        const Craps::BetId& betId,
        Gen::Money contractAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betSetOddsAmount(
        const Craps::BetId& betId,
        Gen::Money oddsAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betRemove(
        const Craps::BetId& betid,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betSetOffComeOutRoll(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betSetOnComeOutRoll(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betSetHardwayOff(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betSetHardwayOn(
        const Craps::BetId& betId,
        Gen::ErrorPass& ep);
    
    // Table related
    static Gen::ReturnCode rollDice(
        const Craps::TableId& tableId,
        Gen::ErrorPass& ep);

    // Program control
    static Gen::ReturnCode getActiveCrapsTable(
        Craps::TableId& tableId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode getUserPlayer(
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode undoLast(
        Gen::ErrorPass& ep);
    static void gameTerminate();
    
    // Auto Fill
    struct AutoFillEntry
    {
        BetName     betName;
        size_t      pivot;
        bool        oddsBet;
        Gen::Money  amount;
    };

    static Gen::ReturnCode setAutoFill(
            const AutoFillEntry& entry,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode deleteAutoFill(
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
    
    static Gen::ReturnCode setQuickBet(
            const QuickBetEntry& entry,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode applyQuickBet(
            size_t index,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode deleteQuickBet(
            const QuickBetEntry& entry,
            BetName betName,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode deleteQuickBet(
            size_t index,
            Gen::ErrorPass& ep);

private:
    // Private helpers
    static std::string diagPrefix(
        const std::string& funcName,
        const std::string& unableToWhat);
};

/*-----------------------------------------------------------*//**

@class CrapsEventEmitters

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


