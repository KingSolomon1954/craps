//----------------------------------------------------------------
//
// File: OperationManager.cpp
//
//----------------------------------------------------------------

#include <cui/managers/OperationManager.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiSender.h>
#include <cui/dialogs/DialogBetAmount.h>
#include <cui/dialogs/DialogConfirm.h>
#include <cui/dialogs/DialogError.h>
#include <cui/dialogs/DialogMessage.h>
#include <cui/dialogs/DialogPickPlayer.h>
#include <cui/dialogs/DialogQuickBetsSetAdd.h>
#include <cui/dialogs/DialogTextInput.h>
#include <controller/AnimationSpeed.h>
#include <controller/AutoOdds.h>
#include <controller/BetSpec.h>
#include <controller/QuickBets.h>
#include <controller/CrapsReaders.h>
#include <controller/RollHistoryFormat.h>
#include <controller/PlayerManager.h>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <gen/MoneyUtils.h>
#include <craps/EnumBetName.h>
#include <cctype>
#include <gen/Logger.h>
#include <string>

using namespace Cui;

namespace {

std::string
trimmed(const std::string& s)
{
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) b++;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) e--;
    return s.substr(b, e - b);
}

bool
sameIgnoringCase(const std::string& a, const std::string& b)
{
    return a.size() == b.size() &&
        std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
            return std::tolower(static_cast<unsigned char>(x)) ==
                   std::tolower(static_cast<unsigned char>(y)); });
}

} // namespace

//----------------------------------------------------------------

OperationManager&
OperationManager::instance()
{
    static OperationManager mgr;
    return mgr;
}

//----------------------------------------------------------------

bool
OperationManager::isActive() const
{
    return stage_ != Stage::Idle;
}

//----------------------------------------------------------------

void
OperationManager::makeBet()
{
    if (isActive())
    {
        // TODO can't just log if this happens
        LOG_WARN("OperationManager::makeBet(): operation already active");
        return;
    }
    
    type_          = OperationType::MakeBet;
    stage_         = Stage::AwaitingResponse;
    const auto& cb = CarrierBet::instance();
    
    if (cb.isOddsBet())
    {
        doOddsBet(cb);
    }
    else
    {
        doContractBet(cb);
    }
}

//----------------------------------------------------------------

void
OperationManager::doContractBet(const CarrierBet& cb)
{
    correlationId_ = CuiSender::instance().reqMakeBet(
                         cb.getPlayerId(),
                         cb.getBetName(),
                         cb.getContractAmount(),
                         cb.getPivot());
    LOG_TRACE("OperationManager::doContractBet(): request correlationId = " +
              std::to_string(correlationId_));
}

//----------------------------------------------------------------

void
OperationManager::doOddsBet(const CarrierBet& cb)
{
    LOG_TRACE("OperationManager::doOddsBet(): request correlationId = " +
              std::to_string(correlationId_));
    
    correlationId_ = CuiSender::instance().reqOddsBet(
                         cb.getPlayerId(),
                         cb.getBetId(),
                         cb.getOddsAmount());

}

//----------------------------------------------------------------

void
OperationManager::onRspMakeBet(const Ctrl::RspMakeBet& ev)
{
    LOG_TRACE("OperationManager::onRspMakeBet() "
              "correlationId = " + std::to_string(correlationId_));
             
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::MakeBet  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspMakeBet(): ignoring "
                 "unmatched RspMakeBet: rsp.correlationId = "       +
                 std::to_string(ev.correlationId)                   +
                 ", have correlationId_ = "                         +
                 std::to_string(correlationId_));
        return;
    }

    if (ev.accepted)
    {
        // ReqMakeBet has succeeded. Unwind the active betting workflow.
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }

    showError(ev.error);
}

//----------------------------------------------------------------
//
// Lists what can come down from the user player's bets and lets the
// person pick one entry, or all of them.
//
//   - A bet the table allows to be removed: "remove the whole bet".
//   - A bet with odds: "remove the odds only", leaving the contract bet.
//     That is the only way to take down the odds on a Pass Line or Come
//     bet once its point is set, because those contract bets must stay.
//
bool
OperationManager::removeBets()
{
    if (isActive()) return false;

    using Reader = Ctrl::CrapsReaders;
    const auto playerId = Reader::getUserPlayerId();

    Gen::ErrorPass ep;
    Craps::BetIds betIds;
    (void) Reader::readPlayerGetBets(playerId, betIds, ep);

    // Newest first; ids grow as bets are made
    std::sort(betIds.begin(), betIds.end(), std::greater<Craps::BetId>());

    std::vector<DialogPickPlayer::Item> items;
    std::vector<Removal> choices;
    std::size_t numBets = 0, numOddsOnly = 0, numLocked = 0;

    auto money = [](Gen::Money m) { return Gen::MoneyUtils::toStringNoCommas(m); };

    for (auto betId : betIds)
    {
        Gen::ErrorPass epBet;
        bool removable = false;
        BetName    betName  = BetName::Invalid;
        unsigned   pivot    = 0;
        Gen::Money contract = 0;
        Gen::Money odds     = 0;

        if (Reader::readBetIsRemovable(betId, removable, epBet) !=
                Gen::ReturnCode::Success ||
            Reader::readBetName(betId, betName, epBet) !=
                Gen::ReturnCode::Success ||
            Reader::readBetPivot(betId, pivot, epBet) !=
                Gen::ReturnCode::Success ||
            Reader::readBetContractAmount(betId, contract, epBet) !=
                Gen::ReturnCode::Success ||
            Reader::readBetOddsAmount(betId, odds, epBet) !=
                Gen::ReturnCode::Success)
        {
            continue;     // Bet went away while we looked
        }

        const std::string name = EnumBetName::toString(betName) +
            (pivot != 0 ? " " + std::to_string(pivot) : std::string());

        if (removable)
        {
            DialogPickPlayer::Item item;
            item.playerId    = playerId;
            item.betId       = betId;
            item.tag         = choices.size();
            item.text        = name;
            item.note        = money(contract);
            item.description = odds > 0 ? "odds " + money(odds) : "";
            items.push_back(std::move(item));
            choices.push_back({betId, false});
            ++numBets;
        }
        else
        {
            ++numLocked;
        }

        if (odds > 0)
        {
            DialogPickPlayer::Item item;
            item.playerId    = playerId;
            item.betId       = betId;
            item.tag         = choices.size();
            item.text        = name + " odds only";
            item.note        = money(odds);
            item.description = "keeps " + money(contract) + " bet";
            items.push_back(std::move(item));
            choices.push_back({betId, true});
            if (!removable) ++numOddsOnly;
        }
    }

    if (items.empty())
    {
        if (numLocked > 0)
        {
            showNotice("No bets can be removed",
                       "Pass Line and Come bets must stay on the table "
                       "once their point is established.");
        }
        else
        {
            showNotice("No bets to remove",
                       "You have no bets on the table.");
        }
        return true;
    }

    // Last, so that pressing Enter on the first line can't do it by accident
    if (numBets + numOddsOnly > 1)
    {
        DialogPickPlayer::Item item;
        item.playerId = playerId;
        item.tag      = choices.size();            // One past the end: all
        item.text     = "All removable";
        std::string d = std::to_string(numBets) +
                        (numBets == 1 ? " bet" : " bets");
        if (numOddsOnly > 0)
        {
            d += ", odds on " + std::to_string(numOddsOnly);
        }
        item.description = d;
        items.push_back(std::move(item));
    }

    removalChoices_ = std::move(choices);
    type_  = OperationType::RemoveBet;
    stage_ = Stage::AwaitingPick;
    DialogPickPlayer::instance().configure("Remove Bets", std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

//----------------------------------------------------------------
//
// Pick how fast the dice roll animation plays. The setting applies to
// the rest of this game and is remembered for the next one. The picker
// pops itself, which returns to the Configuration menu.
//
bool
OperationManager::chooseAnimationSpeed()
{
    if (isActive()) return false;

    using Ctrl::AnimationSpeed;
    const auto current = Ctrl::CrapsReaders::readAnimationSpeed();

    auto describe = [](AnimationSpeed s) -> const char*
    {
        switch (s)
        {
        case AnimationSpeed::Off:    return "no animation; the roll shows at once";
        case AnimationSpeed::Fast:   return "twice as fast as normal";
        case AnimationSpeed::Normal: return "the usual pace";
        case AnimationSpeed::Slow:   return "one and a half times slower";
        }
        return "";
    };
    auto label = [](AnimationSpeed s) -> std::string
    {
        std::string t = Ctrl::toString(s);
        t[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(t[0])));
        return t;
    };

    std::vector<DialogPickPlayer::Item> items;
    std::size_t index = 0;
    for (auto s : Ctrl::AllAnimationSpeeds)
    {
        DialogPickPlayer::Item item;
        item.tag         = index++;
        item.text        = label(s);
        item.note        = (s == current) ? "(current)" : "";
        item.description = describe(s);
        items.push_back(std::move(item));
    }

    type_  = OperationType::AnimationSpeed;
    stage_ = Stage::AwaitingPick;
    DialogPickPlayer::instance().configure("Animation Speed", std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

//----------------------------------------------------------------
//
// Pick how the roll history lists the rolls. Same shape as
// chooseAnimationSpeed(). Keys 1 to 4 pick directly.
//
bool
OperationManager::chooseRollHistoryFormat()
{
    if (isActive()) return false;

    using Ctrl::RollHistoryFormat;
    const auto current = Ctrl::CrapsReaders::readRollHistoryFormat();

    auto sample = [](RollHistoryFormat f) -> const char*
    {
        switch (f)
        {
        case RollHistoryFormat::Spaced:     return "7 8 6 10";
        case RollHistoryFormat::Commas:     return "7,8,6,10";
        case RollHistoryFormat::DiceJoined: return "7(34) 8(44) 6(33) 10(65)";
        case RollHistoryFormat::DiceComma:  return "7(3,4) 8(4,4) 6(3,3) 10(6,5)";
        }
        return "";
    };

    std::vector<DialogPickPlayer::Item> items;
    std::size_t index = 0;
    for (auto f : Ctrl::AllRollHistoryFormats)
    {
        DialogPickPlayer::Item item;
        item.tag         = index++;
        item.text        = sample(f);
        item.description = (f == current) ? "current" : "";
        items.push_back(std::move(item));
    }

    type_  = OperationType::RollHistoryFormat;
    stage_ = Stage::AwaitingPick;
    DialogPickPlayer::instance().configure("Format Roll History",
                                           std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

//----------------------------------------------------------------
//
// Send the next queued removal. An odds-only removal is a request to
// set the odds amount to zero, which the table accepts as taking down
// the odds. One request at a time; the response sends the next.
//
void
OperationManager::sendNextRemoval()
{
    const Removal r = removalQueue_.front();
    removalQueue_.erase(removalQueue_.begin());

    // Taking down several bets at once is one step for undo
    const bool joinsPrevious = removalStarted_;
    removalStarted_ = true;

    const auto playerId = Ctrl::CrapsReaders::getUserPlayerId();
    stage_ = Stage::AwaitingResponse;
    correlationId_ = r.oddsOnly
        ? CuiSender::instance().reqOddsBet(playerId, r.betId, 0, joinsPrevious)
        : CuiSender::instance().reqRemoveBet(playerId, r.betId, joinsPrevious);
}

//----------------------------------------------------------------

void
OperationManager::onRemovalResult(bool accepted, const Gen::ErrorPass& error)
{
    if (accepted)
    {
        if (!removalQueue_.empty())
        {
            sendNextRemoval();
            return;
        }
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }

    removalQueue_.clear();     // Stop at the first failure
    showError(error, "Betting Error", "Unable to Remove Bet");
}

//----------------------------------------------------------------
//
// Take back the player's latest bet change. One request. The bet and
// balance events that follow redraw the screens as for any bet change.
// On success the betting menus unwind, as after any bet.
//
bool
OperationManager::undoLastBet()
{
    if (isActive()) return false;

    type_  = OperationType::UndoBet;
    stage_ = Stage::AwaitingResponse;
    correlationId_ = CuiSender::instance().reqUndoBet(
        Ctrl::CrapsReaders::getUserPlayerId());
    return true;
}

//----------------------------------------------------------------

void
OperationManager::onRspUndoBet(const Ctrl::RspUndoBet& ev)
{
    LOG_TRACE("OperationManager::onRspUndoBet() "
              "correlationId = " + std::to_string(correlationId_));

    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::UndoBet  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspUndoBet(): ignoring "
                 "unmatched RspUndoBet: rsp.correlationId = " +
                 std::to_string(ev.correlationId));
        return;
    }

    if (ev.accepted)
    {
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }
    showError(ev.error, "Betting Error", "Unable to Undo");
}

//----------------------------------------------------------------
//
// Pick one of the user's QuickBets entries and apply it. The pick list
// shows each entry's name and a short description of its bets. Applying
// is one request; the answer says what was placed and what was skipped.
//
bool
OperationManager::applyQuickBets()
{
    if (isActive()) return false;

    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    if (plans.empty())
    {
        showNotice("No QuickBets",
                   "You have not set up any QuickBets yet.");
        return true;
    }

    quickBetNames_.clear();
    std::vector<DialogPickPlayer::Item> items;
    for (const auto& plan : plans)
    {
        DialogPickPlayer::Item item;
        item.tag         = quickBetNames_.size();
        item.text        = plan.name;
        item.description = Ctrl::describe(plan);
        items.push_back(std::move(item));
        quickBetNames_.push_back(plan.name);
    }

    type_  = OperationType::QuickBets;
    stage_ = Stage::AwaitingPick;
    DialogPickPlayer::instance().configure("QuickBets - Apply",
                                           std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

//----------------------------------------------------------------
//
// Save the bet being entered into a QuickBets entry. DialogBetAmount
// calls this with the amount already in CarrierBet. The player picks a
// new entry or an existing one to add to. A new entry asks for a name.
// Afterwards the player is back at DialogBetAmount, with a notice saying
// what happened, so the bet can still be placed.
//
bool
OperationManager::quickBetsSetOrAdd()
{
    if (isActive()) return false;

    const auto& cb = CarrierBet::instance();
    Ctrl::BetSpec spec;
    spec.betName = cb.getBetName();
    spec.isOdds  = cb.isOddsBet();
    spec.pivot   = cb.getPivot();
    spec.amount  = spec.isOdds ? cb.getOddsAmount() : cb.getContractAmount();
    if (spec.isOdds && Ctrl::isPlanOddsBet(spec.betName))
    {
        spec.pivot = 0;          // Line odds sit on the line bet
    }

    if (const auto why = Ctrl::QuickBets::checkSpec(spec); !why.empty())
    {
        showNotice("Can't save to QuickBets", why);
        return true;
    }

    std::string betText = EnumBetName::toString(spec.betName) + " bet";
    if (spec.pivot != 0) betText += " " + std::to_string(spec.pivot);
    if (spec.isOdds)     betText += " odds";
    betText += " " + Gen::MoneyUtils::toString(spec.amount);

    quickBetSpec_ = spec;
    quickBetNames_.clear();
    std::vector<DialogQuickBetsSetAdd::Entry> entries;
    for (const auto& plan : Ctrl::CrapsReaders::readQuickBets())
    {
        entries.push_back({plan.name, Ctrl::describe(plan)});
        quickBetNames_.push_back(plan.name);
    }

    type_  = OperationType::QuickBetsEdit;
    stage_ = Stage::AwaitingPick;
    DialogQuickBetsSetAdd::instance().configure(betText, std::move(entries));
    SurfaceManager::instance().pushSurface(&DialogQuickBetsSetAdd::instance());
    return true;
}

//----------------------------------------------------------------

void
OperationManager::onQuickBetsChoice(bool accepted, std::size_t index)
{
    if (stage_ != Stage::AwaitingPick || type_ != OperationType::QuickBetsEdit)
        return;

    if (!accepted)
    {
        finish();
        return;
    }

    if (index == DialogQuickBetsSetAdd::NewEntry)
    {
        askForQuickBetsName();
        return;
    }
    if (index >= quickBetNames_.size())
    {
        finish();
        return;
    }

    quickBetTarget_ = quickBetNames_[index];
    sendQuickBetsEdit(Ctrl::ReqEditQuickBets::Action::AddBet);
}

//----------------------------------------------------------------
//
// Name for a new QuickBets entry. The dialog says why a name is refused,
// so it can be fixed without leaving.
//
void
OperationManager::askForQuickBetsName()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();

    auto validator = [plans](const std::string& raw) -> std::string
    {
        const std::string name = trimmed(raw);
        if (auto why = Ctrl::QuickBets::checkName(name); !why.empty())
        {
            return why;
        }
        for (const auto& p : plans)
        {
            if (sameIgnoringCase(p.name, name))
            {
                return "An entry named " + p.name + " already exists.";
            }
        }
        if (plans.size() >= Ctrl::QuickBets::MaxEntries)
        {
            return "You already have " +
                   std::to_string(Ctrl::QuickBets::MaxEntries) +
                   " entries. Delete one first.";
        }
        return std::string();
    };

    stage_ = Stage::AwaitingText;
    DialogTextInput::instance().configure(
        "New QuickBets Entry", "Name: ", "",
        Ctrl::QuickBets::MaxNameLength, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::sendQuickBetsEdit(Ctrl::ReqEditQuickBets::Action action)
{
    stage_ = Stage::AwaitingResponse;

    Ctrl::ReqEditQuickBets req;
    req.playerId = Ctrl::CrapsReaders::getUserPlayerId();
    req.action   = action;
    req.name     = quickBetTarget_;
    req.spec     = quickBetSpec_;
    correlationId_ = CuiSender::instance().reqEditQuickBets(std::move(req));
}

//----------------------------------------------------------------
//
// AutoOdds, from the Configuration menu.
//
// One pick list of things to do:
//
//   turn on / turn off   one request
//   show multiples       a notice, no request
//   set a multiple       pick bet type, pick point (or all), type the
//                        multiple (0 clears), one request
//   3-4-5 preset         one request
//   clear all            confirm, one request
//
// The pick lists and prompts each pop themselves and report here, so
// autoStep_ says which one just answered.
//
bool
OperationManager::manageAutoOdds()
{
    if (isActive()) return false;

    type_ = OperationType::AutoOdds;
    pickAutoAction();
    return true;
}

//----------------------------------------------------------------

namespace {

const char*
typeWords(BetName b)
{
    switch (b)
    {
    case BetName::PassLine: return "Pass Line";
    case BetName::DontPass: return "Don't Pass";
    case BetName::Come:     return "Come";
    case BetName::DontCome: return "Don't Come";
    default:                return "?";
    }
}

} // namespace

//----------------------------------------------------------------

void
OperationManager::pickAutoAction()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();
    autoTurnOn_ = !ao.enabled();

    std::vector<DialogPickPlayer::Item> items;
    auto add = [&](std::size_t tag, const std::string& text, const char* desc)
    {
        DialogPickPlayer::Item item;
        item.tag         = tag;
        item.text        = text;
        item.description = desc;
        items.push_back(std::move(item));
    };
    add(0, autoTurnOn_ ? "Turn on" : "Turn off",
        autoTurnOn_ ? "AutoOdds is off now" : "AutoOdds is on now");
    add(1, "List Bets",        "what is set for each bet type");
    add(2, "Set AutoOdds Bet", "pick a bet type and point");
    add(3, "3-4-5 preset",     "3x on 4 and 10, 4x on 5 and 9, 5x on 6 and 8");
    add(4, "Clear all",        "remove every bet");

    stage_    = Stage::AwaitingPick;
    autoStep_ = AutoStep::PickAction;
    DialogPickPlayer::instance().configure("AutoOdds - Configure",
                                           std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
}

//----------------------------------------------------------------

void
OperationManager::pickAutoType()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();

    std::vector<DialogPickPlayer::Item> items;
    std::size_t tag = 0;
    for (BetName b : Ctrl::AutoOdds::oddsBetNames())
    {
        DialogPickPlayer::Item item;
        item.tag         = tag++;
        item.text        = typeWords(b);
        item.description = ao.summary(b);
        items.push_back(std::move(item));
    }

    stage_    = Stage::AwaitingPick;
    autoStep_ = AutoStep::PickType;
    DialogPickPlayer::instance().configure("AutoOdds - Which bet?",
                                           std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
}

//----------------------------------------------------------------

void
OperationManager::pickAutoPoint()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();

    std::vector<DialogPickPlayer::Item> items;
    std::size_t tag = 0;
    for (unsigned point : Ctrl::AutoOdds::points())
    {
        DialogPickPlayer::Item item;
        item.tag  = tag++;
        item.text = "Point " + std::to_string(point);
        item.description = ao.setting(autoBetName_, point).toString();
        items.push_back(std::move(item));
    }
    DialogPickPlayer::Item all;
    all.tag         = tag;
    all.text        = "All points";
    all.description = "the same setting for every point";
    items.push_back(std::move(all));

    stage_    = Stage::AwaitingPick;
    autoStep_ = AutoStep::PickPoint;
    DialogPickPlayer::instance().configure(
        std::string("AutoOdds - ") + typeWords(autoBetName_) + ", which point?",
        std::move(items), DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
}

//----------------------------------------------------------------

void
OperationManager::askAutoMultiple()
{
    auto validator = [](const std::string& raw) -> std::string
    {
        Ctrl::AutoOdds::Setting setting;
        return Ctrl::AutoOdds::parseSetting(raw, setting);
    };

    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();
    const auto current = (autoPoint_ == 0) ? Ctrl::AutoOdds::Setting{}
                       : ao.setting(autoBetName_, autoPoint_);

    stage_    = Stage::AwaitingText;
    autoStep_ = AutoStep::AskMultiple;
    DialogTextInput::instance().configure(
        std::string("AutoOdds ") + typeWords(autoBetName_) +
            (autoPoint_ == 0 ? ", all points" : " " + std::to_string(autoPoint_)),
        "Amount like 50, or 3x the bet (0 clears): ",
        current.isSet() ? current.toString() : std::string(),
        10, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::onAutoPick(bool accepted, std::size_t tag)
{
    if (!accepted)
    {
        finish();       // Cancelled; stay on the Configure menu
        return;
    }

    switch (autoStep_)
    {
    case AutoStep::PickAction:
        switch (tag)
        {
        case 0:
        {
            Ctrl::ReqEditAutoOdds req;
            req.action  = Ctrl::ReqEditAutoOdds::Action::SetEnabled;
            req.enabled = autoTurnOn_;
            sendAutoRequest(std::move(req));
            return;
        }
        case 1:
        {
            const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();
            std::string detail;
            for (BetName b : Ctrl::AutoOdds::oddsBetNames())
            {
                std::string label = std::string(typeWords(b)) + ":";
                label.resize(12, ' ');
                detail += label + ao.summary(b) + "\n";
            }
            detail += "\nAutoOdds is ";
            detail += ao.enabled() ? "on." : "off.";
            finish();
            showNotice("AutoOdds - List Bets", detail);
            return;
        }
        case 2:
            pickAutoType();
            return;
        case 3:
        {
            Ctrl::ReqEditAutoOdds req;
            req.action = Ctrl::ReqEditAutoOdds::Action::Preset345;
            sendAutoRequest(std::move(req));
            return;
        }
        case 4:
            autoStep_ = AutoStep::ConfirmClear;
            askConfirmation("Clear every AutoOdds multiple?");
            return;
        default:
            finish();
            return;
        }

    case AutoStep::PickType:
    {
        const auto& names = Ctrl::AutoOdds::oddsBetNames();
        if (tag >= names.size())
        {
            finish();
            return;
        }
        autoBetName_ = names[tag];
        pickAutoPoint();
        return;
    }

    case AutoStep::PickPoint:
    {
        const auto& points = Ctrl::AutoOdds::points();
        if (tag > points.size())
        {
            finish();
            return;
        }
        autoPoint_ = (tag == points.size()) ? 0u : points[tag];
        askAutoMultiple();
        return;
    }

    default:
        finish();
        return;
    }
}

//----------------------------------------------------------------

void
OperationManager::sendAutoRequest(Ctrl::ReqEditAutoOdds request)
{
    request.playerId = Ctrl::CrapsReaders::getUserPlayerId();
    stage_           = Stage::AwaitingResponse;
    correlationId_   = CuiSender::instance().reqEditAutoOdds(std::move(request));
}

//----------------------------------------------------------------
//
// From the bet amount dialog while entering odds: set AutoOdds for this
// bet type and point to exactly the amount entered. An amount of zero
// unsets it. One request, and the player stays in the amount dialog, with
// a notice.
//
bool
OperationManager::autoOddsSetOrUnset()
{
    if (isActive()) return false;

    const auto& cb = CarrierBet::instance();
    if (!cb.isOddsBet() || !Ctrl::AutoOdds::isOddsBetName(cb.getBetName()))
    {
        return true;
    }

    unsigned point = cb.getPivot();
    if (point == 0)
    {
        Craps::TableId tableId;
        Gen::ErrorPass ep;
        if (Ctrl::CrapsReaders::getActiveCrapsTable(tableId, ep) ==
                Gen::ReturnCode::Success)
        {
            Ctrl::CrapsReaders::readTablePoint(tableId, point, ep);
        }
    }
    if (!Ctrl::AutoOdds::isPoint(point))
    {
        showNotice("AutoOdds", "There is no point yet, so there is no "
                   "point to set AutoOdds for.");
        return true;
    }

    Ctrl::ReqEditAutoOdds req;
    req.action  = Ctrl::ReqEditAutoOdds::Action::Set;
    req.betName = cb.getBetName();
    req.point   = point;
    req.amount  = cb.getOddsAmount();       // Zero unsets

    type_ = OperationType::AutoOdds;
    sendAutoRequest(std::move(req));
    return true;
}

//----------------------------------------------------------------

void
OperationManager::onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& ev)
{
    LOG_TRACE("OperationManager::onRspEditAutoOdds() "
              "correlationId = " + std::to_string(correlationId_));

    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::AutoOdds ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspEditAutoOdds(): ignoring "
                 "unmatched RspEditAutoOdds: rsp.correlationId = " +
                 std::to_string(ev.correlationId));
        return;
    }

    if (!ev.accepted)
    {
        showError(ev.error, "AutoOdds Error", "Unable to Save AutoOdds");
        return;
    }

    finish();
    SurfaceManager::instance().draw();      // The status line may have changed
    showNotice("AutoOdds", ev.message);
}

//----------------------------------------------------------------
//
// AutoOdds acted after betting reopened. The status line says how many
// were placed; a notice lists what could not be, unless the player is in
// the middle of something else.
//
void
OperationManager::onAutoOddsApplied(const Ctrl::UslAutoOddsApplied& ev)
{
    LOG_TRACE("OperationManager::onAutoOddsApplied()");

    autoOddsStatus_ = "AutoOdds: " + std::to_string(ev.placed) + " placed";
    SurfaceManager::instance().draw();
    if (!ev.skippedDetail.empty() && !isActive())
    {
        showNotice(ev.headline, ev.skippedDetail);
    }
}

//----------------------------------------------------------------

void
OperationManager::clearAutoOddsStatus()
{
    autoOddsStatus_.clear();
}

//----------------------------------------------------------------

const std::string&
OperationManager::autoOddsStatus() const
{
    return autoOddsStatus_;
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Managing QuickBets, from the Configuration menu.
//
// Pick an entry, then pick what to do with it: rename it, change the
// amount of one of its bets, remove one of its bets, or delete it. Each
// action asks what it needs (a name, a bet, an amount, a confirmation)
// and then sends one ReqEditQuickBets. New entries are made from the bet
// amount dialog, where the bet is already chosen.
//
// The pick lists and prompts each pop themselves and report here, so
// manageStep_ says which one just answered.
//
bool
OperationManager::manageQuickBets()
{
    if (isActive()) return false;

    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    if (plans.empty())
    {
        showNotice("No QuickBets",
                   "You have not set up any QuickBets yet. While you enter "
                   "a bet, press q to save it into one.");
        return true;
    }

    quickBetNames_.clear();
    std::vector<DialogPickPlayer::Item> items;
    for (const auto& plan : plans)
    {
        DialogPickPlayer::Item item;
        item.tag         = quickBetNames_.size();
        item.text        = plan.name;
        item.description = Ctrl::describe(plan);
        items.push_back(std::move(item));
        quickBetNames_.push_back(plan.name);
    }

    type_       = OperationType::QuickBetsManage;
    stage_      = Stage::AwaitingPick;
    manageStep_ = ManageStep::PickEntry;
    DialogPickPlayer::instance().configure("QuickBets - Change which entry?",
                                           std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

//----------------------------------------------------------------

namespace {

const Ctrl::BetPlan*
findPlan(const std::vector<Ctrl::BetPlan>& plans, const std::string& name)
{
    for (const auto& p : plans)
    {
        if (p.name == name) return &p;
    }
    return nullptr;
}

} // namespace

//----------------------------------------------------------------

void
OperationManager::onManagePick(bool accepted, std::size_t tag)
{
    using Action = Ctrl::ReqEditQuickBets::Action;

    if (!accepted)
    {
        finish();       // Cancelled; stay on the Configure menu
        return;
    }

    switch (manageStep_)
    {
    case ManageStep::PickEntry:
        if (tag >= quickBetNames_.size())
        {
            finish();
            return;
        }
        manageEntry_ = quickBetNames_[tag];
        pickManageAction();
        return;

    case ManageStep::PickAction:
        switch (tag)
        {
        case 0:
            askManageName();
            return;
        case 1:
            pickManageBet(Action::SetAmount);
            return;
        case 2:
        {
            const auto plans = Ctrl::CrapsReaders::readQuickBets();
            const auto* plan = findPlan(plans, manageEntry_);
            if (plan != nullptr && plan->specs.size() <= 1)
            {
                finish();
                showNotice("Can't remove the bet",
                           manageEntry_ + " has only one bet. Delete the "
                           "entry instead.");
                return;
            }
            pickManageBet(Action::RemoveBet);
            return;
        }
        case 3:
            manageStep_ = ManageStep::ConfirmDelete;
            askConfirmation("Delete the QuickBets entry " + manageEntry_ + "?");
            return;
        default:
            finish();
            return;
        }

    case ManageStep::PickBet:
        manageBetIndex_ = tag;
        if (manageAction_ == Action::SetAmount)
        {
            askManageAmount();
        }
        else
        {
            sendManageRequest();
        }
        return;

    default:
        finish();
        return;
    }
}

//----------------------------------------------------------------

void
OperationManager::pickManageAction()
{
    std::vector<DialogPickPlayer::Item> items;
    auto add = [&](std::size_t tag, const char* text, const char* desc)
    {
        DialogPickPlayer::Item item;
        item.tag         = tag;
        item.text        = text;
        item.description = desc;
        items.push_back(std::move(item));
    };
    add(0, "Rename",           "give the entry another name");
    add(1, "Change an amount", "set a different amount for one bet");
    add(2, "Remove a bet",     "take one bet out of the entry");
    add(3, "Delete",           "delete the whole entry");

    stage_      = Stage::AwaitingPick;
    manageStep_ = ManageStep::PickAction;
    DialogPickPlayer::instance().configure("QuickBets - " + manageEntry_,
                                           std::move(items),
                                           DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
}

//----------------------------------------------------------------

void
OperationManager::pickManageBet(Ctrl::ReqEditQuickBets::Action action)
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const auto* plan = findPlan(plans, manageEntry_);
    if (plan == nullptr || plan->specs.empty())
    {
        finish();
        showNotice("QuickBets", "That entry is no longer there.");
        return;
    }

    std::vector<DialogPickPlayer::Item> items;
    for (std::size_t i = 0; i < plan->specs.size(); ++i)
    {
        DialogPickPlayer::Item item;
        item.tag  = i;
        item.text = Ctrl::describe(plan->specs[i]);
        items.push_back(std::move(item));
    }

    manageAction_ = action;
    stage_        = Stage::AwaitingPick;
    manageStep_   = ManageStep::PickBet;
    DialogPickPlayer::instance().configure(
        (action == Ctrl::ReqEditQuickBets::Action::SetAmount
             ? "Change the amount of which bet?" : "Remove which bet?"),
        std::move(items), DialogPickPlayer::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
}

//----------------------------------------------------------------

void
OperationManager::askManageName()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const std::string current = manageEntry_;

    auto validator = [plans, current](const std::string& raw) -> std::string
    {
        const std::string name = trimmed(raw);
        if (auto why = Ctrl::QuickBets::checkName(name); !why.empty())
        {
            return why;
        }
        for (const auto& p : plans)
        {
            if (p.name != current && sameIgnoringCase(p.name, name))
            {
                return "An entry named " + p.name + " already exists.";
            }
        }
        return std::string();
    };

    manageAction_ = Ctrl::ReqEditQuickBets::Action::Rename;
    stage_        = Stage::AwaitingText;
    manageStep_   = ManageStep::AskName;
    DialogTextInput::instance().configure(
        "Rename " + manageEntry_, "Name: ", manageEntry_,
        Ctrl::QuickBets::MaxNameLength, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::askManageAmount()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const auto* plan = findPlan(plans, manageEntry_);
    if (plan == nullptr || manageBetIndex_ >= plan->specs.size())
    {
        finish();
        showNotice("QuickBets", "That bet is no longer in the entry.");
        return;
    }
    const Ctrl::BetSpec& spec = plan->specs[manageBetIndex_];

    auto validator = [](const std::string& raw) -> std::string
    {
        const std::string text = trimmed(raw);
        if (text.empty())
        {
            return "Enter an amount.";
        }
        if (!std::all_of(text.begin(), text.end(),
                         [](unsigned char c) { return std::isdigit(c); }))
        {
            return "Use digits only.";
        }
        if (text.size() > 7 || std::stoul(text) == 0)
        {
            return "Enter an amount more than zero.";
        }
        return std::string();
    };

    stage_      = Stage::AwaitingText;
    manageStep_ = ManageStep::AskAmount;
    DialogTextInput::instance().configure(
        "Amount for " + Ctrl::describe(spec), "Amount: $",
        std::to_string(spec.amount), 7, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------
//
// Sends the request for the action chosen. text is the new name or the
// typed amount, when the action has one.
//
void
OperationManager::sendManageRequest(const std::string& text)
{
    using Action = Ctrl::ReqEditQuickBets::Action;

    Ctrl::ReqEditQuickBets req;
    req.playerId = Ctrl::CrapsReaders::getUserPlayerId();
    req.action   = manageAction_;
    req.name     = manageEntry_;
    req.index    = manageBetIndex_;
    if (manageAction_ == Action::Rename)
    {
        req.newName = text;
    }
    else if (manageAction_ == Action::SetAmount)
    {
        req.amount = static_cast<Gen::Money>(std::stoul(text));
    }

    stage_         = Stage::AwaitingResponse;
    correlationId_ = CuiSender::instance().reqEditQuickBets(std::move(req));
}

//----------------------------------------------------------------

void
OperationManager::onRspEditQuickBets(const Ctrl::RspEditQuickBets& ev)
{
    LOG_TRACE("OperationManager::onRspEditQuickBets() "
              "correlationId = " + std::to_string(correlationId_));

    const bool ours = type_ == OperationType::QuickBetsEdit ||
                      type_ == OperationType::QuickBetsManage;
    if (stage_ != Stage::AwaitingResponse || !ours ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspEditQuickBets(): ignoring "
                 "unmatched RspEditQuickBets: rsp.correlationId = " +
                 std::to_string(ev.correlationId));
        return;
    }

    if (!ev.accepted)
    {
        showError(ev.error, "QuickBets Error", "Unable to Save QuickBets");
        return;
    }

    // Stay where we are: the bet amount dialog or the Configure menu
    const bool managing = (type_ == OperationType::QuickBetsManage);
    finish();
    showNotice(managing ? "QuickBets changed" : "Saved to QuickBets", ev.message);
}

//----------------------------------------------------------------

void
OperationManager::onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& ev)
{
    LOG_TRACE("OperationManager::onRspApplyQuickBets() "
              "correlationId = " + std::to_string(correlationId_));

    if (stage_ != Stage::AwaitingResponse   ||
        type_  != OperationType::QuickBets  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspApplyQuickBets(): ignoring "
                 "unmatched RspApplyQuickBets: rsp.correlationId = " +
                 std::to_string(ev.correlationId));
        return;
    }

    if (!ev.accepted)
    {
        showError(ev.error, "Betting Error", "Unable to Apply QuickBets");
        return;
    }

    // Unwind to the table as after any bet. Say so if some bets were
    // not placed; a clean result needs no message.
    SurfaceManager::instance().popToRoot();
    if (ev.skipped > 0)
    {
        showNotice("QuickBets \"" + ev.name + "\": " + ev.headline,
                   ev.skippedDetail);
    }
    finish();
}

//----------------------------------------------------------------

void
OperationManager::onRspRemoveBet(const Ctrl::RspRemoveBet& ev)
{
    LOG_TRACE("OperationManager::onRspRemoveBet() "
              "correlationId = " + std::to_string(correlationId_));

    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::RemoveBet ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspRemoveBet(): ignoring "
                 "unmatched RspRemoveBet: rsp.correlationId = " +
                 std::to_string(ev.correlationId));
        return;
    }
    onRemovalResult(ev.accepted, ev.error);
}

//----------------------------------------------------------------

void
OperationManager::onRspOddsBet(const Ctrl::RspOddsBet& ev)
{
    LOG_TRACE("OperationManager::onRspOddsBet() "
              "correlationId = " + std::to_string(correlationId_));
             
    if (stage_ == Stage::AwaitingResponse &&
        type_  == OperationType::RemoveBet &&
        ev.correlationId == correlationId_)
    {
        onRemovalResult(ev.accepted, ev.error);   // Odds taken down
        return;
    }

    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::MakeBet  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspMakeBet(): ignoring "
                 "unmatched RspMakeBet: rsp.correlationId = "       +
                 std::to_string(ev.correlationId)                   +
                 ", have correlationId_ = "                         +
                 std::to_string(correlationId_));
        return;
    }

    if (ev.accepted)
    {
        // ReqOddsBet has succeeded. Unwind the active betting workflow.
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }

    showError(ev.error);
}

//----------------------------------------------------------------

void
OperationManager::showError(const Gen::ErrorPass& error,
                            const char* defaultType,
                            const char* defaultName)
{
    LOG_TRACE("OperationManager::showError()");

    // TODO labels should have already been set
    Gen::ErrorPass displayError = error;
    if (displayError.errorTypeStr().empty())
        displayError.setErrorType(defaultType);
    if (displayError.errorNameStr().empty())
        displayError.setErrorName(defaultName);
    if (displayError.description().empty())
        displayError.setDescription("The requested operation could not be completed.");

    stage_ = Stage::AwaitingErrorAcknowledgement;
    LOG_TRACE("calling DialogError::instance().configure(error)");
    DialogError::instance().configure(displayError);
    SurfaceManager::instance().pushSurface(&DialogError::instance());
}

//----------------------------------------------------------------

void
OperationManager::onErrorAcknowledged()
{
    if (stage_ != Stage::AwaitingErrorAcknowledgement) return;

    if (type_ == OperationType::Player)
    {
        // Back to the Player menu, which is still on the stack.
        finish();
        return;
    }

    if (type_ == OperationType::QuickBetsEdit ||
        type_ == OperationType::QuickBetsManage ||
        type_ == OperationType::AutoOdds)
    {
        // Back to where the person was: the bet amount dialog, or the
        // Configure menu. Both are still on the stack.
        finish();
        return;
    }

    // DialogError has already popped itself. Now unwind the operation's
    // menus/dialogs to the root screen and release the operation state.
    SurfaceManager::instance().popToRoot();
    finish();
}

//----------------------------------------------------------------

bool
OperationManager::confirmQuit()
{
    if (isActive()) return false;

    type_ = OperationType::Quit;
    stage_ = Stage::AwaitingConfirmation;
    DialogConfirm::instance().configure("Confirm Quit RoyalCraps game");
    SurfaceManager::instance().pushSurface(&DialogConfirm::instance());
    return true;
}

//----------------------------------------------------------------

void
OperationManager::onConfirmationResult(SurfaceBase::OperationResult result)
{
    if (stage_ != Stage::AwaitingConfirmation) return;

    if (type_ == OperationType::Player)
    {
        if (result == SurfaceBase::OperationResult::Yes)
        {
            sendPlayerRequest();
        }
        else
        {
            finish();
        }
        return;
    }

    if (type_ == OperationType::QuickBetsManage)
    {
        if (result == SurfaceBase::OperationResult::Yes)
        {
            manageAction_ = Ctrl::ReqEditQuickBets::Action::Delete;
            sendManageRequest();
        }
        else
        {
            finish();
        }
        return;
    }

    if (type_ == OperationType::AutoOdds)
    {
        if (result == SurfaceBase::OperationResult::Yes)
        {
            Ctrl::ReqEditAutoOdds req;
            req.action = Ctrl::ReqEditAutoOdds::Action::Clear;
            sendAutoRequest(std::move(req));
        }
        else
        {
            finish();
        }
        return;
    }

    if (type_ != OperationType::Quit) return;

    if (result == SurfaceBase::OperationResult::Yes)
    {
        CuiSender::instance().reqTerminate();
    }
    finish();
}

//----------------------------------------------------------------

void
OperationManager::finish()
{
    type_          = OperationType::None;
    stage_         = Stage::Idle;
    correlationId_ = 0;
    removalChoices_.clear();
    removalQueue_.clear();
    removalStarted_ = false;
    quickBetNames_.clear();
    quickBetTarget_.clear();
    manageEntry_.clear();
    manageBetIndex_ = 0;
    autoBetName_ = BetName::Invalid;
    autoPoint_   = 0;
    targetId_.clear();
    text_.clear();
    description_.clear();
    askingDescription_ = false;
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Player operations, started from MenuPlayers
//
// Each is a short chain of dialogs, then one request to the controller:
//
//   List, ListBuddies   show list
//   Switch              pick, [confirm if bets are on the table], request
//   Rename              type name, request
//   Create              type name, request
//   Delete              pick, confirm, request
//   BuddyJoins/Leaves   pick, request
//
// The dialogs pop themselves before calling back. The Player menu stays
// on the stack, except after a successful Switch, which returns to the
// table.
//
bool
OperationManager::playerOperation(PlayerOp op)
{
    if (isActive()) return false;

    const auto roster = Ctrl::CrapsReaders::readPlayerRoster();
    using Info = Ctrl::PlayerManager::PlayerInfo;

    auto pickable = [&](auto wanted, auto noteFor)
    {
        std::vector<DialogPickPlayer::Item> items;
        for (const Info& p : roster)
        {
            if (wanted(p))
            {
                items.push_back({p.playerId, p.name, noteFor(p),
                                 p.description});
            }
        }
        return items;
    };
    auto userNote  = [](const Info& p) -> std::string
        { return p.isUser ? "(you)" : ""; };
    auto buddyNote = [](const Info& p) -> std::string
        { return p.atTable ? "(at table)" : ""; };
    auto isUserKind  = [](const Info& p)
        { return p.kind == Ctrl::PlayerKind::User; };
    auto isBuddyKind = [](const Info& p)
        { return p.kind == Ctrl::PlayerKind::Buddy; };

    std::vector<DialogPickPlayer::Item> items;
    std::string title;
    auto mode = DialogPickPlayer::Mode::Pick;

    switch (op)
    {
    case PlayerOp::List:
        title = "Your Players";
        mode  = DialogPickPlayer::Mode::Show;
        items = pickable(isUserKind, userNote);
        break;

    case PlayerOp::ListBuddies:
        title = "Buddy Players";
        mode  = DialogPickPlayer::Mode::Show;
        items = pickable(isBuddyKind, buddyNote);
        if (items.empty())
        {
            showNotice("No buddy players", "There are no buddy players.");
            return true;
        }
        break;

    case PlayerOp::Switch:
        title = "Switch to Player";
        items = pickable([&](const Info& p)
                         { return isUserKind(p) && !p.isUser; }, userNote);
        if (items.empty())
        {
            showNotice("No other players",
                       "You have only one player. Use Create New Player "
                       "to make another.");
            return true;
        }
        break;

    case PlayerOp::Delete:
        title = "Delete Player";
        items = pickable([&](const Info& p)
                         { return isUserKind(p) && !p.isUser; }, userNote);
        if (items.empty())
        {
            showNotice("No player to delete",
                       "You can't delete the player you are playing as. "
                       "Switch to another player first.");
            return true;
        }
        break;

    case PlayerOp::BuddyJoins:
        title = "Buddy Joins Table";
        items = pickable([&](const Info& p)
                         { return isBuddyKind(p) && !p.atTable; }, buddyNote);
        if (items.empty())
        {
            showNotice("No buddy to add",
                       "All of the buddy players are already at the table.");
            return true;
        }
        break;

    case PlayerOp::BuddyLeaves:
        title = "Buddy Leaves Table";
        items = pickable([&](const Info& p)
                         { return isBuddyKind(p) && p.atTable; }, buddyNote);
        if (items.empty())
        {
            showNotice("No buddy at the table",
                       "None of the buddy players are at the table.");
            return true;
        }
        break;

    case PlayerOp::Rename:
    case PlayerOp::Create:
        break;   // Asks for a name below
    }

    type_     = OperationType::Player;
    playerOp_ = op;
    text_.clear();
    description_.clear();
    askingDescription_ = false;
    targetId_.clear();
    targetName_.clear();

    if (op == PlayerOp::Rename)
    {
        for (const Info& p : roster)
        {
            if (p.isUser)
            {
                targetId_   = p.playerId;
                targetName_ = p.name;
            }
        }
        askForName("Rename Player", targetName_, targetId_);
        return true;
    }

    if (op == PlayerOp::Create)
    {
        askForName("Create New Player", "", Craps::PlayerId());
        return true;
    }

    stage_ = Stage::AwaitingPick;
    DialogPickPlayer::instance().configure(title, std::move(items), mode);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

//----------------------------------------------------------------
//
// Ask for a player name. The dialog refuses names that are not valid or
// that another player (other than exceptId) already has, and says why,
// so the person can fix it without leaving the dialog.
//
void
OperationManager::askForName(const std::string& title,
                             const std::string& initial,
                             const Craps::PlayerId& exceptId)
{
    const auto roster = Ctrl::CrapsReaders::readPlayerRoster();

    auto validator = [roster, exceptId](const std::string& raw) -> std::string
    {
        const std::string name = trimmed(raw);
        std::string why;
        if (!Ctrl::PlayerManager::checkName(name, why))
        {
            return why;
        }
        for (const auto& p : roster)
        {
            if (p.playerId != exceptId && sameIgnoringCase(p.name, name))
            {
                return "A player named " + p.name + " already exists.";
            }
        }
        return std::string();
    };

    stage_ = Stage::AwaitingText;
    DialogTextInput::instance().configure(
        title, "Name: ", initial,
        Ctrl::PlayerManager::MaxNameLength, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::onPickResult(bool accepted, const Craps::PlayerId& playerId,
                               Craps::BetId betId, std::size_t tag)
{
    if (stage_ == Stage::AwaitingPick && type_ == OperationType::QuickBetsManage)
    {
        (void)playerId; (void)betId;
        onManagePick(accepted, tag);
        return;
    }

    if (stage_ == Stage::AwaitingPick && type_ == OperationType::AutoOdds)
    {
        (void)playerId; (void)betId;
        onAutoPick(accepted, tag);
        return;
    }

    if (stage_ == Stage::AwaitingPick && type_ == OperationType::RemoveBet)
    {
        (void)playerId; (void)betId;
        if (!accepted || tag > removalChoices_.size())
        {
            finish();       // Cancelled; stay on the betting menu
            return;
        }

        // tag is an index into removalChoices_; one past the end means
        // "all of them".
        if (tag == removalChoices_.size())
        {
            removalQueue_ = removalChoices_;   // All is already the right set
            // Choices hold both "whole" and "odds only" for a Don't bet
            // with odds. Keep just one removal per bet: the whole bet.
            std::vector<Removal> unique;
            for (const auto& r : removalChoices_)
            {
                bool seen = false;
                for (auto& u : unique)
                {
                    if (u.betId == r.betId)
                    {
                        seen = true;
                        if (!r.oddsOnly) u.oddsOnly = false;
                    }
                }
                if (!seen) unique.push_back(r);
            }
            removalQueue_ = std::move(unique);
        }
        else
        {
            removalQueue_.assign(1, removalChoices_[tag]);
        }
        sendNextRemoval();
        return;
    }

    if (stage_ == Stage::AwaitingPick && type_ == OperationType::QuickBets)
    {
        (void)playerId; (void)betId;
        if (!accepted || tag >= quickBetNames_.size())
        {
            quickBetNames_.clear();
            finish();       // Cancelled; stay on the betting menu
            return;
        }
        stage_ = Stage::AwaitingResponse;
        correlationId_ = CuiSender::instance().reqApplyQuickBets(
            Ctrl::CrapsReaders::getUserPlayerId(), quickBetNames_[tag]);
        return;
    }

    if (stage_ == Stage::AwaitingPick && type_ == OperationType::AnimationSpeed)
    {
        if (accepted && tag < Ctrl::AllAnimationSpeeds.size())
        {
            CuiSender::instance().reqSetAnimationSpeed(
                Ctrl::AllAnimationSpeeds[tag]);
        }
        finish();
        return;
    }

    if (stage_ == Stage::AwaitingPick && type_ == OperationType::RollHistoryFormat)
    {
        if (accepted && tag < Ctrl::AllRollHistoryFormats.size())
        {
            CuiSender::instance().reqSetRollHistoryFormat(
                Ctrl::AllRollHistoryFormats[tag]);
        }
        finish();
        return;
    }

    if (stage_ != Stage::AwaitingPick || type_ != OperationType::Player)
        return;

    if (!accepted)       // Cancelled, or just looking at a list
    {
        finish();
        return;
    }

    targetId_ = playerId;
    targetName_.clear();
    for (const auto& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (p.playerId == playerId) targetName_ = p.name;
    }

    switch (playerOp_)
    {
    case PlayerOp::Switch:
    {
        unsigned numBets = 0;
        Gen::ErrorPass ep;
        (void) Ctrl::CrapsReaders::readPlayerNumBetsOnTable(
                   Ctrl::CrapsReaders::getUserPlayerId(), numBets, ep);
        if (numBets > 0)
        {
            askConfirmation("Switch to " + targetName_ + "? Your " +
                            std::to_string(numBets) +
                            (numBets == 1 ? " bet is" : " bets are") +
                            " returned");
        }
        else
        {
            sendPlayerRequest();
        }
        break;
    }
    case PlayerOp::Delete:
        askConfirmation("Delete player " + targetName_ + "?");
        break;

    case PlayerOp::BuddyJoins:
    case PlayerOp::BuddyLeaves:
        sendPlayerRequest();
        break;

    default:
        finish();     // Not expected: other operations don't pick
        break;
    }
}

//----------------------------------------------------------------

void
OperationManager::onTextResult(bool accepted, const std::string& text)
{
    if (stage_ == Stage::AwaitingText && type_ == OperationType::QuickBetsEdit)
    {
        if (!accepted)
        {
            finish();
            return;
        }
        quickBetTarget_ = trimmed(text);
        sendQuickBetsEdit(Ctrl::ReqEditQuickBets::Action::Create);
        return;
    }

    if (stage_ == Stage::AwaitingText && type_ == OperationType::QuickBetsManage)
    {
        if (!accepted)
        {
            finish();
            return;
        }
        sendManageRequest(trimmed(text));
        return;
    }

    if (stage_ == Stage::AwaitingText && type_ == OperationType::AutoOdds)
    {
        if (!accepted)
        {
            finish();
            return;
        }
        Ctrl::ReqEditAutoOdds req;
        req.betName  = autoBetName_;
        req.point    = autoPoint_;
        Ctrl::AutoOdds::Setting setting;
        Ctrl::AutoOdds::parseSetting(text, setting);    // Validated already
        req.multiple = setting.multiple;
        req.amount   = setting.amount;
        req.action   = Ctrl::ReqEditAutoOdds::Action::Set;
        sendAutoRequest(std::move(req));
        return;
    }

    if (stage_ != Stage::AwaitingText || type_ != OperationType::Player)
        return;

    if (!accepted)
    {
        finish();
        return;
    }
    if (playerOp_ == PlayerOp::Create && !askingDescription_)
    {
        text_ = trimmed(text);
        askForDescription();     // Second question; the first is answered
        return;
    }
    if (playerOp_ == PlayerOp::Create)
    {
        description_ = trimmed(text);
    }
    else
    {
        text_ = trimmed(text);
    }
    sendPlayerRequest();
}

//----------------------------------------------------------------
//
// Creating a player: after the name, an optional short description.
// Esc here cancels the whole creation.
//
void
OperationManager::askForDescription()
{
    askingDescription_ = true;

    auto validator = [](const std::string& raw) -> std::string
    {
        std::string why;
        if (!Ctrl::PlayerManager::checkDescription(trimmed(raw), why))
        {
            return why;
        }
        return std::string();       // Empty is fine
    };

    stage_ = Stage::AwaitingText;
    DialogTextInput::instance().configure(
        "Describe " + text_ + " (optional)", "About: ", "",
        Ctrl::PlayerManager::MaxDescriptionLength, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::askConfirmation(const std::string& question)
{
    stage_ = Stage::AwaitingConfirmation;
    DialogConfirm::instance().configure(question);
    SurfaceManager::instance().pushSurface(&DialogConfirm::instance());
}

//----------------------------------------------------------------

void
OperationManager::sendPlayerRequest()
{
    stage_ = Stage::AwaitingResponse;
    auto& sender = CuiSender::instance();

    switch (playerOp_)
    {
    case PlayerOp::Switch:
        correlationId_ = sender.reqSwitchPlayer(targetId_);
        break;
    case PlayerOp::Rename:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::Rename, targetId_, text_);
        break;
    case PlayerOp::Create:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::Create, Craps::PlayerId(), text_, description_);
        break;
    case PlayerOp::Delete:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::Delete, targetId_, std::string());
        break;
    case PlayerOp::BuddyJoins:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::BuddyJoins, targetId_, std::string());
        break;
    case PlayerOp::BuddyLeaves:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::BuddyLeaves, targetId_, std::string());
        break;
    default:
        LOG_ERROR("OperationManager::sendPlayerRequest(): "
                  "operation sends no request");
        finish();
        break;
    }
}

//----------------------------------------------------------------

const char*
OperationManager::playerOpVerb() const
{
    switch (playerOp_)
    {
    case PlayerOp::Switch:      return "Unable to Switch Player";
    case PlayerOp::Rename:      return "Unable to Rename Player";
    case PlayerOp::Create:      return "Unable to Create Player";
    case PlayerOp::Delete:      return "Unable to Delete Player";
    case PlayerOp::BuddyJoins:  return "Unable to Seat Buddy";
    case PlayerOp::BuddyLeaves: return "Unable to Remove Buddy";
    default:                    return "Unable to Complete Operation";
    }
}

//----------------------------------------------------------------

void
OperationManager::onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& ev)
{
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::Player   ||
        playerOp_ != PlayerOp::Switch     ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspSwitchPlayer(): ignoring "
                 "unmatched RspSwitchPlayer");
        return;
    }

    if (ev.switched)
    {
        // Back to the table, now played by the new user player.
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }
    showError(ev.error, "Player Error", playerOpVerb());
}

//----------------------------------------------------------------

void
OperationManager::onRspManagePlayer(const Ctrl::RspManagePlayer& ev)
{
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::Player   ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspManagePlayer(): ignoring "
                 "unmatched RspManagePlayer");
        return;
    }

    if (!ev.ok)
    {
        showError(ev.error, "Player Error", playerOpVerb());
        return;
    }

    const PlayerOp op = playerOp_;
    finish();

    if (op == PlayerOp::Create)
    {
        showNotice("Created player " + ev.name,
                   "Use Switch to Player to play as " + ev.name + ".");
    }
    else if (op == PlayerOp::Delete)
    {
        showNotice("Deleted player " + targetName_,
                   "The player file was moved to the trash folder inside "
                   "the players folder.");
    }
}

//----------------------------------------------------------------

void
OperationManager::showNotice(const std::string& brief,
                             const std::string& detail)
{
    DialogMessage::MessageText m;
    m.label  = DialogMessage::MessageLabel::Info;
    m.brief  = brief;
    m.detail = detail;
    DialogMessage::instance().configure(m);
    SurfaceManager::instance().pushSurface(&DialogMessage::instance());
}

//----------------------------------------------------------------
