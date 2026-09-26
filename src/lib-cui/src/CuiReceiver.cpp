//----------------------------------------------------------------
//
// File: CuiReceiver.cpp
//
//----------------------------------------------------------------

#include <cui/CuiReceiver.h>
#include <cui/WorkOrder.h>
#include <cui/CuiThread.h>
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
    // TODO subscribe ...

    LOG_TRACE("CuiReceiver() subscribing to UslDiceThrowStart");
    Gen::EventManager::instance().subscribe<Ctrl::UslDiceThrowStart>(
        [this](const Ctrl::UslDiceThrowStart& ev)
        {
            this->onDiceThrowStart(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslDiceNewValue");
    Gen::EventManager::instance().subscribe<Ctrl::UslDiceNewValue>(
        [this](const Ctrl::UslDiceNewValue& ev)
        {
            this->onDiceNewValue(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslResolveBetsEnd");
    Gen::EventManager::instance().subscribe<Ctrl::UslResolveBetsEnd>(
        [this](const Ctrl::UslResolveBetsEnd& ev)
        {
            this->onResolveBetsEnd(ev);
        });
    
    LOG_TRACE("CuiReceiver() subscribing to UslPointEstablished");
    Gen::EventManager::instance().subscribe<Ctrl::UslPointEstablished>(
        [this](const Ctrl::UslPointEstablished& ev)
        {
            this->onPointEstablished(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslSevenOut");
    Gen::EventManager::instance().subscribe<Ctrl::UslSevenOut>(
        [this](const Ctrl::UslSevenOut& ev)
        {
            this->onSevenOut(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslPassLineWinner");
    Gen::EventManager::instance().subscribe<Ctrl::UslPassLineWinner>(
        [this](const Ctrl::UslPassLineWinner& ev)
        {
            this->onPassLineWinner(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslNewShooter");
    Gen::EventManager::instance().subscribe<Ctrl::UslNewShooter>(
        [this](const Ctrl::UslNewShooter& ev)
        {
            this->onNewShooter(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslTableBalanceChanged");
    Gen::EventManager::instance().subscribe<Ctrl::UslTableBalanceChanged>(
        [this](const Ctrl::UslTableBalanceChanged& ev)
        {
            this->onTableBalanceChanged(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslPlayerBalanceChanged");
    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerBalanceChanged>(
        [this](const Ctrl::UslPlayerBalanceChanged& ev)
        {
            this->onPlayerBalanceChanged(ev);
        });
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceThrowStart(const Ctrl::UslDiceThrowStart& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onDiceThrowStart()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceNewValue(const Ctrl::UslDiceNewValue& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onDiceNewValue()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);

    std::string s = "CuiReceiver::onDiceNewValue() Dice " +
                    std::to_string(ev.val) + " ("          +
                    std::to_string(ev.d1) + ","            +
                    std::to_string(ev.d2) + ") Roll: "     +
                    std::to_string(ev.rollCount);
    LOG_TRACE(s);
}

//----------------------------------------------------------------

void
CuiReceiver::onResolveBetsEnd(const Ctrl::UslResolveBetsEnd& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onResolveBetsEnd()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPointEstablished(const Ctrl::UslPointEstablished& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onPointEstablished()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onSevenOut(const Ctrl::UslSevenOut& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onSevenOut()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPassLineWinner(const Ctrl::UslPassLineWinner& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onPassLineWinner()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onNewShooter(const Ctrl::UslNewShooter& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onNewShooter()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableBalanceChanged(const Ctrl::UslTableBalanceChanged& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onTableBalanceChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerBalanceChanged(const Ctrl::UslPlayerBalanceChanged& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onPlayerBalanceChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

