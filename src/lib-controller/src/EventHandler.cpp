//----------------------------------------------------------------
//
// File: EventHandler.cpp
//
//----------------------------------------------------------------

#include <controller/EventHandler.h>
#include <controller/GameEvents.h>
#include <controller/GameMain.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/AutoFill.h>
#include <controller/QuickBet.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>
#include <cassert>
#include <utility>

using namespace Ctrl;

//----------------------------------------------------------------

EventHandler::EventHandler()
{
    Gen::EventManager::instance().subscribe<Ctrl::ReqMakeBet>(
        [this](const Ctrl::ReqMakeBet& ev)
        {
            this->onReqMakeBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqOddsBet>(
        [this](const Ctrl::ReqOddsBet& ev)
        {
            this->onReqOddsBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqRollDice>(
        [this](const Ctrl::ReqRollDice& ev)
        {
            this->onReqRollDice(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqAutoFill>(
        [this](const Ctrl::ReqAutoFill& ev)
        {
            this->onReqAutoFill(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqQuickBet>(
        [this](const Ctrl::ReqQuickBet& ev)
        {
            this->onReqQuickBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqTerminate>(
        [this](const Ctrl::ReqTerminate& ev)
        {
            this->onReqTerminate(ev);
        });
}

//----------------------------------------------------------------
//
//  Events from the View (lib-cui, lib-gui)
//
//----------------------------------------------------------------

void
EventHandler::onReqMakeBet(const Ctrl::ReqMakeBet& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqMakeBet()");

    Ctrl::RspMakeBet rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;

    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    if (pPlayer == nullptr)
    {
        assert(false && "pPlayer is null");
        LOG_TRACE("EventHandler::onReqMakeBet() pPlayer is null");
        rsp.error = std::move(ep);
    }

    LOG_TRACE("EventHandler::onReqMakeBet() making contract bet");
    auto result = pPlayer->makeBet(ev.betName,
                                   ev.contractAmount,
                                   ev.pivot);
    if (!result)
    {
        LOG_TRACE("EventHandler::onReqMakeBet() error back from makeBet()");
        rsp.error = std::move(result).error();
        LOG_TRACE(rsp.error.description());
        }
    else
    {
        LOG_TRACE("EventHandler::onReqMakeBet() make bet accepted");
        auto pBet = std::move(result).value();
        rsp.accepted = true;
        rsp.betId = pBet->betId();
    }

    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::onReqOddsBet(const Ctrl::ReqOddsBet& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqOddsBet()");

    Ctrl::RspOddsBet rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;
    rsp.betId         = ev.betId;

    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    assert(pPlayer);
    if (pPlayer == nullptr)
    {
        LOG_TRACE("EventHandler::onReqOddsBet() pPlayer is null");
        rsp.error = std::move(ep);
    }
    
    LOG_TRACE("EventHandler::onReqMakeBet() making odds bet");
    auto betPtr = pPlayer->getBet(ev.betId, ep);
    if (!betPtr)
    {
        rsp.error = std::move(ep);
    }
    else
    {
        auto result = pPlayer->setOddsAmount(betPtr, ev.oddsAmount);
        if (!result)
        {
            rsp.error = std::move(result).error();
        }
        else
        {
            rsp.accepted = true;
        }
    }
    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::onReqRollDice(const Ctrl::ReqRollDice& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqRollDice()");
    Gbl::pTable->rollDice();
    
    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqAutoFill(const Ctrl::ReqAutoFill& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqAutoFill()");
    
    AutoFill::AutoFillEntry afe
    {
        ev.betName,
        ev.pivot,
        ev.isOddsBet,
        ev.amount
    };
    AutoFill::instance().setAutoFill(afe);

    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqQuickBet(const Ctrl::ReqQuickBet& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqQuickBet()");

    QuickBet::QuickBetEntry qbe
    {
        ev.betName,
        ev.pivot,
        ev.isOddsBet,
        ev.amount
    };
    QuickBet::instance().setQuickBet(qbe);

    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqTerminate(const Ctrl::ReqTerminate& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqTerminate()");
    
    GameMain::instance()->terminateApp();

    // No response event
}

//----------------------------------------------------------------
//
//  Events from the Model (lib-craps) we're interested in handling
//  here in the Controller.
//
//----------------------------------------------------------------

//----------------------------------------------------------------

