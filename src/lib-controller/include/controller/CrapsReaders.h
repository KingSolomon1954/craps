//----------------------------------------------------------------
//
// File: CrapsReaders.h
//
// The UI uses this API to interact with the game.
// It retries read-only information directly from the model 
// without enqueueing GameEvents.
//
//----------------------------------------------------------------

#pragma once

#include <deque>
#include <controller/PlayerManifest.h>
#include <controller/TableManifest.h>
#include <craps/EnumBetName.h>
#include <craps/CrapsTypes.h>
#include <craps/SessionHistory.h>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtils.h>
#include <gen/ReturnCode.h>

namespace Craps {
    class BankStats;       // fwd
    class Dice;            // fwd
    class LastRollStats;   // fwd
    class PlayerStats;     // fwd
    class TableStats;      // fwd
}

namespace Ctrl {

class CrapsReaders
{
public:    
    CrapsReaders() = delete;  // Pure static class, no instantiation

    // Player related
    static Gen::ReturnCode readPlayerHaveBet(
        const Craps::PlayerId& playerId,
        const Craps::BetId& betId,
        bool& haveBet,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerGetBets(
        const Craps::PlayerId& playerId,
        std::vector<Craps::BetId>& betIds,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerGetOddsBets(
        const Craps::PlayerId& playerId,
        std::vector<Craps::BetId>& betIds,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerName(
        const Craps::PlayerId& playerId,
        std::string& playerName,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerAmountOnTable(
        const Craps::PlayerId& playerId,
        Gen::Money& amount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerNumBetsOnTable(
        const Craps::PlayerId& playerId,
        unsigned& numBets,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerBalance(
        const Craps::PlayerId& playerId,
        Gen::Money& balance,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerLastRollStats(
        const Craps::PlayerId& playerId,
        Craps::LastRollStats& lastRollStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerCurrentStats(
        const Craps::PlayerId& playerId,
        Craps::PlayerStats& playerCurrentStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerAlltimeStats(
        const Craps::PlayerId& playerId,
        Craps::PlayerStats& playerAlltimeStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerCurrentStatsBank(
        const Craps::PlayerId& playerId,
        Craps::BankStats& playerCurrentStatsBank,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readPlayerSessionHistory(
        const Craps::PlayerId& playerId,
        Craps::SessionHistory::Sessions& playerSessionHistory,
        Gen::ErrorPass& ep);
    static const PlayerManifest::PlayerList& readPlayerList();
    
    // Bet related
    static Gen::ReturnCode readBetIsRemovable(
        const Craps::BetId& betid,
        bool& isRemovable,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetPivot(
        const Craps::BetId& betId,
        unsigned& pivot,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetPlayerId(
        const Craps::BetId& betId,
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetName(
        const Craps::BetId& betId,
        BetName& betName,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetContractAmount(
        const Craps::BetId& betId,
        Gen::Money& contractAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetOddsAmount(
        const Craps::BetId& betId,
        Gen::Money& oddsAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetOffComeOutRoll(
        const Craps::BetId& betId,
        bool& offComeOutRoll,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetHardwayWorking(
        const Craps::BetId& betId,
        bool& hardwayWorking,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetDistance(
        const Craps::BetId& betId,
        unsigned& distance,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetWhenCreated(
        const Craps::BetId& betId,
        Gen::Timepoint& whenCreated,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readBetWhenDecided(
        const Craps::BetId& betId,
        Gen::Timepoint& whenDecided,
        Gen::ErrorPass& ep);
    
    // Table related
    static Gen::ReturnCode readTableNumPlayers(
        const Craps::TableId& tableId,
        unsigned& numPlayers,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTablePoint(
        const Craps::TableId& tableId,
        unsigned& point,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableCurrentRoll(
        const Craps::TableId& tableId,
        Craps::Dice& dice,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableNumRolls(
        const Craps::TableId& tableId,
        unsigned& numRolls,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableAmountOnTable(
        const Craps::TableId& tableId,
        Gen::Money& amount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableNumBetsOnTable(
        const Craps::TableId& tableId,
        unsigned& numBets,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTablePlayers(
        const Craps::TableId& tableId,
        std::vector<Craps::PlayerId>& playerIds,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableCurrentShooter(
        const Craps::TableId& tableId,
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableRecentRolls(
        const Craps::TableId& tableId,
        std::deque<Craps::Dice>& recentRolls,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  readTableCurrentStats(
        const Craps::TableId& tableId,
        Craps::TableStats& currentTableStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  readTableAlltimeStats(
        const Craps::TableId& tableId,
        Craps::TableStats& alltimeTableStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  readTableCurrentStatsBank(
        const Craps::TableId& tableId,
        Craps::BankStats& currentStatsBank,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  readTableAlltimeStatsBank(
        const Craps::TableId& tableId,
        Craps::BankStats& alltimeStatsBank,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableLastRollStats(
        const Craps::TableId& tableId,
        Craps::LastRollStats& lastRollStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  readTableSessionHistory(
        const Craps::TableId& tableId,
        Craps::SessionHistory::Sessions& tableSessionHistory,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableIsComeOutRoll(
        const Craps::TableId& tableId,
        bool& isComeOutRoll,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableIsBettingOpen(
        const Craps::TableId& tableId,
        bool& isBettingOpen,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableHaveBet(
        const Craps::TableId& tableId,
        const Craps::BetId& betId,
        bool& haveBet,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode readTableHavePlayer(
        const Craps::TableId& tableId,
        const Craps::PlayerId& playerId,
        bool& havePlayer,
        Gen::ErrorPass& ep);
    static const TableManifest::TableList& readTableList();

    // Program control
    static Gen::ReturnCode getActiveCrapsTable(
        Craps::TableId& tableId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode getUserPlayer(
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    
    // Auto Fill
    struct AutoFillEntry
    {
        BetName     betName;
        size_t      pivot;
        bool        oddsBet;
        Gen::Money  amount;
    };

    static Gen::ReturnCode getAutoFill(
            AutoFillEntry& entry,
            Gen::ErrorPass& ep);
    static Gen::ReturnCode getAutoFills(
            std::vector<AutoFillEntry>& autoFills,  // return arg
            Gen::ErrorPass& ep);

    // Quick Bet
    struct QuickBetEntry
    {
        BetName     betName;
        size_t      pivot;
        bool        oddsBet;
        Gen::Money  amount;
    };
    
    static Gen::ReturnCode getQuickBets(
            std::vector<QuickBetEntry>& quickBets,  // return arg
            Gen::ErrorPass& ep);

private:
    // Private helpers
    static std::string diagPrefix(
        const std::string& funcName,
        const std::string& unableToWhat);
};

/*-----------------------------------------------------------*//**

@class CrapsReaders

@brief UI retrieves read-only information directly from the model

A collection of functions that retrieves information from the model
directly, bypassing the EventLoop. These are read-only retrievals that
do not change the game state.

See class CrapsCommands for issueing commands
that change the model state.

Both GUI and CUI go through this interface.

*/    

} // namespace Ctrl

//----------------------------------------------------------------


