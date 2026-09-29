//----------------------------------------------------------------
//
// File: WorkOrderDispatcher.cpp
//
//----------------------------------------------------------------

#include <cui/WorkOrderDispatcher.h>
#include <cui/SurfaceManager.h>
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
        assert(false);
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
    static int pin = 0;
    LOG_TRACE("WorkOrderDispatcher::process(UslBettingClosed) evId = " + std::to_string(ev.id) + " pin " + std::to_string(pin++));
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslDiceThrowStart& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslDiceThrowStart) evId = " + std::to_string(ev.id));
    WindowAnimation::instance().onDiceThrowStart();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslDiceNewValue& ev)
{
    static int pin = 0;
    LOG_TRACE("WorkOrderDispatcher::process(UslDiceNewValue) evId = " + std::to_string(ev.id) + " pin " + std::to_string(pin++));
    WindowAnimation::instance().onDiceNewValue  (ev.d1, ev.d2, ev.rollCount);
    WindowRollHistory::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);
    WindowTitleBar::instance().onDiceNewValue(ev.d1, ev.d2, ev.rollCount);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslResolveBetsEnd& ev)
{
    static int pin = 0;
    LOG_TRACE("WorkOrderDispatcher::process(UslResolveBetsEnd) evId = " + std::to_string(ev.id) + " pin " + std::to_string(pin++));
    WindowHouseBrief::instance().onResolveBetsEnd();
    WindowPlayerBrief::instance().onResolveBetsEnd();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPointEstablished& ev)
{
    static int pin = 0;
    LOG_TRACE("WorkOrderDispatcher::process(UslPointEstablished) evId = " + std::to_string(ev.id)  + " pin " + std::to_string(pin++));
    WindowTitleBar::instance().onPointEstablished(ev.point);
    WindowRollHistory::instance().onPointEstablished(ev.point);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslSevenOut& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslSevenOut) evId = " + std::to_string(ev.id));
    WindowTitleBar::instance().onSevenOut();
    WindowRollHistory::instance().onSevenOut();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslPassLineWinner& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslPassLineWinner) evId = " + std::to_string(ev.id));
    WindowTitleBar::instance().onPassLineWinner();
    WindowRollHistory::instance().onPassLineWinner();
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslNewShooter& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslNewShooter) evId = " + std::to_string(ev.id));
    WindowPlayerBrief::instance().onNewShooter(ev.playerId);
    WindowAnimation::instance().onNewShooter(ev.playerId);
}

//----------------------------------------------------------------

void
WorkOrderDispatcher::process(const Ctrl::UslBettingOpened& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslBettingOpened) evId = " + std::to_string(ev.id));
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
WorkOrderDispatcher::process(const Ctrl::UslTableBalanceChanged& ev)
{
    LOG_TRACE("WorkOrderDispatcher::process(UslTableBalanceChanged)");
    WindowHouseBrief::instance().onTableBalanceChanged(
        ev.balance, ev.netBalance);
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

