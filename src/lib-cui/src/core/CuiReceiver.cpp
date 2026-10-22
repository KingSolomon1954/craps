//----------------------------------------------------------------
//
// File: CuiReceiver.cpp
//
//----------------------------------------------------------------

#include <cui/core/CuiReceiver.h>
#include <cui/core/WorkOrder.h>
#include <cui/core/CuiThread.h>
#include <craps/CrapsTypes.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

CuiReceiver&
CuiReceiver::instance()
{
    static CuiReceiver cr;
    return cr;
}

//----------------------------------------------------------------

CuiReceiver::CuiReceiver()
{
    Gen::EventManager::instance().subscribe<Ctrl::RspMakeBet>(
        [this](const Ctrl::RspMakeBet& ev)
        {
            this->onMakeBetResponse(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBettingClosed>(
        [this](const Ctrl::UslBettingClosed& ev)
        {
            this->onBettingClosed(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslDiceThrowStart>(
        [this](const Ctrl::UslDiceThrowStart& ev)
        {
            this->onDiceThrowStart(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslDiceNewValue>(
        [this](const Ctrl::UslDiceNewValue& ev)
        {
            this->onDiceNewValue(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslResolveBetsEnd>(
        [this](const Ctrl::UslResolveBetsEnd& ev)
        {
            this->onResolveBetsEnd(ev);
        });
    
    Gen::EventManager::instance().subscribe<Ctrl::UslPointEstablished>(
        [this](const Ctrl::UslPointEstablished& ev)
        {
            this->onPointEstablished(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslSevenOut>(
        [this](const Ctrl::UslSevenOut& ev)
        {
            this->onSevenOut(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPassLineWinner>(
        [this](const Ctrl::UslPassLineWinner& ev)
        {
            this->onPassLineWinner(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslNewShooter>(
        [this](const Ctrl::UslNewShooter& ev)
        {
            this->onNewShooter(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBettingOpened>(
        [this](const Ctrl::UslBettingOpened& ev)
        {
            this->onBettingOpened(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBetMade>(
        [this](const Ctrl::UslBetMade& ev)
        {
            this->onBetMade(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBetResolved>(
        [this](const Ctrl::UslBetResolved& ev)
        {
            this->onBetResolved(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableBetsChanged>(
        [this](const Ctrl::UslTableBetsChanged& ev)
        {
            this->onTableBetsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableSessionStatsChanged>(
        [this](const Ctrl::UslTableSessionStatsChanged& ev)
        {
            this->onTableSessionStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableLastRollStatsChanged>(
        [this](const Ctrl::UslTableLastRollStatsChanged& ev)
        {
            this->onTableLastRollStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableBalanceChanged>(
        [this](const Ctrl::UslTableBalanceChanged& ev)
        {
            this->onTableBalanceChanged(ev);
        });
    
    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerBetsChanged>(
        [this](const Ctrl::UslPlayerBetsChanged& ev)
        {
            this->onPlayerBetsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerSessionStatsChanged>(
        [this](const Ctrl::UslPlayerSessionStatsChanged& ev)
        {
            this->onPlayerSessionStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerLastRollStatsChanged>(
        [this](const Ctrl::UslPlayerLastRollStatsChanged& ev)
        {
            this->onPlayerLastRollStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerBalanceChanged>(
        [this](const Ctrl::UslPlayerBalanceChanged& ev)
        {
            this->onPlayerBalanceChanged(ev);
        });
    
    Gen::EventManager::instance().subscribe<Ctrl::UslAutoFillChanged>(
        [this](const Ctrl::UslAutoFillChanged& ev)
        {
            this->onAutoFillChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslQuickBetChanged>(
        [this](const Ctrl::UslQuickBetChanged& ev)
        {
            this->onQuickBetChanged(ev);
        });
}

//----------------------------------------------------------------

void
CuiReceiver::onMakeBetResponse(const Ctrl::RspMakeBet& ev) const
{
    LOG_TRACE("CuiReceiver::onMakeBetResponse()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onBettingClosed(const Ctrl::UslBettingClosed& ev) const
{
    LOG_TRACE("CuiReceiver()::onBettingClosed()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceThrowStart(const Ctrl::UslDiceThrowStart& ev) const
{
    LOG_TRACE("CuiReceiver()::onDiceThrowStart()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceNewValue(const Ctrl::UslDiceNewValue& ev) const
{
    LOG_TRACE("CuiReceiver()::onDiceNewValue()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);

    std::string s = "CuiReceiver::onDiceNewValue() Dice " +
                    std::to_string(ev.val) + " ("         +
                    std::to_string(ev.d1)  + ","          +
                    std::to_string(ev.d2)  + ") Roll: "   +
                    std::to_string(ev.rollCount);
    LOG_TRACE(s);
}

//----------------------------------------------------------------

void
CuiReceiver::onResolveBetsEnd(const Ctrl::UslResolveBetsEnd& ev) const
{
    LOG_TRACE("CuiReceiver()::onResolveBetsEnd()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPointEstablished(const Ctrl::UslPointEstablished& ev) const
{
    LOG_TRACE("CuiReceiver()::onPointEstablished()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onSevenOut(const Ctrl::UslSevenOut& ev) const
{
    LOG_TRACE("CuiReceiver()::onSevenOut()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPassLineWinner(const Ctrl::UslPassLineWinner& ev) const
{
    LOG_TRACE("CuiReceiver()::onPassLineWinner()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onNewShooter(const Ctrl::UslNewShooter& ev) const
{
    LOG_TRACE("CuiReceiver()::onNewShooter()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onBettingOpened(const Ctrl::UslBettingOpened& ev) const
{
    LOG_TRACE("CuiReceiver()::onBettingOpened()");
    
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onBetMade(const Ctrl::UslBetMade& ev) const
{
    LOG_TRACE("CuiReceiver()::onBetMade()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onBetResolved(const Ctrl::UslBetResolved& ev) const
{
    LOG_TRACE("CuiReceiver()::onBetResolved()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableBetsChanged (const Ctrl::UslTableBetsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableBetsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableLastRollStatsChanged(const Ctrl::UslTableLastRollStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableLastRollStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}
   
//----------------------------------------------------------------

void
CuiReceiver::onTableSessionStatsChanged(const Ctrl::UslTableSessionStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableSessionStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableBalanceChanged(const Ctrl::UslTableBalanceChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableBalanceChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerBetsChanged (const Ctrl::UslPlayerBetsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerBetsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerLastRollStatsChanged(const Ctrl::UslPlayerLastRollStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerLastRollStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}
   
//----------------------------------------------------------------

void
CuiReceiver::onPlayerSessionStatsChanged(const Ctrl::UslPlayerSessionStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerSessionStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerBalanceChanged(const Ctrl::UslPlayerBalanceChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerBalanceChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onAutoFillChanged(const Ctrl::UslAutoFillChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onAutoFillChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onQuickBetChanged(const Ctrl::UslQuickBetChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onQuickBetChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

