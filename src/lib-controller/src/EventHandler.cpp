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

    Gen::EventManager::instance().subscribe<Ctrl::ReqSwitchPlayer>(
        [this](const Ctrl::ReqSwitchPlayer& ev)
        {
            this->onReqSwitchPlayer(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqManagePlayer>(
        [this](const Ctrl::ReqManagePlayer& ev)
        {
            this->onReqManagePlayer(ev);
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
        LOG_ERROR(fname + " Error" + ep.fullDesc());
        rsp.error = std::move(ep);
    }
    else
    {
        // The same bet again changes the amount of the one already on the
        // table; it does not make a second bet. See Player::makeOrChangeBet().
        auto result = pPlayer->makeOrChangeBet(ev.betName,
                                               ev.contractAmount,
                                               ev.pivot);
        if (!result)
        {
            rsp.error = std::move(result).error();
        }
        else
        {
            auto placed = std::move(result).value();
            rsp.accepted       = true;
            rsp.betId          = placed.bet->betId();
            rsp.changed        = placed.changed;
            rsp.previousAmount = placed.previousAmount;

            setLastFill(ev.playerId, *placed.bet, false); // Set/replace last fill
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
                setLastFill(ev.playerId, *betPtr, true); // Set/replace last fill
            }
        }
    }
    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::setLastFill(const Craps::PlayerId& playerId,
                          const Craps::CrapsBet& bet, bool isOddsBet) const
{
    // Remembered per player, so a bet by one player never changes
    // what another player sees as their last fill.
    PlayerPrefs* prefs = Gbl::pPlayerMgr->getPrefs(playerId);
    if (prefs == nullptr)
    {
        return;
    }

    LastFill::LastFillEntry lfe;
    lfe.betName   = bet.betName();
    lfe.pivot     = bet.pivot();
    lfe.isOddsBet = isOddsBet;
    lfe.amount    = (isOddsBet ? bet.oddsAmount() : bet.contractAmount());
    prefs->lastFill().setLastFill(lfe);
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
    Gbl::pPlayerMgr->userPrefs().autoFill().setAutoFill(afe);

    Ctrl::UslAutoFillChanged changed;
    changed.playerId  = Gbl::pPlayerMgr->getUserPlayer()->getPlayerId();
    changed.betName   = ev.betName;
    changed.pivot     = ev.pivot;
    changed.isOddsBet = ev.isOddsBet;
    changed.amount    = ev.amount;
    Gen::EventManager::instance().publish(changed);

    LOG_INFO(std::string("AutoFill Configured") +
        "\n    playerId: " + changed.playerId                    +
        "\n     betName: " + EnumBetName::toString(ev.betName)    +
        "\n       pivot: " + std::to_string(ev.pivot)             +
        "\n   isOddsBet: " + (ev.isOddsBet ? "true" : "false")    +
        "\n      amount: " + std::to_string(ev.amount));

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
    Gbl::pPlayerMgr->userPrefs().quickBet().setQuickBet(qbe);

    Ctrl::UslQuickBetChanged changed;
    changed.playerId  = Gbl::pPlayerMgr->getUserPlayer()->getPlayerId();
    changed.betName   = ev.betName;
    changed.pivot     = ev.pivot;
    changed.isOddsBet = ev.isOddsBet;
    changed.amount    = ev.amount;
    Gen::EventManager::instance().publish(changed);

    LOG_INFO(std::string("QuickBet Configured") +
        "\n    playerId: " + changed.playerId                    +
        "\n     betName: " + EnumBetName::toString(ev.betName)    +
        "\n       pivot: " + std::to_string(ev.pivot)             +
        "\n   isOddsBet: " + (ev.isOddsBet ? "true" : "false")    +
        "\n      amount: " + std::to_string(ev.amount));

    // No response event
}

//----------------------------------------------------------------
//
// Makes the requested player the user player. Always answers with
// RspSwitchPlayer. Only when the switch happened, follows with
// UslUserPlayerSwitched.
//
void
EventHandler::onReqSwitchPlayer(const Ctrl::ReqSwitchPlayer& ev) const
{
    const std::string fname("EventHandler::onReqSwitchPlayer(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspSwitchPlayer rsp;
    rsp.correlationId = ev.correlationId;
    rsp.toPlayerId    = ev.playerId;
    if (Craps::Player* pUser = Gbl::pPlayerMgr->getUserPlayer())
    {
        rsp.fromPlayerId = pUser->getPlayerId();
    }

    Gen::ErrorPass ep;
    if (Gbl::pPlayerMgr->switchPlayer(ev.playerId, ep) == Gen::ReturnCode::Success)
    {
        rsp.switched = true;
    }
    else
    {
        ep.prepend(fname);
        LOG_ERROR(fname + " Error" + ep.fullDesc());
        rsp.error = std::move(ep);
    }
    Gen::EventManager::instance().publish(rsp);

    if (rsp.switched)
    {
        Ctrl::UslUserPlayerSwitched usl;
        usl.fromPlayerId = rsp.fromPlayerId;
        usl.toPlayerId   = rsp.toPlayerId;
        Gen::EventManager::instance().publish(usl);
    }
}

//----------------------------------------------------------------
//
// Renames, creates or deletes a player, or has a buddy join or leave
// the table. Always answers with RspManagePlayer. After a rename that
// happened, follows with UslPlayerRenamed.
//
void
EventHandler::onReqManagePlayer(const Ctrl::ReqManagePlayer& ev) const
{
    const std::string fname("EventHandler::onReqManagePlayer(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspManagePlayer rsp;
    rsp.correlationId = ev.correlationId;
    rsp.action        = ev.action;
    rsp.playerId      = ev.playerId;
    rsp.name          = ev.name;

    Gen::ErrorPass ep;
    Gen::ReturnCode rc = Gen::ReturnCode::Fail;
    switch (ev.action)
    {
    case Ctrl::PlayerAction::Rename:
        rc = Gbl::pPlayerMgr->renamePlayer(ev.playerId, ev.name, ep);
        break;
    case Ctrl::PlayerAction::Create:
        rc = Gbl::pPlayerMgr->createPlayer(ev.name, ev.description, rsp.playerId, ep);
        break;
    case Ctrl::PlayerAction::Delete:
        rc = Gbl::pPlayerMgr->deletePlayer(ev.playerId, ep);
        break;
    case Ctrl::PlayerAction::BuddyJoins:
        rc = Gbl::pPlayerMgr->buddyJoinsTable(ev.playerId, ep);
        break;
    case Ctrl::PlayerAction::BuddyLeaves:
        rc = Gbl::pPlayerMgr->buddyLeavesTable(ev.playerId, ep);
        break;
    }

    if (rc == Gen::ReturnCode::Success)
    {
        rsp.ok = true;
        // The name actually used was trimmed.
        for (const auto& info : Gbl::pPlayerMgr->listPlayers())
        {
            if (info.playerId == rsp.playerId)
            {
                rsp.name = info.name;
                break;
            }
        }
    }
    else
    {
        ep.prepend(fname);
        rsp.error = std::move(ep);
    }
    Gen::EventManager::instance().publish(rsp);

    if (rsp.ok && ev.action == Ctrl::PlayerAction::Rename)
    {
        Ctrl::UslPlayerRenamed usl;
        usl.playerId = rsp.playerId;
        usl.newName  = rsp.name;
        Gen::EventManager::instance().publish(usl);
    }
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

