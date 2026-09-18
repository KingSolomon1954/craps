//----------------------------------------------------------------
//
// File: CuiReceiver.cpp
//
//----------------------------------------------------------------

#include <cui/CuiReceiver.h>
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
    
    LOG_TRACE("CuiReceiver() subscribing to UslDiceRollValue");
    Gen::EventManager::instance().subscribe<Ctrl::UslDiceRollValue>(
        [this](const Ctrl::UslDiceRollValue& ev)
        {
            this->onDiceRollValue(ev);
        });
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceRollValue(const Ctrl::UslDiceRollValue& ev) const
{
    LOG_TRACE("Entered CuiReceiver()::onDiceRollValue()");
    std::string s = "CuiReceiver::onDiceRollValue() Dice " +
                    std::to_string(ev.val) + " ("          +
                    std::to_string(ev.d1) + ","            +
                    std::to_string(ev.d2) + ") Roll: "     +
                    std::to_string(ev.rollCount);
    LOG_TRACE(s);

    // TODO
    // std::cout << playerName_ << " acknowledges roll: " << ev.val
    //           << "(" << ev.d1 << "," << ev.d2 << ")\n";
}

//----------------------------------------------------------------
