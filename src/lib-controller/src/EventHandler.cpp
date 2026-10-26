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
#include <controller/LastFill.h>
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
    const std::string fname("EventHandler::onReqMakeBet(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspMakeBet rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;
    
    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    if (pPlayer == nullptr)
    {
        assert(false && "pPlayer is null");
        ep.prepend(fname);
        rsp.error = std::move(ep);
        LOG_ERROR(fname + " Error" + ep.fullDesc());
    }
    else
    {
        auto result = pPlayer->makeBet(ev.betName,
                                       ev.contractAmount,
                                       ev.pivot);
        if (!result)
        {
            rsp.error = std::move(result).error();
        }
        else
        {
            auto betPtr = std::move(result).value();
            rsp.accepted = true;
            rsp.betId = betPtr->betId();
            
            setLastFill(*betPtr, false); // Set/replace last fill
        }
    }

    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::onReqOddsBet(const Ctrl::ReqOddsBet& ev) const
{
    const std::string fname("EventHandler::onReqOddsBet(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspOddsBet rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;
    rsp.betId         = ev.betId;

    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    if (pPlayer == nullptr)
    {
        assert(false && "pPlayer is null");
        ep.prepend(fname);
        rsp.error = std::move(ep);
        LOG_ERROR(fname + " Error" + ep.fullDesc());
    }
    else
    {
        auto betPtr = pPlayer->getBet(ev.betId, ep);
        if (!betPtr)
        {
            ep.prepend(fname);
            LOG_ERROR(fname + " Error" + ep.fullDesc());
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
                setLastFill(*betPtr, true); // Set/replace last fill
            }
        }
    }
    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::setLastFill(const Craps::CrapsBet& bet, bool isOddsBet) const
{
    LastFill::LastFillEntry lfe;
    lfe.betName   = bet.betName();
    lfe.pivot     = bet.pivot();
    lfe.isOddsBet = isOddsBet;
    lfe.amount    = (isOddsBet ? bet.oddsAmount() : bet.contractAmount());
    LastFill::instance().setLastFill(lfe);
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

