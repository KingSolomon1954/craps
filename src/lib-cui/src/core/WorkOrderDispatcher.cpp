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
#include <string>
#include <type_traits>

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

    case SurfaceType::ReleaseHeldEvents:
        releaseHeldEvents();
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

// Events that are never held back while the dice are rolling
bool
WorkOrderDispatcher::passesThrough(const WorkOrderEvent& woe)
{
    return std::visit(
        [](const auto& event) -> bool
        {
            using T = std::decay_t<decltype(event)>;
            return std::is_same_v<T, Ctrl::UslDiceThrowStart> ||
                   std::is_same_v<T, Ctrl::UslDiceNewValue>   || // splits itself
                   std::is_same_v<T, Ctrl::UslBettingClosed>  ||
                   std::is_same_v<T, Ctrl::RspMakeBet>        ||
                   std::is_same_v<T, Ctrl::RspOddsBet>        ||
                   std::is_same_v<T, Ctrl::RspRemoveBet>      ||
                   std::is_same_v<T, Ctrl::RspUndoBet>        ||
                   std::is_same_v<T, Ctrl::RspApplyQuickBets> ||
                   std::is_same_v<T, Ctrl::RspSwitchPlayer>   ||
                   std::is_same_v<T, Ctrl::RspManagePlayer>;
        },
        woe.event);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const WorkOrderEvent& woe)
{
    auto run = [this, woe]
    {
        std::visit(
            [this](const auto& event)
            {
                process(event);
            },
            woe.event);
    };

    if (holding_ && !passesThrough(woe))
    {
        held_.push_back(std::move(run));
        return;
    }
    run();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::holdOrRun(std::function<void()> fn)
{
    if (holding_) held_.push_back(std::move(fn));
    else          fn();
}

//----------------------------------------------------------------
//
// The dice have landed (or we gave up waiting). Show everything that
// was held, in the order it happened.
//
void
WorkOrderDispatcher::releaseHeldEvents()
{
    if (!holding_ && held_.empty()) return;

    holding_ = false;
    auto queued = std::move(held_);
    held_.clear();

    LOG_DEBUG("WorkOrderDispatcher::releaseHeldEvents() releasing " +
              std::to_string(queued.size()) + " held events");

    for (auto& fn : queued)
    {
        fn();
    }
    SurfaceManager::instance().draw();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::tick()
{
    if (holding_ &&
        std::chrono::steady_clock::now() - holdStart_ > MaxHold)
    {
        LOG_WARN("WorkOrderDispatcher::tick(): animation did not finish; "
                 "releasing held events");
        releaseHeldEvents();
    }
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

    // If the dice are animated, hold the results until they land.
    if (WindowAnimation::instance().onDiceThrowStart())
    {
        holding_   = true;
        holdStart_ = std::chrono::steady_clock::now();
    }
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslDiceNewValue& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslDiceNewValue)");
    
    // The animation needs the dice right away; everyone else waits
    WindowAnimation::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);

    holdOrRun([ev]
    {
        WindowRollHistory::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);
        WindowTitleBar::instance().onDiceNewValue   (ev.d1, ev.d2, ev.rollCount);
    });
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslResolveBetsEnd& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslResolveBetsEnd)");

    WindowHouseBrief::instance().onResolveBetsEnd();
    WindowPlayerBrief::instance().onResolveBetsEnd();
    WindowPlayerArea::instance().onResolveBetsEnd();
    SurfaceManager::instance().draw();
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
    SurfaceManager::instance().draw();
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
    SurfaceManager::instance().draw();
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
WorkOrderDispatcher::process(const Ctrl::UslRollHistoryFormatChanged& ev)
{
    (void) ev;      // WindowRollHistory reads the setting as it draws
    LOG_TRACE("WorkOrderDispatcher::process(UslRollHistoryFormatChanged)");
    SurfaceManager::instance().draw();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBetRemoved& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBetRemoved)");

    WindowPlayerArea::instance().onBetRemoved(ev.playerId, ev.betId);
    SurfaceManager::instance().draw();
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
    SurfaceManager::instance().draw();
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
WorkOrderDispatcher::process(const Ctrl::RspMakeBet& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspMakeBet)");
    OperationManager::instance().onRspMakeBet(ev);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspUndoBet& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspUndoBet)");
    OperationManager::instance().onRspUndoBet(ev);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspApplyQuickBets& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspApplyQuickBets)");
    OperationManager::instance().onRspApplyQuickBets(ev);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::RspRemoveBet& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(RspRemoveBet)");
    OperationManager::instance().onRspRemoveBet(ev);
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
