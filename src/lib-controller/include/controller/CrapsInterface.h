//----------------------------------------------------------------
//
// File: CrapsInterface.h
//
// The UI uses this API to interact with the game.
// It is the only API the UI sees.
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

class CrapsInterface
{
public:    
    CrapsInterface() = delete;  // Pure static class, no instantiation

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
    static Gen::ReturnCode playerHaveBet(
        const Craps::PlayerId& playerId,
        const Craps::BetId& betId,
        bool& haveBet,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerGetBets(
        const Craps::PlayerId& playerId,
        std::vector<Craps::BetId>& betIds,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerGetOddsBets(
        const Craps::PlayerId& playerId,
        std::vector<Craps::BetId>& betIds,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerName(
        const Craps::PlayerId& playerId,
        std::string& playerName,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerAmountOnTable(
        const Craps::PlayerId& playerId,
        Gen::Money& amount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerNumBetsOnTable(
        const Craps::PlayerId& playerId,
        unsigned& numBets,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerBalance(
        const Craps::PlayerId& playerId,
        Gen::Money& balance,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerLastRollStats(
        const Craps::PlayerId& playerId,
        Craps::LastRollStats& lastRollStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerCurrentStats(
        const Craps::PlayerId& playerId,
        Craps::PlayerStats& playerCurrentStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerAlltimeStats(
        const Craps::PlayerId& playerId,
        Craps::PlayerStats& playerAlltimeStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerCurrentStatsBank(
        const Craps::PlayerId& playerId,
        Craps::BankStats& playerCurrentStatsBank,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode playerSessionHistory(
        const Craps::PlayerId& playerId,
        Craps::SessionHistory::Sessions& playerSessionHistory,
        Gen::ErrorPass& ep);
    static const PlayerManifest::PlayerList& playerList();
    
    // Bet related
    static Gen::ReturnCode betSetOddsAmount(
        const Craps::BetId& betId,
        Gen::Money oddsAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betSetContractAmount(
        const Craps::BetId& betId,
        Gen::Money contractAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betIsRemovable(
        const Craps::BetId& betid,
        bool& isRemovable,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betRemove(
        const Craps::BetId& betid,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betPivot(
        const Craps::BetId& betId,
        unsigned& pivot,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betPlayerId(
        const Craps::BetId& betId,
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betName(
        const Craps::BetId& betId,
        BetName& betName,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betContractAmount(
        const Craps::BetId& betId,
        Gen::Money& contractAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betOddsAmount(
        const Craps::BetId& betId,
        Gen::Money& oddsAmount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betOffComeOutRoll(
        const Craps::BetId& betId,
        bool& offComeOutRoll,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betHardwayWorking(
        const Craps::BetId& betId,
        bool& hardwayWorking,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betDistance(
        const Craps::BetId& betId,
        unsigned& distance,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betWhenCreated(
        const Craps::BetId& betId,
        Gen::Timepoint& whenCreated,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode betWhenDecided(
        const Craps::BetId& betId,
        Gen::Timepoint& whenDecided,
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
    static Gen::ReturnCode tableNumPlayers(
        const Craps::TableId& tableId,
        unsigned& numPlayers,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tablePoint(
        const Craps::TableId& tableId,
        unsigned& point,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableCurrentRoll(
        const Craps::TableId& tableId,
        Craps::Dice& dice,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableNumRolls(
        const Craps::TableId& tableId,
        unsigned& numRolls,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableAmountOnTable(
        const Craps::TableId& tableId,
        Gen::Money& amount,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableNumBetsOnTable(
        const Craps::TableId& tableId,
        unsigned& numBets,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tablePlayers(
        const Craps::TableId& tableId,
        std::vector<Craps::PlayerId>& playerIds,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableCurrentShooter(
        const Craps::TableId& tableId,
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableRecentRolls(
        const Craps::TableId& tableId,
        std::deque<Craps::Dice>& recentRolls,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  tableCurrentStats(
        const Craps::TableId& tableId,
        Craps::TableStats& currentTableStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  tableAlltimeStats(
        const Craps::TableId& tableId,
        Craps::TableStats& alltimeTableStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  tableCurrentStatsBank(
        const Craps::TableId& tableId,
        Craps::BankStats& currentStatsBank,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  tableAlltimeStatsBank(
        const Craps::TableId& tableId,
        Craps::BankStats& alltimeStatsBank,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableLastRollStats(
        const Craps::TableId& tableId,
        Craps::LastRollStats& lastRollStats,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode  tableSessionHistory(
        const Craps::TableId& tableId,
        Craps::SessionHistory::Sessions& tableSessionHistory,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableIsComeOutRoll(
        const Craps::TableId& tableId,
        bool& isComeOutRoll,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableIsBettingOpen(
        const Craps::TableId& tableId,
        bool& isBettingOpen,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableHaveBet(
        const Craps::TableId& tableId,
        const Craps::BetId& betId,
        bool& haveBet,
        Gen::ErrorPass& ep);
    static Gen::ReturnCode tableHavePlayer(
        const Craps::TableId& tableId,
        const Craps::PlayerId& playerId,
        bool& havePlayer,
        Gen::ErrorPass& ep);
    static const TableManifest::TableList& tableList();

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
    
private:
    // Private helpers
    static std::string diagPrefix(
        const std::string& funcName,
        const std::string& unableToWhat);
};

/*-----------------------------------------------------------*//**

@class CrapsInterface

*/    

} // namespace Ctrl

//----------------------------------------------------------------







#if 0
class CrapsInterface
{
    // Table related (TableManager)
    using TableId = std::string;
    using TableList = std::vector<TableId>;
    static TableList getTableList() const;
    static TablePickerView getTablePickerView(TableId tableId);
    static TableGameView getTableGameView(TableId tableId);
    static TableStatsView getTableStatsView(TableId tableId);

    // Player related (PlayerManager)
    using PlayerId = std::string;
    using PlayerList = std::vector<PlayerId> ;
    static PlayerList getPlayerList() const;
    static PlayerPickerView getPlayerPickerView(PlayerId playerId);
    static PlayerGameView getPlayerGameView(PlayerId playerId);
    static PlayerStatsView getPlayerStatsView(TableId tableId);
    
    static Gen::ReturnCode Craps::TableStats
        getCurrentTableStats(TableId tableId, Gen::ErrorPass& ep);
    static Gen::ReturnCode Craps::TableStats
        getAlltimeTableStats(TableId tableId, Gen::ErrorPass& ep);

    static Gen::ReturnCode openTable(TableId tableId, PlayerList p);
    static Gen::ReturnCode closeTable(TableId tableId);

    using BetId = unsigned;
    using BetList = std::list<BetId>;
    using Money = unsigned;

    static ReturnCode join(TableId tableId, PlayerId playerId, Gen::ErrorPass& ep);
    static ReturnCode leave(TableId tableId, PlayerId playerId, Gen::ErrorPass& ep);
    
    static BetId placeBet(TableId tableId,
                          PlayerId playerId,
                          BetNameStr betName,
                          Gen::Money contractAmount,
                          Gen::ErrorPass& ep);
    static ReturnCode removeBet(TableId tableId, BetId bet, Gen::ErrorPass& ep);
    static ReturnCode setOdds(TableId tableId, BetId bet, Gen::Money oddsAmount, Gen::ErrorPass& ep);
    static ReturnCode changeBetAmount(TableId tableId, BetId bet, int delta, Gen::ErrorPass& ep);
    static void rollDice(TalbeId tableId);
    
};    
#endif
