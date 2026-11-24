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

void
CuiSender::reqSetRollHistoryFormat(Ctrl::RollHistoryFormat format)
{
    LOG_TRACE("CuiSender::reqSetRollHistoryFormat()");

    Ctrl::ReqSetRollHistoryFormat ev;
    ev.format = format;
    Gen::EventManager::instance().publish(ev);
}

//----------------------------------------------------------------

void
CuiSender::reqSetAnimationSpeed(Ctrl::AnimationSpeed speed)
{
    LOG_TRACE("CuiSender::reqSetAnimationSpeed()");

    Ctrl::ReqSetAnimationSpeed ev;
    ev.speed = speed;
    Gen::EventManager::instance().publish(ev);
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqRemoveBet(const Craps::PlayerId& playerId,
                        Craps::BetId betId,
                        bool undoJoinsPrevious)
{
    LOG_TRACE("CuiSender::reqRemoveBet()");

    Ctrl::ReqRemoveBet ev;
    ev.playerId = playerId;
    ev.betId    = betId;
    ev.undoJoinsPrevious = undoJoinsPrevious;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);

    LOG_DEBUG("CuiSender::reqRemoveBet() correlationId = " +
              std::to_string(correlationId));

    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqUndoBet(const Craps::PlayerId& playerId)
{
    LOG_TRACE("CuiSender::reqUndoBet()");

    Ctrl::ReqUndoBet ev;
    ev.playerId = playerId;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);

    LOG_DEBUG("CuiSender::reqUndoBet() correlationId = " +
              std::to_string(correlationId));

    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqSetBetFlags(const Craps::PlayerId& playerId,
                          Ctrl::BetFlags::Scope scope,
                          Craps::BetId betId,
                          Ctrl::BetFlags::Change change)
{
    LOG_TRACE("CuiSender::reqSetBetFlags()");

    Ctrl::ReqSetBetFlags ev;
    ev.playerId = playerId;
    ev.scope    = scope;
    ev.betId    = betId;
    ev.change   = change;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);

    LOG_DEBUG("CuiSender::reqSetBetFlags() correlationId = " +
              std::to_string(correlationId));

    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqApplyQuickBets(const Craps::PlayerId& playerId,
                             const std::string& name)
{
    LOG_TRACE("CuiSender::reqApplyQuickBets()");

    Ctrl::ReqApplyQuickBets ev;
    ev.playerId = playerId;
    ev.name     = name;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);

    LOG_DEBUG("CuiSender::reqApplyQuickBets() correlationId = " +
              std::to_string(correlationId));

    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqEditQuickBets(Ctrl::ReqEditQuickBets ev)
{
    LOG_TRACE("CuiSender::reqEditQuickBets()");

    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);

    LOG_DEBUG("CuiSender::reqEditQuickBets() correlationId = " +
              std::to_string(correlationId));

    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqEditAutoOdds(Ctrl::ReqEditAutoOdds ev)
{
    LOG_TRACE("CuiSender::reqEditAutoOdds()");

    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);

    LOG_DEBUG("CuiSender::reqEditAutoOdds() correlationId = " +
              std::to_string(correlationId));

    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqOddsBet(const Craps::PlayerId& playerId,
                      Craps::BetId betId,
                      Gen::Money oddsAmount,
                      bool undoJoinsPrevious)
{
    LOG_TRACE("CuiSender::reqOddsBet()");

    Ctrl::ReqOddsBet ev;
    ev.playerId       = playerId;
    ev.betId          = betId;
    ev.oddsAmount     = oddsAmount;
    ev.undoJoinsPrevious = undoJoinsPrevious;
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

Ctrl::CorrelationId
CuiSender::reqSwitchPlayer(const Craps::PlayerId& playerId)
{
    LOG_TRACE("CuiSender::reqSwitchPlayer()");

    Ctrl::ReqSwitchPlayer ev;
    ev.playerId = playerId;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);
    return correlationId;
}

//----------------------------------------------------------------

Ctrl::CorrelationId
CuiSender::reqManagePlayer(Ctrl::PlayerAction action,
                           const Craps::PlayerId& playerId,
                           const std::string& name,
                           const std::string& description)
{
    LOG_TRACE("CuiSender::reqManagePlayer()");

    Ctrl::ReqManagePlayer ev;
    ev.action   = action;
    ev.playerId = playerId;
    ev.name     = name;
    ev.description = description;
    const auto correlationId = ev.correlationId;
    Gen::EventManager::instance().publish(ev);
    return correlationId;
}

//----------------------------------------------------------------

void
CuiSender::reqRollDice()
{
    LOG_TRACE("CuiSender::reqRollDice()");
    
    Gen::EventManager::instance().publish(Ctrl::ReqRollDice{});
}

//----------------------------------------------------------------
