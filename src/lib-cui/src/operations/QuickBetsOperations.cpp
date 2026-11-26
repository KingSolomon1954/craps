//----------------------------------------------------------------
//
// File: QuickBetsOperations.cpp
//
//----------------------------------------------------------------

#include <cui/operations/QuickBetsOperations.h>
#include <cui/core/CuiSender.h>
#include <cui/dialogs/DialogQuickBetsSetAdd.h>
#include <cui/dialogs/DialogTextInput.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/operations/OperationText.h>
#include <controller/CrapsReaders.h>
#include <controller/QuickBets.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <algorithm>

using namespace Cui;

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

// One line for each entry, tagged by its place in names
std::vector<DialogPicker::Item>
entryItems(const std::vector<Ctrl::BetPlan>& plans,
           std::vector<std::string>& names)
{
    names.clear();
    std::vector<DialogPicker::Item> items;
    for (const auto& plan : plans)
    {
        DialogPicker::Item item;
        item.tag         = names.size();
        item.text        = plan.name;
        item.description = Ctrl::describe(plan);
        items.push_back(std::move(item));
        names.push_back(plan.name);
    }
    return items;
}

} // namespace

//----------------------------------------------------------------
//
// Apply QuickBets
//

void
ApplyQuickBetsOperation::start()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    if (plans.empty())
    {
        showNotice("No QuickBets", "You have not set up any QuickBets yet.");
        finish();
        return;
    }
    pickFrom("QuickBets - Apply", entryItems(plans, names_));
}

//----------------------------------------------------------------

void
ApplyQuickBetsOperation::onPick(bool accepted, const Craps::PlayerId&,
                                Craps::BetId, std::size_t tag)
{
    if (!accepted || tag >= names_.size())
    {
        finish();       // Cancelled; stay on the betting menu
        return;
    }
    awaitResponse(CuiSender::instance().reqApplyQuickBets(
        Ctrl::CrapsReaders::getUserPlayerId(), names_[tag]));
}

//----------------------------------------------------------------

void
ApplyQuickBetsOperation::onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& rsp)
{
    if (!rsp.accepted)
    {
        showError(rsp.error, "Betting Error", "Unable to Apply QuickBets");
        return;
    }

    // Unwind to the table as after any bet. Say so if some bets were
    // not placed; a clean result needs no message.
    SurfaceManager::instance().popToRoot();
    if (rsp.skipped > 0)
    {
        showNotice("QuickBets \"" + rsp.name + "\": " + rsp.headline,
                   rsp.skippedDetail);
    }
    finish();
}

//----------------------------------------------------------------
//
// What saving and managing share
//

void
EditQuickBetsOperation::send(Ctrl::ReqEditQuickBets request)
{
    request.playerId = Ctrl::CrapsReaders::getUserPlayerId();
    awaitResponse(CuiSender::instance().reqEditQuickBets(std::move(request)));
}

//----------------------------------------------------------------

void
EditQuickBetsOperation::onRspEditQuickBets(const Ctrl::RspEditQuickBets& rsp)
{
    if (!rsp.accepted)
    {
        showError(rsp.error, "QuickBets Error", "Unable to Save QuickBets");
        return;
    }

    // Stay where we are: the bet amount dialog or the Configure menu
    finish();
    showNotice(doneTitle(), rsp.message);
}

//----------------------------------------------------------------

void
EditQuickBetsOperation::onErrorAcknowledged()
{
    finish();       // Back to where the person was, still on the stack
}

//----------------------------------------------------------------
//
// Save to QuickBets
//

void
SaveToQuickBetsOperation::start()
{
    const auto& cb = CarrierBet::instance();
    spec_.betName = cb.getBetName();
    spec_.isOdds  = cb.isOddsBet();
    spec_.pivot   = cb.getPivot();
    spec_.amount  = spec_.isOdds ? cb.getOddsAmount() : cb.getContractAmount();
    if (spec_.isOdds && Ctrl::isPlanOddsBet(spec_.betName))
    {
        spec_.pivot = 0;          // Line odds sit on the line bet
    }

    if (const auto why = Ctrl::QuickBets::checkSpec(spec_); !why.empty())
    {
        showNotice("Can't save to QuickBets", why);
        finish();
        return;
    }

    // "Place bet 8 $30": what the person is about to save
    std::string betText = EnumBetName::toString(spec_.betName) + " bet";
    if (spec_.pivot != 0) betText += " " + std::to_string(spec_.pivot);
    if (spec_.isOdds)     betText += " odds";
    betText += " " + Gen::MoneyUtils::toString(spec_.amount);

    std::vector<DialogQuickBetsSetAdd::Entry> entries;
    for (const auto& plan : Ctrl::CrapsReaders::readQuickBets())
    {
        entries.push_back({plan.name, Ctrl::describe(plan)});
        names_.push_back(plan.name);
    }

    awaitPick();
    DialogQuickBetsSetAdd::instance().configure(betText, std::move(entries));
    SurfaceManager::instance().pushSurface(&DialogQuickBetsSetAdd::instance());
}

//----------------------------------------------------------------

void
SaveToQuickBetsOperation::onQuickBetsChoice(bool accepted, std::size_t index)
{
    if (!accepted || (index != DialogQuickBetsSetAdd::NewEntry &&
                      index >= names_.size()))
    {
        finish();
        return;
    }

    if (index == DialogQuickBetsSetAdd::NewEntry)
    {
        askForName();
        return;
    }
    send(Ctrl::ReqEditQuickBets::Action::AddBet, names_[index]);
}

//----------------------------------------------------------------
//
// Name for a new QuickBets entry. The dialog says why a name is refused,
// so it can be fixed without leaving.
//
void
SaveToQuickBetsOperation::askForName()
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

    askText("New QuickBets Entry", "Name: ", "",
            Ctrl::QuickBets::MaxNameLength, std::move(validator));
}

//----------------------------------------------------------------

void
SaveToQuickBetsOperation::onText(bool accepted, const std::string& text)
{
    if (!accepted)
    {
        finish();
        return;
    }
    send(Ctrl::ReqEditQuickBets::Action::Create, trimmed(text));
}

//----------------------------------------------------------------

void
SaveToQuickBetsOperation::send(Ctrl::ReqEditQuickBets::Action action,
                               const std::string& name)
{
    Ctrl::ReqEditQuickBets req;
    req.action = action;
    req.name   = name;
    req.spec   = spec_;
    EditQuickBetsOperation::send(std::move(req));
}

//----------------------------------------------------------------
//
// Manage QuickBets
//

void
ManageQuickBetsOperation::start()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    if (plans.empty())
    {
        showNotice("No QuickBets",
                   "You have not set up any QuickBets yet. While you enter "
                   "a bet, press q to save it into one.");
        finish();
        return;
    }

    step_ = Step::PickEntry;
    pickFrom("QuickBets - Change which entry?", entryItems(plans, names_));
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::onPick(bool accepted, const Craps::PlayerId&,
                                 Craps::BetId, std::size_t tag)
{
    if (!accepted)
    {
        finish();       // Cancelled; stay on the Configure menu
        return;
    }

    switch (step_)
    {
    case Step::PickEntry:  onEntryPicked(tag);  break;
    case Step::PickAction: onActionPicked(tag); break;
    case Step::PickBet:    onBetPicked(tag);    break;
    default:               finish();            break;
    }
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::onEntryPicked(std::size_t tag)
{
    if (tag >= names_.size())
    {
        finish();
        return;
    }
    entry_ = names_[tag];
    pickAction();
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::pickAction()
{
    std::vector<DialogPicker::Item> items;
    auto add = [&](std::size_t tag, const char* text, const char* desc)
    {
        DialogPicker::Item item;
        item.tag         = tag;
        item.text        = text;
        item.description = desc;
        items.push_back(std::move(item));
    };
    add(0, "Rename",           "give the entry another name");
    add(1, "Change an amount", "set a different amount for one bet");
    add(2, "Remove a bet",     "take one bet out of the entry");
    add(3, "Delete",           "delete the whole entry");

    step_ = Step::PickAction;
    pickFrom("QuickBets - " + entry_, std::move(items));
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::onActionPicked(std::size_t tag)
{
    switch (tag)
    {
    case 0:  askName();                    break;
    case 1:  pickBet(Action::SetAmount);   break;
    case 2:  removeABet();                 break;
    case 3:
        step_ = Step::ConfirmDelete;
        askConfirmation("Delete the QuickBets entry " + entry_ + "?");
        break;
    default: finish();                     break;
    }
}

//----------------------------------------------------------------
//
// An entry keeps at least one bet. To lose the last, delete the entry.
//
void
ManageQuickBetsOperation::removeABet()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const auto* plan = findPlan(plans, entry_);
    if (plan != nullptr && plan->specs.size() <= 1)
    {
        finish();
        showNotice("Can't remove the bet",
                   entry_ + " has only one bet. Delete the entry instead.");
        return;
    }
    pickBet(Action::RemoveBet);
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::pickBet(Action action)
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const auto* plan = findPlan(plans, entry_);
    if (plan == nullptr || plan->specs.empty())
    {
        finish();
        showNotice("QuickBets", "That entry is no longer there.");
        return;
    }

    std::vector<DialogPicker::Item> items;
    for (std::size_t i = 0; i < plan->specs.size(); ++i)
    {
        DialogPicker::Item item;
        item.tag  = i;
        item.text = Ctrl::describe(plan->specs[i]);
        items.push_back(std::move(item));
    }

    action_ = action;
    step_   = Step::PickBet;
    pickFrom(action == Action::SetAmount ? "Change the amount of which bet?"
                                         : "Remove which bet?",
             std::move(items));
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::onBetPicked(std::size_t tag)
{
    betIndex_ = tag;
    if (action_ == Action::SetAmount) askAmount();
    else                              sendRequest();
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::askName()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const std::string current = entry_;

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

    action_ = Action::Rename;
    step_   = Step::AskName;
    askText("Rename " + entry_, "Name: ", entry_,
            Ctrl::QuickBets::MaxNameLength, std::move(validator));
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::askAmount()
{
    const auto plans = Ctrl::CrapsReaders::readQuickBets();
    const auto* plan = findPlan(plans, entry_);
    if (plan == nullptr || betIndex_ >= plan->specs.size())
    {
        finish();
        showNotice("QuickBets", "That bet is no longer in the entry.");
        return;
    }
    const Ctrl::BetSpec& spec = plan->specs[betIndex_];

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

    step_ = Step::AskAmount;
    askText("Amount for " + Ctrl::describe(spec), "Amount: $",
            std::to_string(spec.amount), 7, std::move(validator));
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::onText(bool accepted, const std::string& text)
{
    if (!accepted)
    {
        finish();
        return;
    }
    sendRequest(trimmed(text));     // The new name, or the amount typed
}

//----------------------------------------------------------------

void
ManageQuickBetsOperation::onConfirm(SurfaceBase::OperationResult result)
{
    if (result != SurfaceBase::OperationResult::Yes)
    {
        finish();
        return;
    }
    action_ = Action::Delete;
    sendRequest();
}

//----------------------------------------------------------------
//
// Sends the request for the action chosen. text is the new name or the
// typed amount, when the action has one.
//
void
ManageQuickBetsOperation::sendRequest(const std::string& text)
{
    Ctrl::ReqEditQuickBets req;
    req.action = action_;
    req.name   = entry_;
    req.index  = betIndex_;
    if (action_ == Action::Rename)
    {
        req.newName = text;
    }
    else if (action_ == Action::SetAmount)
    {
        req.amount = static_cast<Gen::Money>(std::stoul(text));
    }
    send(std::move(req));
}

//----------------------------------------------------------------
