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

    LOG_TRACE("CuiReceiver() subscribing to UslTableResults");
    Gen::EventManager::instance().subscribe<Ctrl::UslTableResults>(
        [this](const Ctrl::UslTableResults& ev)
        {
            this->onTableResults(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslTableNumBetsOnTableChanged");
    Gen::EventManager::instance().subscribe<Ctrl::UslTableNumBetsOnTableChanged>(
        [this](const Ctrl::UslTableNumBetsOnTableChanged& ev)
        {
            this->onTableNumBetsOnTableChanged(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslPlayerResults");
    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerResults>(
        [this](const Ctrl::UslPlayerResults& ev)
        {
            this->onPlayerResults(ev);
        });

    LOG_TRACE("CuiReceiver() subscribing to UslPlayerNumBetsOnTableChanged");
    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerNumBetsOnTableChanged>(
        [this](const Ctrl::UslPlayerNumBetsOnTableChanged& ev)
        {
            this->onPlayerNumBetsOnTableChanged(ev);
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
CuiReceiver::onTableResults(const Ctrl::UslTableResults& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onTableResults()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableNumBetsOnTableChanged(const Ctrl::UslTableNumBetsOnTableChanged& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onTableNumBetsOnTableChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerResults(const Ctrl::UslPlayerResults& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onPlayerResults()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerNumBetsOnTableChanged(const Ctrl::UslPlayerNumBetsOnTableChanged& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onPlayerNumBetsOnTableChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

