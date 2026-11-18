//----------------------------------------------------------------
//
// File: WorkOrderDispatcher.cpp
//
//----------------------------------------------------------------

#include <cui/core/WorkOrderDispatcher.h>
#include <cui/managers/ColorManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/managers/OperationManager.h>
#include <cui/panels/WindowAnimation.h>
#include <cui/panels/WindowHouseBrief.h>
#include <cui/panels/WindowPlayerBrief.h>
#include <cui/panels/WindowPlayerArea.h>
#include <cui/panels/WindowRollHistory.h>
#include <cui/panels/WindowTitleBar.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

void
WorkOrderDispatcher::dispatch(const WorkOrder& wo)
{
    std::visit(
        [this](const auto& workOrder)
        {
            process(workOrder);
        },
        wo);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const WorkOrderKey& wo)
{
    SurfaceManager::instance().handleKey(wo.key);    
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const WorkOrderSurface& wo)
{
    switch (wo.type)
    {
    case SurfaceType::Draw:
        if (wo.pSurface)
        {
            SurfaceManager::instance().draw(wo.pSurface);
        }
        else
        {
            SurfaceManager::instance().draw();
        }
        break;

    case SurfaceType::SetSurface:
        SurfaceManager::instance().setSurface(wo.pSurface);
        break;

    case SurfaceType::PopSurface:
        SurfaceManager::instance().popSurface();
        break;

    case SurfaceType::PushSurface:
        SurfaceManager::instance().pushSurface(wo.pSurface);
        break;

    default:
        assert(false && "Unhandled SurfaceType");
    }
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const WorkOrderEvent& woe)
{
    std::visit(
        [this](const auto& event)
        {
            process(event);
        },
        woe.event);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBettingClosed& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBettingClosed)");
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslDiceThrowStart& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslDiceThrowStart)");

    WindowAnimation::instance().onDiceThrowStart();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslDiceNewValue& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslDiceNewValue)");
    
    WindowAnimation::instance().onDiceNewValue  (ev.d1, ev.d2, ev.rollCount);
    WindowRollHistory::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);
    WindowTitleBar::instance().onDiceNewValue   (ev.d1, ev.d2, ev.rollCount);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslResolveBetsEnd& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslResolveBetsEnd)");

    WindowHouseBrief::instance().onResolveBetsEnd();
    WindowPlayerBrief::instance().onResolveBetsEnd();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPointEstablished& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPointEstablished)");

    WindowTitleBar::instance().onPointEstablished   (ev.point);
    WindowRollHistory::instance().onPointEstablished(ev.point);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslSevenOut& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslSevenOut)");

    WindowTitleBar::instance().onSevenOut();
    WindowRollHistory::instance().onSevenOut();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPassLineWinner& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPassLineWinner)");

    WindowTitleBar::instance().onPassLineWinner();
    WindowRollHistory::instance().onPassLineWinner();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslNewShooter& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslNewShooter)");

    WindowPlayerBrief::instance().onNewShooter(ev.playerId);
    WindowAnimation::instance().onNewShooter  (ev.playerId);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBettingOpened& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBettingOpened)");
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBetMade& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBetMade)");

    WindowPlayerArea::instance().onBetMade(
        ev.playerId,
        ev.betId,
        ev.betName,
        ev.contractAmount,
        ev.oddsAmount,
        ev.pivot);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBetResolved& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBetResolved)");

    WindowPlayerArea::instance().onBetResolved(
        ev.playerId,
        ev.betName,
        ev.betId,
        ev.amountWin,
        ev.amountLose);
    // TODO    
    // WindowMessageArea::instance().onBetResolved(
    //     ev.playerId,
    //     ev.betName,
    //     ev.betId,
    //     ev.amountWin,
    //     ev.amountLose,
    //     ev.returnedToPlayer,
    //     ev.commission);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBetChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBetChanged)");

    WindowPlayerArea::instance().onBetChanged(
        ev.playerId,
        ev.betId,
        ev.betName,
        ev.contractAmount,
        ev.oddsAmount,
        ev.pivot);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslTableBetsChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslTableBetsChanged)");

    WindowHouseBrief::instance().onTableBetsChanged(
        ev.numBetsOnTable, ev.amountOnTable);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslTableSessionStatsChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslTableSessionStatsChanged)");

    WindowHouseBrief::instance().onTableSessionStatsChanged(
        ev.numBetsWon, ev.numBetsLost);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslTableLastRollStatsChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslTableLastRollStatsChanged)");

    WindowHouseBrief::instance().onTableLastRollStatsChanged(ev.stats);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslTableBalanceChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslTableBalanceChanged)");

    WindowHouseBrief::instance().onTableBalanceChanged(
        ev.balance, ev.netBalance);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerBetsChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerBetsChanged)");

    WindowPlayerBrief::instance().onPlayerBetsChanged(
        ev.playerId, ev.numBetsOnTable, ev.amountOnTable);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerSessionStatsChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerSessionStatsChanged)");

    WindowPlayerBrief::instance().onPlayerSessionStatsChanged(
        ev.playerId, ev.numBetsWon, ev.numBetsLost);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerLastRollStatsChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerLastRollStatsChanged)");

    WindowPlayerBrief::instance().onPlayerLastRollStatsChanged(
        ev.playerId, ev.stats);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerBalanceChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerBalanceChanged)");

    WindowPlayerBrief::instance().onPlayerBalanceChanged(
        ev.playerId, ev.balance, ev.netBalance);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslAutoFillChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslAutoFillChanged)");

    // TODO
    // WindowMessageArea::instance().onAutoFillChanged(
    //         Craps::PlayerId playerId;
    //         BetName         betName;
    //         unsigned        pivot;
    //         bool            forOdds;
    //         Gen::Money      amount;
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslQuickBetChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslQuickBetChanged)");

    // TODO
    // WindowMessageArea::instance().onQuickBetChanged(
    //         Craps::PlayerId playerId;
    //         BetName         betName;
    //         unsigned        pivot;
    //         bool            forOdds;
    //         Gen::Money      amount;
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspMakeBet& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspMakeBet)");
    OperationManager::instance().onRspMakeBet(ev);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspOddsBet& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspOddsBet)");
    OperationManager::instance().onRspOddsBet(ev);
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Who is at the table, who the user player is, or a player's name
// changed. Rather than patch each panel for each kind of change, bring
// everything that shows players up to date from the current facts,
// then redraw the whole screen composition.
//
void
WorkOrderDispatcher::refreshRoster()
{
    ColorManager::instance().syncPlayerColors();
    WindowPlayerArea::instance().onRosterChanged();
    WindowPlayerBrief::instance().reload();
    WindowAnimation::instance().onRosterChanged();
    SurfaceManager::instance().draw();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerJoinedTable& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerJoinedTable)");
    (void) ev;
    refreshRoster();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerLeftTable& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerLeftTable)");
    (void) ev;
    refreshRoster();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslUserPlayerSwitched& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslUserPlayerSwitched)");
    (void) ev;
    refreshRoster();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPlayerRenamed& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPlayerRenamed)");
    (void) ev;
    refreshRoster();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspSwitchPlayer& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspSwitchPlayer)");
    OperationManager::instance().onRspSwitchPlayer(ev);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspManagePlayer& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspManagePlayer)");
    OperationManager::instance().onRspManagePlayer(ev);
}

//----------------------------------------------------------------
