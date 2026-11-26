//----------------------------------------------------------------
//
// File: BetFlagsOperation.cpp
//
//----------------------------------------------------------------

#include <cui/operations/BetFlagsOperation.h>
#include <cui/core/CuiSender.h>
#include <cui/managers/SurfaceManager.h>
#include <controller/CrapsReaders.h>
#include <cctype>

using namespace Cui;

namespace {

// "odds bets" -> "Odds Bets"
std::string
capitalizeWords(std::string text)
{
    bool start = true;
    for (auto& ch : text)
    {
        if (start) ch = static_cast<char>(
            std::toupper(static_cast<unsigned char>(ch)));
        start = (ch == ' ');
    }
    return text;
}

} // namespace

//----------------------------------------------------------------

void
BetFlagsOperation::start()
{
    if (scope_ == Ctrl::BetFlags::Scope::Bet) pickBet();
    else                                      pickChange();
}

//----------------------------------------------------------------

void
BetFlagsOperation::pickBet()
{
    const auto flagBets = Ctrl::CrapsReaders::readFlagBets();
    if (flagBets.empty())
    {
        showNotice("No Bets to Flag",
                   "You have no bets that can be turned off or on.");
        finish();
        return;
    }

    std::vector<DialogPicker::Item> items;
    for (const auto& fb : flagBets)
    {
        DialogPicker::Item item;
        item.playerId    = Ctrl::CrapsReaders::getUserPlayerId();
        item.betId       = fb.betId;
        item.tag         = items.size();
        item.text        = fb.label;
        item.description = fb.flags;
        items.push_back(std::move(item));
    }

    step_ = Step::PickBet;
    pickFrom("Bet Flags - Which Bet", std::move(items));
}

//----------------------------------------------------------------
//
// The five changes, in the order of the design document. The tag is the
// Change.
//
void
BetFlagsOperation::pickChange()
{
    using Ctrl::BetFlags::Change;

    const std::string what =
        capitalizeWords(Ctrl::BetFlags::scopeText(scope_));

    const Change changes[] = {
        Change::NotWorking, Change::NotWorkingNextRoll,
        Change::Working,    Change::WorkingNextRoll,
        Change::TableDefault };

    std::vector<DialogPicker::Item> items;
    for (auto change : changes)
    {
        DialogPicker::Item item;
        item.playerId = Ctrl::CrapsReaders::getUserPlayerId();
        item.tag      = static_cast<std::size_t>(change);
        item.text     = "Set " + what +
            (change == Change::TableDefault ? " To " : " ") +
            Ctrl::BetFlags::changeText(change);
        items.push_back(std::move(item));
    }

    step_ = Step::PickChange;
    pickFrom(changeListTitle(), std::move(items));
}

//----------------------------------------------------------------
//
// "Bet Flags - Odds Bets", or for one bet, the bet's own label
//
std::string
BetFlagsOperation::changeListTitle() const
{
    std::string title = "Bet Flags - ";
    if (scope_ == Ctrl::BetFlags::Scope::Bet)
    {
        for (const auto& fb : Ctrl::CrapsReaders::readFlagBets())
        {
            if (fb.betId == betId_) title += fb.label;
        }
    }
    else
    {
        title += capitalizeWords(Ctrl::BetFlags::scopeText(scope_));
    }
    return title;
}

//----------------------------------------------------------------

void
BetFlagsOperation::onPick(bool accepted, const Craps::PlayerId&,
                          Craps::BetId betId, std::size_t tag)
{
    if (!accepted)
    {
        finish();           // Back to the Bet Flags menu
        return;
    }

    if (step_ == Step::PickBet)
    {
        betId_ = betId;     // The bet list hands back the bet id
        pickChange();
        return;
    }

    // The change list hands back the change as the tag
    sendChange(static_cast<Ctrl::BetFlags::Change>(tag));
}

//----------------------------------------------------------------

void
BetFlagsOperation::sendChange(Ctrl::BetFlags::Change change)
{
    awaitResponse(CuiSender::instance().reqSetBetFlags(
        Ctrl::CrapsReaders::getUserPlayerId(), scope_, betId_, change));
}

//----------------------------------------------------------------

void
BetFlagsOperation::onRspSetBetFlags(const Ctrl::RspSetBetFlags& rsp)
{
    if (!rsp.accepted)
    {
        showError(rsp.error, "Betting Error", "Unable to Set Bet Flags");
        return;
    }

    // Unwind to the table as after any bet, then say what was done.
    SurfaceManager::instance().popToRoot();
    showNotice("Bet Flags", rsp.message);
    finish();
}

//----------------------------------------------------------------
