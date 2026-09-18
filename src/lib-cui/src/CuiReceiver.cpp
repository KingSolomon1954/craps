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
    
    LOG_TRACE("CuiReceiver() subscribing to UslDiceNewValue");
    Gen::EventManager::instance().subscribe<Ctrl::UslDiceNewValue>(
        [this](const Ctrl::UslDiceNewValue& ev)
        {
            this->onDiceNewValue(ev);
        });
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceNewValue(const Ctrl::UslDiceNewValue& ev) const
{
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
    
    LOG_TRACE("Entered CuiReceiver()::onDiceNewValue()");
    std::string s = "CuiReceiver::onDiceNewValue() Dice " +
                    std::to_string(ev.val) + " ("          +
                    std::to_string(ev.d1) + ","            +
                    std::to_string(ev.d2) + ") Roll: "     +
                    std::to_string(ev.rollCount);
    LOG_TRACE(s);
}

//----------------------------------------------------------------
