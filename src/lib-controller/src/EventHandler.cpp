//----------------------------------------------------------------
//
// File: EventHandler.cpp
//
//----------------------------------------------------------------

#include <controller/AutoOdds.h>
#include <controller/BetExecutor.h>
#include <controller/ConfigManager.h>
#include <controller/EventHandler.h>
#include <controller/GameEvents.h>
#include <controller/GameMain.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
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

    Gen::EventManager::instance().subscribe<Ctrl::ReqEditAutoOdds>(
        [this](const Ctrl::ReqEditAutoOdds& ev)
        {
            this->onReqEditAutoOdds(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBettingOpened>(
        [this](const Ctrl::UslBettingOpened& ev)
        {
            this->onUslBettingOpened(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqSetBetFlags>(
        [this](const Ctrl::ReqSetBetFlags& ev)
        {
            this->onReqSetBetFlags(ev);
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

    Gen::EventManager::instance().subscribe<Ctrl::ReqSwitchTable>(
        [this](const Ctrl::ReqSwitchTable& ev)
        {
            this->onReqSwitchTable(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqSetBuddyFollow>(
        [this](const Ctrl::ReqSetBuddyFollow& ev)
        {
            this->onReqSetBuddyFollow(ev);
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
// Changes the player's AutoOdds settings. Always answers with
// RspEditAutoOdds. Nothing here touches the table, so it works whether
// or not betting is open.
//
void
EventHandler::onReqEditAutoOdds(const Ctrl::ReqEditAutoOdds& ev) const
{
    const std::string fname("EventHandler::onReqEditAutoOdds(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspEditAutoOdds rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;

    PlayerPrefs* prefs = Gbl::pPlayerMgr->getPrefs(ev.playerId);
    std::string why;
    if (prefs == nullptr)
    {
        why = "That player could not be found.";
    }
    else
    {
        AutoOdds& ao = prefs->autoOdds();
        using Action = Ctrl::ReqEditAutoOdds::Action;
        const std::string type = EnumBetName::toString(ev.betName);

        switch (ev.action)
        {
        case Action::SetEnabled:
            ao.setEnabled(ev.enabled);
            rsp.message = ev.enabled ? "AutoOdds is on." : "AutoOdds is off.";
            break;

        case Action::Set:
        {
            AutoOdds::Setting st;
            st.multiple = ev.multiple;
            st.amount   = ev.amount;
            why = ao.set(ev.betName, ev.point, st);
            if (why.empty())
            {
                const std::string where = (ev.point == 0)
                    ? type + ", all points" : type + " " + std::to_string(ev.point);
                rsp.message = "AutoOdds for " + where + ": " +
                              (st.isSet() ? "set to " + st.toString() : "unset");
                if (st.isSet() && !ao.enabled())
                    rsp.message += "\nAutoOdds is off; turn it on in Configure.";
            }
            break;
        }

        case Action::Preset345:
            ao.setPreset345();
            rsp.message = "AutoOdds set to 3-4-5x for all bet types.";
            break;

        case Action::Clear:
            ao.clearAll();
            rsp.message = "AutoOdds cleared.";
            break;

        }
    }

    if (why.empty())
    {
        rsp.accepted = true;
        LOG_INFO("AutoOdds edited for " + ev.playerId + ": " + rsp.message);
    }
    else
    {
        Gen::ErrorPass ep;
        EP_SET(ep, Gen::ErrorPass::ErrorName::InvalidArgumentError,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate, why);
        ep.setErrorName("Unable to Save AutoOdds");
        rsp.error = std::move(ep);
    }

    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------
//
// Betting has reopened, so the point and Come bets are settled. If the
// user player has AutoOdds on, put odds on the bets that have a point and
// no odds yet. Bets are tried once; see AutoOdds::plan(). One undo group.
//
void
EventHandler::onUslBettingOpened(const Ctrl::UslBettingOpened& ev) const
{
    (void)ev;
    const std::string fname("EventHandler::onUslBettingOpened(): ");
    LOG_TRACE("Entered " + fname);

    if (Gbl::pTable == nullptr || !Gbl::pTable->isBettingOpen())
    {
        return;
    }

    Gen::ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getUserPlayer();
    if (pPlayer == nullptr)
    {
        return;
    }
    const Craps::PlayerId playerId = pPlayer->getPlayerId();
    PlayerPrefs* prefs = Gbl::pPlayerMgr->getPrefs(playerId);
    if (prefs == nullptr || !prefs->autoOdds().enabled())
    {
        return;
    }

    std::vector<AutoOdds::Candidate> candidates;
    for (const auto& bet : pPlayer->getBets())
    {
        if (!AutoOdds::isOddsBetName(bet->betName()))
        {
            continue;
        }
        AutoOdds::Candidate c;
        c.betId    = bet->betId();
        c.betName  = bet->betName();
        c.pivot    = bet->pivot();
        c.contract = bet->contractAmount();
        c.odds     = bet->oddsAmount();
        c.limits   = Gbl::pTable->getBetAmountLimits(
                         c.betName, c.pivot, true, c.contract);
        candidates.push_back(c);
    }

    const auto plan = prefs->autoOdds().plan(candidates);
    if (plan.specs.empty() && plan.skipped.empty())
    {
        return;
    }

    // Not LastFill: AutoOdds is not the player's own choice of bet
    const auto result = BetExecutor::apply(*pPlayer, plan.specs, nullptr);

    Ctrl::UslAutoOddsApplied usl;
    usl.playerId = playerId;
    usl.placed   = static_cast<unsigned>(result.count(BetExecutor::Status::Placed) +
                                         result.count(BetExecutor::Status::Changed));
    usl.skipped  = static_cast<unsigned>(result.count(BetExecutor::Status::Skipped) +
                                         plan.skipped.size());
    usl.headline = "AutoOdds: " + std::to_string(usl.placed) + " placed" +
                   (usl.skipped ? ", " + std::to_string(usl.skipped) + " skipped" : "");

    for (const auto& line : plan.skipped)
    {
        usl.skippedDetail += (usl.skippedDetail.empty() ? "" : "\n") + line;
    }
    const auto detail = result.skippedDetail();
    if (!detail.empty())
    {
        usl.skippedDetail += (usl.skippedDetail.empty() ? "" : "\n") + detail;
    }

    LOG_INFO(usl.headline + (usl.skippedDetail.empty() ? "" : "\n" + usl.skippedDetail));
    Gen::EventManager::instance().publish(usl);
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Takes back the player's latest bet change since the last roll.
// Always answers with RspUndoBet. The screens follow from the bet and
// balance events the undo causes, as for any bet change.
//
void
EventHandler::onReqSetBetFlags(const Ctrl::ReqSetBetFlags& ev) const
{
    const std::string fname("EventHandler::onReqSetBetFlags(): ");
    LOG_TRACE("Entered " + fname);

    using Gen::ErrorPass;
    Ctrl::RspSetBetFlags rsp;
    rsp.correlationId = ev.correlationId;
    rsp.playerId      = ev.playerId;

    auto fail = [&](ErrorPass::ErrorName name, const std::string& title,
                    const std::string& why)
    {
        ErrorPass e;
        EP_SET(e, name, ErrorPass::ErrorType::ProcessingError,
                  ErrorPass::Severity::Indeterminate, why);
        e.setErrorName(title);
        rsp.error = std::move(e);
        Gen::EventManager::instance().publish(rsp);
    };

    ErrorPass ep;
    Craps::Player* pPlayer = Gbl::pPlayerMgr->getPlayer(ev.playerId, ep);
    if (pPlayer == nullptr)
    {
        ep.prepend(fname);
        LOG_ERROR(fname + " Error" + ep.fullDesc());
        rsp.error = std::move(ep);
        Gen::EventManager::instance().publish(rsp);
        return;
    }
    if (Gbl::pTable != nullptr && !Gbl::pTable->isBettingOpen())
    {
        fail(ErrorPass::ErrorName::NoPermissionError, "Betting Closed",
             "Betting is closed while the dice are rolling. "
             "Try again when betting reopens.");
        return;
    }

    // The bets the request is about
    std::vector<Craps::BetPtr> targets;
    for (const auto& bet : pPlayer->getBets())
    {
        if (ev.scope == BetFlags::Scope::Bet)
        {
            if (bet->betId() == ev.betId) targets.push_back(bet);
        }
        else if (BetFlags::inScope(ev.scope, *bet))
        {
            targets.push_back(bet);
        }
    }

    if (targets.empty())
    {
        fail(ErrorPass::ErrorName::NotFoundError, "No Bets to Flag",
             ev.scope == BetFlags::Scope::Bet
                 ? "That bet is no longer on the table."
                 : "There are no " + BetFlags::scopeText(ev.scope) +
                   " on the table that can be turned off or on.");
        return;
    }

    UndoGroup group;
    for (const auto& bet : targets)
    {
        const auto [standing, oneRoll] = BetFlags::newState(ev.change, *bet);
        if (standing == bet->flagStanding() && oneRoll == bet->flagOneRoll())
        {
            ++rsp.unchanged;
            continue;
        }

        auto undoChange = std::make_unique<UndoBetFlagsChanged>(bet);
        ErrorPass e;
        if (pPlayer->setBetFlags(bet, standing, oneRoll, e) ==
            Gen::ReturnCode::Fail)
        {
            // Keep what was done so far undoable, then report
            if (group.size() > 0)
            {
                UndoManager::instance().push(
                    ev.playerId, std::make_unique<UndoGroup>(std::move(group)));
            }
            e.prepend(fname);
            LOG_ERROR(fname + " Error" + e.fullDesc());
            rsp.error = std::move(e);
            Gen::EventManager::instance().publish(rsp);
            return;
        }
        group.add(std::move(undoChange));
        ++rsp.changed;
    }

    if (group.size() > 0)
    {
        UndoManager::instance().push(
            ev.playerId, std::make_unique<UndoGroup>(std::move(group)));
    }

    const std::string what = (ev.scope == BetFlags::Scope::Bet)
        ? BetFlags::betLabel(*targets[0])
        : BetFlags::scopeText(ev.scope);
    rsp.accepted = true;
    rsp.message  = "Set " + what + " to " + BetFlags::changeText(ev.change) + ": ";
    if (rsp.changed == 0)
    {
        rsp.message += "already set";
    }
    else
    {
        rsp.message += std::to_string(rsp.changed) +
                       (rsp.changed == 1 ? " bet changed" : " bets changed");
        if (rsp.unchanged > 0)
        {
            rsp.message += ", " + std::to_string(rsp.unchanged) + " already set";
        }
    }
    LOG_INFO("Bet flags for " + ev.playerId + ": " + rsp.message);
    Gen::EventManager::instance().publish(rsp);
}

//----------------------------------------------------------------

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
// Moves the user player, and the buddies who follow, to another table.
// Always answers with RspSwitchTable. Only when the switch happened,
// follows with UslTableSwitched, then publishes the new table's state so
// the View's displays start from the right numbers.
//
void
EventHandler::onReqSwitchTable(const Ctrl::ReqSwitchTable& ev) const
{
    const std::string fname("EventHandler::onReqSwitchTable(): ");
    LOG_TRACE("Entered " + fname);

    Ctrl::RspSwitchTable rsp;
    rsp.correlationId = ev.correlationId;
    rsp.toTableId     = ev.tableId;
    if (Gbl::pTable)
    {
        rsp.fromTableId = Gbl::pTable->getTableId();
    }

    TableManager::SwitchResult result;
    Gen::ErrorPass ep;
    if (Gbl::pTableMgr->switchCrapsTable(ev.tableId, ev.buddiesFollow,
                                         result, ep) == Gen::ReturnCode::Success)
    {
        rsp.switched           = true;
        rsp.fromTableId        = result.fromTableId;
        rsp.toTableName        = result.toTableName;
        rsp.numBuddiesFollowed = result.buddiesFollowed;
        rsp.numBuddiesLeft     = result.buddiesLeft;
        rsp.numBetsReturned    = result.betsReturned;
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
        Ctrl::UslTableSwitched usl;
        usl.fromTableId = rsp.fromTableId;
        usl.toTableId   = rsp.toTableId;
        Gen::EventManager::instance().publish(usl);
        Gbl::pTable->publishSnapshot();
    }
}

//----------------------------------------------------------------
//
// Keeps the user's choice of what the buddies do on a table switch.
//
void
EventHandler::onReqSetBuddyFollow(const Ctrl::ReqSetBuddyFollow& ev) const
{
    LOG_TRACE("Entered EventHandler::onReqSetBuddyFollow()");
    Gbl::pPlayerMgr->userPrefs().setBuddyFollow(ev.value);
    Gbl::pPlayerMgr->savePlayers();
    // No response event
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

