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

Ctrl::CorrelationId
CuiSender::reqMakeBet(const Craps::PlayerId& playerId,
                      BetName betName,
                      Gen::Money contractAmount,
                      unsigned pivot)
{
    Ctrl::ReqMakeBet ev;
    ev.playerId       = playerId;
    ev.betName        = betName;
    ev.contractAmount = contractAmount;
    ev.pivot          = pivot;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);
    
    LOG_DEBUG("CuiSender::reqMakeBet() correlationId = " +
              std::to_string(correlationId));
    
    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqOddsBet(const Craps::PlayerId& playerId,
                      Craps::BetId betId,
                      Gen::Money oddsAmount)
{
    LOG_TRACE("CuiSender::reqOddsBet()");

    Ctrl::ReqOddsBet ev;
    ev.playerId       = playerId;
    ev.betId          = betId;
    ev.oddsAmount     = oddsAmount;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);
    
    LOG_DEBUG("CuiSender::reqOddsBet() correlationId = " +
              std::to_string(correlationId));
    
    return correlationId;
}

//----------------------------------------------------------------

void
CuiSender::reqTerminate()
{
    LOG_TRACE("CuiSender::reqTerminate()");

    Gen::EventManager::instance().publish(Ctrl::ReqTerminate{});
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
CuiSender::reqAutoFill(BetName    betName,
                       unsigned   pivot,
                       bool       isOddsBet,
                       Gen::Money amount)
{
    LOG_TRACE("CuiSender::reqAutoFill()");

    Ctrl::ReqAutoFill ev;           // No response to this
    ev.betName   = betName;         // So no setting correlationId
    ev.pivot     = pivot;
    ev.isOddsBet = isOddsBet;
    ev.amount    = amount;
    Gen::EventManager::instance().publish(ev);

LOG_TRACE("CuiSender::reqAutoFill()"
          " betName:" + EnumBetName::toString(betName)    +
          " pivot:"   + std::to_string(pivot)             +
          " isOdds:"  + (isOddsBet ? "true" : "false" ) +
          " amount:"  + std::to_string(amount));
}

//----------------------------------------------------------------

void
CuiSender::reqQuickBet(BetName    betName,
                       unsigned   pivot,
                       bool       isOddsBet,
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
