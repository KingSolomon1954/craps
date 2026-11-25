//----------------------------------------------------------------
//
// File: WorkOrder.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>
#include <ncurses.h>

namespace Cui {

class SurfaceBase;  // fwd

//----------------------------------------------------------------

enum class WorkOrderType
{
    Unset,
    Event,
    Key,
    Surface
};

enum class SurfaceType
{
    Unset,
    Draw,
    PopSurface,
    PushSurface,
    SetSurface,
    ReleaseHeldEvents   // Dice have landed; show the roll results
};

//----------------------------------------------------------------

struct WorkOrderKey
{
    int key = ERR;
};

//----------------------------------------------------------------
    
using GameEvent = std::variant<
    Ctrl::UslBettingClosed,
    Ctrl::UslDiceThrowStart,
    Ctrl::UslDiceNewValue,
    Ctrl::UslResolveBetsEnd,
    Ctrl::UslPointEstablished,
    Ctrl::UslSevenOut,
    Ctrl::UslPassLineWinner,
    Ctrl::UslNewShooter,
    Ctrl::UslBettingOpened,
//  Ctrl::UslCountdownRollDice,      // not yet implemented
    
    Ctrl::UslBetMade,
    Ctrl::UslBetResolved,
    Ctrl::UslBetRemoved,
    Ctrl::UslBetChanged,
    
    Ctrl::UslTableBetsChanged,
    Ctrl::UslTableSessionStatsChanged,
    Ctrl::UslTableLastRollStatsChanged,
    Ctrl::UslTableBalanceChanged,

    Ctrl::UslPlayerBetsChanged,
    Ctrl::UslPlayerSessionStatsChanged,
    Ctrl::UslPlayerLastRollStatsChanged,
    Ctrl::UslPlayerBalanceChanged,

    Ctrl::UslPlayerJoinedTable,
    Ctrl::UslPlayerLeftTable,
    Ctrl::UslUserPlayerSwitched,
    Ctrl::UslTableSwitched,
    Ctrl::UslPlayerRenamed,
    Ctrl::UslRollHistoryFormatChanged,
    Ctrl::UslAutoOddsApplied,
    
    Ctrl::RspMakeBet,
    Ctrl::RspOddsBet,
    Ctrl::RspRemoveBet,
    Ctrl::RspUndoBet,
    Ctrl::RspApplyQuickBets,
    Ctrl::RspEditQuickBets,
    Ctrl::RspEditAutoOdds,
    Ctrl::RspSetBetFlags,
    Ctrl::RspSwitchPlayer,
    Ctrl::RspSwitchTable,
    Ctrl::RspManagePlayer
    >;
    
struct WorkOrderEvent
{
    GameEvent event;
};

//----------------------------------------------------------------
    
struct WorkOrderSurface
{
    SurfaceType type = SurfaceType::Unset;  // Must always be set
    SurfaceBase* pSurface = nullptr;
    // if SurfaceType == Draw then
    //     if pSurface ==  null then
    //        the active surface draw() is called
    //     else
    //        the supplied surface draw is called
};
    
//----------------------------------------------------------------

using WorkOrder = std::variant<
    WorkOrderKey,
    WorkOrderEvent,
    WorkOrderSurface
    >;

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
