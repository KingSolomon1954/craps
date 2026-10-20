//----------------------------------------------------------------
//
// File: CuiSender.cpp
//
//----------------------------------------------------------------

#include <cui/core/CuiSender.h>
#include <controller/GameEvents.h>
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

void
CuiSender::reqAutoFill(BetName betName,
                       unsigned pivot,
                       bool isOddsBet,
                       Gen::Money amount)
{
    LOG_TRACE("CuiSender::reqAutoFill()");

    Ctrl::ReqAutoFill ev;           // No response to this
    ev.betName   = betName;         // So no setting correlationId
    ev.pivot     = pivot;
    ev.isOddsBet = isOddsBet;
    ev.amount    = amount;
    Gen::EventManager::instance().publish(ev);
}

//----------------------------------------------------------------

void
CuiSender::reqQuickBet(BetName betName,
                       unsigned pivot,
                       bool isOddsBet,
                       Gen::Money amount)
{
    LOG_TRACE("CuiSender::reqQuickBet()");

    Ctrl::ReqQuickBet ev;           // No response to this
    ev.betName   = betName;         // So no setting correlationId
    ev.pivot     = pivot;
    ev.isOddsBet = isOddsBet;
    ev.amount    = amount;
    Gen::EventManager::instance().publish(ev);
}

//----------------------------------------------------------------
