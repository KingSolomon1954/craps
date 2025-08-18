//----------------------------------------------------------------
//
// File: CrapsInterface.h
//
// The UI uses this API to interact with the game.
// It is the only API the UI sees.
//
//----------------------------------------------------------------

#pragma once

#include <craps/EnumBetName.h>
#include <craps/CrapsTypes.h>
#include <craps/SessionHistory.h>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtils.h>
#include <gen/ReturnCode.h>

namespace Craps {
    class BankStats;       // fwd
    class PlayerStats;     // fwd
}

namespace Ctrl {

class CrapsInterface
{
    // Player related
    static Gen::ReturnCode joinTable(const Craps::PlayerId& playerId,
                                     const Craps::TableId& tableId,
                                     Gen::ErrorPass& ep);
    static Gen::ReturnCode leaveTable(const Craps::PlayerId& playerId,
                                      Gen::ErrorPass& ep);
    static Craps::BetId makeBet(const Craps::PlayerId& playerId,
                                BetName betName,
                                Gen::Money contractAmount,
                                unsigned pivot,
                                Gen::ErrorPass& ep);
    static Gen::ReturnCode setOddsAmount(const Craps::PlayerId& playerId,
                                         Craps::BetId betId,
                                         Gen::Money oddsAmount,
                                         Gen::ErrorPass& ep);
    static bool isBetRemovable(const Craps::PlayerId& playerId,
                               Craps::BetId betid);
    static Gen::ReturnCode removeBet(const Craps::PlayerId& playerId,
                                     Craps::BetId betid,
                                     Gen::ErrorPass& ep);
    static const Craps::PlayerStats& getCurrentStatsPlayer(
                                     const Craps::PlayerId& playerId);
    static const Craps::PlayerStats& getAlltimeStatsPlayer(
                                     const Craps::PlayerId& playerId);
    static const Craps::BankStats& getBankCurrentStatsPlayer(
                                     const Craps::PlayerId& playerId);
    static const Craps::BankStats& getBankAlltimeStatsPlayer(
                                     const Craps::PlayerId& playerId);
    static const Craps::SessionHistory::Sessions& getSessionHistoryPlayer(
                                     const Craps::PlayerId& playerId);

    // Table related
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
