//----------------------------------------------------------------
//
// File: CuiSender.cpp
//
//----------------------------------------------------------------

#include <cui/CuiSender.h>
#include <controller/GameEvents.h>
#include <craps/CrapsTypes.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

CuiSender&
CuiSender::instance()
{
    static CuiSender cs;
    return cs;
}

//----------------------------------------------------------------

void
CuiSender::reqRollDice()
{
    LOG_TRACE("CuiSender::reqRollDice()");
    
    Gen::EventManager::instance().publish(Ctrl::ReqRollDice{});
}

//----------------------------------------------------------------
