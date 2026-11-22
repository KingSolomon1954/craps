//----------------------------------------------------------------
//
// File: EventHandler.cpp
//
//----------------------------------------------------------------

#include <controller/BetExecutor.h>
#include <controller/ConfigManager.h>
#include <controller/EventHandler.h>
#include <controller/GameEvents.h>
#include <controller/GameMain.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/LastFill.h>
#include <controller/UndoBet.h>
#include <controller/UndoManager.h>
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

    Gen::EventManager::instance().subscribe<Ctrl::ReqRemoveBet>(
        [this](const Ctrl::ReqRemoveBet& ev)
        {
            this->onReqRemoveBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqApplyQuickBets>(
        [this](const Ctrl::ReqApplyQuickBets& ev)
        {
            this->onReqApplyQuickBets(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqEditQuickBets>(
        [this](const Ctrl::ReqEditQuickBets& ev)
        {
            this->onReqEditQuickBets(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqUndoBet>(
        [this](const Ctrl::ReqUndoBet& ev)
        {
            this->onReqUndoBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqSetRollHistoryFormat>(
        [this](const Ctrl::ReqSetRollHistoryFormat& ev)
        {
            this->onReqSetRollHistoryFormat(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqSetAnimationSpeed>(
        [this](const Ctrl::ReqSetAnimationSpeed& ev)
        {
            this->onReqSetAnimationSpeed(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqRollDice>(
        [this](const Ctrl::ReqRollDice& ev)
        {
            this->onReqRollDice(ev);
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
        // For undo, remember how an existing bet was before it changes.
        std::unique_ptr<UndoBet> undoChange;
        if (auto existing = pPlayer->findBet(ev.betName, ev.pivot))
        {
            undoChange = std::make_unique<UndoBetModifiedAmount>(existing);
        }

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

            if (!placed.changed)
            {
                UndoManager::instance().push(
                    ev.playerId, std::make_unique<UndoBetAdded>(placed.bet));
            }
            else if (undoChange && placed.previousAmount != ev.contractAmount)
            {
                UndoManager::instance().push(ev.playerId, std::move(undoChange));
            }
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
            std::unique_ptr<UndoBet> undoChange;
            if (betPtr->oddsAmount() != ev.oddsAmount)
            {
                undoChange = std::make_unique<UndoBetModifiedAmount>(betPtr);
            }

            auto result = pPlayer->setOddsAmount(betPtr, ev.oddsAmount);
            if (!result)
            {
                rsp.error = std::move(result).error();
            }
            else
            {
                rsp.accepted = true;
                if (undoChange)
                {
                    UndoManager::instance().push(ev.playerId,
                        std::move(undoChange), ev.undoJoinsPrevious);
                }
                if (ev.oddsAmount > 0)   // Zero takes odds down; not a fill
                {
                    setLastFill(ev.playerId, *betPtr, true);
                }
            }
        }
    }
    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::onReqSetRollHistoryFormat(const Ctrl::ReqSetRollHistoryFormat& ev) const
{
    LOG_TRACE("Entered EventHandler::onReqSetRollHistoryFormat()");
    Gbl::pConfigMgr->setRollHistoryFormat(ev.format);

    Ctrl::UslRollHistoryFormatChanged changed;
    changed.format = ev.format;
    Gen::EventManager::instance().publish(changed);
}

//----------------------------------------------------------------

void
EventHandler::onReqSetAnimationSpeed(const Ctrl::ReqSetAnimationSpeed& ev) const
{
    LOG_TRACE("Entered EventHandler::onReqSetAnimationSpeed()");
    Gbl::pConfigMgr->setAnimationSpeed(ev.speed);
    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqRemoveBet(const Ctrl::ReqRemoveBet& ev) const
{
    const std::string fname("EventHandler::onReqRemoveBet(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspRemoveBet rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;
    rsp.betId         = ev.betId;

    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    if (pPlayer == nullptr)
    {
        ep.prepend(fname);
        LOG_ERROR(fname + " Error" + ep.fullDesc());
        rsp.error = std::move(ep);
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
            // Remember what it was; the bet is gone after removal.
            rsp.betName  = betPtr->betName();
            rsp.pivot    = betPtr->pivot();
            rsp.returned = betPtr->contractAmount() + betPtr->oddsAmount();

            auto result = pPlayer->removeBet(ev.betId);
            if (!result)
            {
                rsp.error = std::move(result).error();
            }
            else
            {
                rsp.accepted = true;
                UndoManager::instance().push(ev.playerId,
                    std::make_unique<UndoBetRemoved>(betPtr),
                    ev.undoJoinsPrevious);
            }
        }
    }
    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------
//
// Takes back the player's latest bet change since the last roll.
// Always answers with RspUndoBet. The screens follow from the bet and
// balance events the undo causes, as for any bet change.
//
void
EventHandler::onReqUndoBet(const Ctrl::ReqUndoBet& ev) const
{
    const std::string fname("EventHandler::onReqUndoBet(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspUndoBet rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;

    if (Gbl::pTable != nullptr && !Gbl::pTable->isBettingOpen())
    {
        // Keep the step; the player can undo once betting reopens
        Gen::ErrorPass ep;
        EP_SET(ep, Gen::ErrorPass::ErrorName::NoPermissionError,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate,
               "Betting is closed while the dice are rolling. "
               "Try again when betting reopens.");
        ep.setErrorName("Betting Closed");
        rsp.error = std::move(ep);
    }
    else
    {
        Gen::ErrorPass ep;
        if (UndoManager::instance().undo(ev.playerId, rsp.description, ep) ==
            Gen::ReturnCode::Success)
        {
            rsp.accepted = true;
            LOG_INFO(fname + rsp.description);
        }
        else
        {
            rsp.error = std::move(ep);
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

//----------------------------------------------------------------
//
// Applies one of the player's QuickBets entries. Always answers with
// RspApplyQuickBets. Applying is best effort: bets that cannot be placed
// are skipped and reported, not treated as a failure of the request.
//
void
EventHandler::onReqApplyQuickBets(const Ctrl::ReqApplyQuickBets& ev) const
{
    const std::string fname("EventHandler::onReqApplyQuickBets(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspApplyQuickBets rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;
    rsp.name          = ev.name;

    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    PlayerPrefs*   prefs   = Gbl::pPlayerMgr->getPrefs(ev.playerId);
    const BetPlan* plan    = (prefs != nullptr) ? prefs->quickBets().find(ev.name)
                                                : nullptr;

    if (pPlayer == nullptr || prefs == nullptr)
    {
        ep.prepend(fname);
        LOG_ERROR(fname + " Error" + ep.fullDesc());
        rsp.error = std::move(ep);
    }
    else if (Gbl::pTable != nullptr && !Gbl::pTable->isBettingOpen())
    {
        EP_SET(ep, Gen::ErrorPass::ErrorName::NoPermissionError,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate,
               "Betting is closed while the dice are rolling. "
               "Try again when betting reopens.");
        ep.setErrorName("Betting Closed");
        rsp.error = std::move(ep);
    }
    else if (plan == nullptr)
    {
        EP_SET(ep, Gen::ErrorPass::ErrorName::NotFoundError,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate,
               "There is no QuickBets entry named \"" + ev.name + "\".");
        ep.setErrorName("No Such QuickBets Entry");
        rsp.error = std::move(ep);
    }
    else
    {
        const auto result = BetExecutor::apply(*pPlayer, plan->specs,
                                               &prefs->lastFill());

        rsp.accepted       = true;
        rsp.placed         = static_cast<unsigned>(result.count(BetExecutor::Status::Placed));
        rsp.changed        = static_cast<unsigned>(result.count(BetExecutor::Status::Changed));
        rsp.alreadyInPlace = static_cast<unsigned>(result.count(BetExecutor::Status::AlreadyInPlace));
        rsp.skipped        = static_cast<unsigned>(result.count(BetExecutor::Status::Skipped));
        rsp.headline       = result.headline();
        rsp.skippedDetail  = result.skippedDetail();

        LOG_INFO("QuickBets \"" + ev.name + "\" applied for " + ev.playerId +
                 ": " + rsp.headline +
                 (rsp.skippedDetail.empty() ? "" : "\n" + rsp.skippedDetail));
    }

    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Changes the player's QuickBets: saves a bet, renames, changes an
// amount, removes a bet or deletes an entry. Always answers with
// RspEditQuickBets. Nothing here touches the table, so it works whether
// or not betting is open.
//
void
EventHandler::onReqEditQuickBets(const Ctrl::ReqEditQuickBets& ev) const
{
    const std::string fname("EventHandler::onReqEditQuickBets(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspEditQuickBets rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;
    rsp.name          = ev.name;

    PlayerPrefs* prefs = Gbl::pPlayerMgr->getPrefs(ev.playerId);
    std::string why;
    if (prefs == nullptr)
    {
        why = "That player could not be found.";
    }
    else
    {
        QuickBets& qb = prefs->quickBets();
        using Action = Ctrl::ReqEditQuickBets::Action;
        const std::string quoted = "\"" + ev.name + "\"";
        BetSpec changed;
        bool replaced = false;

        switch (ev.action)
        {
        case Action::Create:
            why = qb.create(ev.name, ev.spec);
            if (why.empty())
                rsp.message = "Saved " + describe(ev.spec) +
                              " as the new QuickBets entry " + quoted + ".";
            break;

        case Action::AddBet:
            why = qb.addBet(ev.name, ev.spec, replaced);
            if (why.empty())
                rsp.message = (replaced ? "Changed to " : "Added ") +
                              describe(ev.spec) +
                              (replaced ? " in " : " to ") + quoted + ".";
            break;

        case Action::Rename:
            why = qb.rename(ev.name, ev.newName);
            if (why.empty())
            {
                rsp.name    = ev.newName;
                rsp.message = "Renamed " + quoted + " to \"" + ev.newName + "\".";
            }
            break;

        case Action::SetAmount:
            why = qb.setAmount(ev.name, ev.index, ev.amount, changed);
            if (why.empty())
                rsp.message = "Changed " + describe(changed) + " in " + quoted + ".";
            break;

        case Action::RemoveBet:
            why = qb.removeBet(ev.name, ev.index, changed);
            if (why.empty())
                rsp.message = "Removed " + describe(changed) + " from " + quoted + ".";
            break;

        case Action::Delete:
            why = qb.removeEntry(ev.name);
            if (why.empty())
                rsp.message = "Deleted the QuickBets entry " + quoted + ".";
            break;
        }
    }

    if (why.empty())
    {
        rsp.accepted = true;
        LOG_INFO("QuickBets edited for " + ev.playerId + ": " + rsp.message);
    }
    else
    {
        Gen::ErrorPass ep;
        EP_SET(ep, Gen::ErrorPass::ErrorName::InvalidArgumentError,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate, why);
        ep.setErrorName("Unable to Save QuickBets");
        rsp.error = std::move(ep);
    }

    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

void
EventHandler::onReqRollDice(const Ctrl::ReqRollDice& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqRollDice()");
    UndoManager::instance().clear();        // Bets are about to be decided
    Gbl::pTable->rollDice();
    
    // No response event
}

//----------------------------------------------------------------

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

