//----------------------------------------------------------------
//
// File: BetOperations.cpp
//
//----------------------------------------------------------------

#include <cui/operations/BetOperations.h>
#include <cui/core/CuiSender.h>
#include <cui/managers/CarrierBet.h>
#include <controller/CrapsReaders.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <gen/Logger.h>
#include <algorithm>
#include <functional>

using namespace Cui;

//----------------------------------------------------------------
//
// Make Bet
//

void
MakeBetOperation::start()
{
    const auto& cb = CarrierBet::instance();

    Ctrl::CorrelationId id;
    if (cb.isOddsBet())
    {
        id = CuiSender::instance().reqOddsBet(
                 cb.getPlayerId(), cb.getBetId(), cb.getOddsAmount());
    }
    else
    {
        id = CuiSender::instance().reqMakeBet(
                 cb.getPlayerId(), cb.getBetName(),
                 cb.getContractAmount(), cb.getPivot());
    }
    LOG_TRACE("MakeBetOperation::start(): request correlationId = " +
              std::to_string(id));
    awaitResponse(id);
}

//----------------------------------------------------------------

void
MakeBetOperation::onRspMakeBet(const Ctrl::RspMakeBet& rsp)
{
    onResult(rsp.accepted, rsp.error);
}

//----------------------------------------------------------------

void
MakeBetOperation::onRspOddsBet(const Ctrl::RspOddsBet& rsp)
{
    onResult(rsp.accepted, rsp.error);
}

//----------------------------------------------------------------

void
MakeBetOperation::onResult(bool accepted, const Gen::ErrorPass& error)
{
    if (accepted) finishAtRoot();    // Unwind the betting menus
    else          showError(error);
}

//----------------------------------------------------------------
//
// Undo Bet
//

void
UndoBetOperation::start()
{
    awaitResponse(CuiSender::instance().reqUndoBet(
        Ctrl::CrapsReaders::getUserPlayerId()));
}

//----------------------------------------------------------------

void
UndoBetOperation::onRspUndoBet(const Ctrl::RspUndoBet& rsp)
{
    if (rsp.accepted) finishAtRoot();
    else showError(rsp.error, "Betting Error", "Unable to Undo");
}

//----------------------------------------------------------------
//
// Remove Bets: building the list
//

namespace {

std::string
money(Gen::Money m)
{
    return Gen::MoneyUtils::toStringNoCommas(m);
}

} // namespace

//----------------------------------------------------------------

void
RemoveBetOperation::start()
{
    using Reader = Ctrl::CrapsReaders;
    playerId_ = Reader::getUserPlayerId();

    Gen::ErrorPass ep;
    Craps::BetIds betIds;
    (void) Reader::readPlayerGetBets(playerId_, betIds, ep);

    // Newest first; ids grow as bets are made
    std::sort(betIds.begin(), betIds.end(), std::greater<Craps::BetId>());

    std::vector<DialogPicker::Item> items;
    Tally tally;
    for (auto betId : betIds)
    {
        addEntries(betId, items, tally);
    }

    if (items.empty())
    {
        showNothingToRemove(tally);
        finish();
        return;
    }

    addAllEntry(items, tally);
    pickFrom("Remove Bets", std::move(items));
}

//----------------------------------------------------------------
//
// One bet makes up to two entries: the whole bet, if the table lets it
// come down, and the odds alone, if it has any. Each entry's tag is its
// index in choices_.
//
void
RemoveBetOperation::addEntries(Craps::BetId betId,
                               std::vector<DialogPicker::Item>& items,
                               Tally& tally)
{
    using Reader = Ctrl::CrapsReaders;

    Gen::ErrorPass ep;
    bool       removable = false;
    BetName    betName   = BetName::Invalid;
    unsigned   pivot     = 0;
    Gen::Money contract  = 0;
    Gen::Money odds      = 0;

    if (Reader::readBetIsRemovable(betId, removable, ep) !=
            Gen::ReturnCode::Success ||
        Reader::readBetName(betId, betName, ep) !=
            Gen::ReturnCode::Success ||
        Reader::readBetPivot(betId, pivot, ep) !=
            Gen::ReturnCode::Success ||
        Reader::readBetContractAmount(betId, contract, ep) !=
            Gen::ReturnCode::Success ||
        Reader::readBetOddsAmount(betId, odds, ep) !=
            Gen::ReturnCode::Success)
    {
        return;     // Bet went away while we looked
    }

    const std::string name = EnumBetName::toString(betName) +
        (pivot != 0 ? " " + std::to_string(pivot) : std::string());

    if (removable)
    {
        DialogPicker::Item item;
        item.playerId    = playerId_;
        item.betId       = betId;
        item.tag         = choices_.size();
        item.text        = name;
        item.note        = money(contract);
        item.description = odds > 0 ? "odds " + money(odds) : "";
        items.push_back(std::move(item));
        choices_.push_back({betId, false});
        ++tally.numBets;
    }
    else
    {
        ++tally.numLocked;
    }

    if (odds > 0)
    {
        DialogPicker::Item item;
        item.playerId    = playerId_;
        item.betId       = betId;
        item.tag         = choices_.size();
        item.text        = name + " odds only";
        item.note        = money(odds);
        item.description = "keeps " + money(contract) + " bet";
        items.push_back(std::move(item));
        choices_.push_back({betId, true});
        if (!removable) ++tally.numOddsOnly;
    }
}

//----------------------------------------------------------------
//
// "All removable" goes last, so that pressing Enter on the first line
// can't do it by accident. Its tag is one past the end of choices_.
//
void
RemoveBetOperation::addAllEntry(std::vector<DialogPicker::Item>& items,
                                const Tally& tally)
{
    if (tally.numBets + tally.numOddsOnly <= 1) return;

    DialogPicker::Item item;
    item.playerId = playerId_;
    item.tag      = choices_.size();
    item.text     = "All removable";

    std::string d = std::to_string(tally.numBets) +
                    (tally.numBets == 1 ? " bet" : " bets");
    if (tally.numOddsOnly > 0)
    {
        d += ", odds on " + std::to_string(tally.numOddsOnly);
    }
    item.description = d;
    items.push_back(std::move(item));
}

//----------------------------------------------------------------

void
RemoveBetOperation::showNothingToRemove(const Tally& tally)
{
    if (tally.numLocked > 0)
    {
        showNotice("No bets can be removed",
                   "Pass Line and Come bets must stay on the table "
                   "once their point is established.");
    }
    else
    {
        showNotice("No bets to remove", "You have no bets on the table.");
    }
}

//----------------------------------------------------------------
//
// Remove Bets: doing what was picked
//

void
RemoveBetOperation::onPick(bool accepted, const Craps::PlayerId&,
                           Craps::BetId, std::size_t tag)
{
    if (!accepted || tag > choices_.size())
    {
        finish();       // Cancelled; stay on the betting menu
        return;
    }

    if (tag == choices_.size())
    {
        queue_ = onePerBet(choices_);      // "All removable"
    }
    else
    {
        queue_.assign(1, choices_[tag]);
    }
    sendNext();
}

//----------------------------------------------------------------
//
// The choices hold both "whole bet" and "odds only" for a bet that has
// odds and may come down. When everything is removed, one removal for
// each bet is enough: the whole bet.
//
std::vector<RemoveBetOperation::Removal>
RemoveBetOperation::onePerBet(const std::vector<Removal>& choices)
{
    std::vector<Removal> unique;
    for (const auto& r : choices)
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
    return unique;
}

//----------------------------------------------------------------
//
// Send the next queued removal. An odds-only removal is a request to
// set the odds amount to zero, which the table accepts as taking down
// the odds. One request at a time; the response sends the next.
//
void
RemoveBetOperation::sendNext()
{
    const Removal r = queue_.front();
    queue_.erase(queue_.begin());

    // Taking down several bets at once is one step for undo
    const bool joinsPrevious = started_;
    started_ = true;

    awaitResponse(r.oddsOnly
        ? CuiSender::instance().reqOddsBet(playerId_, r.betId, 0, joinsPrevious)
        : CuiSender::instance().reqRemoveBet(playerId_, r.betId, joinsPrevious));
}

//----------------------------------------------------------------

void
RemoveBetOperation::onRspRemoveBet(const Ctrl::RspRemoveBet& rsp)
{
    onResult(rsp.accepted, rsp.error);
}

//----------------------------------------------------------------

void
RemoveBetOperation::onRspOddsBet(const Ctrl::RspOddsBet& rsp)
{
    onResult(rsp.accepted, rsp.error);     // Odds taken down
}

//----------------------------------------------------------------

void
RemoveBetOperation::onResult(bool accepted, const Gen::ErrorPass& error)
{
    if (accepted)
    {
        if (!queue_.empty()) sendNext();
        else                 finishAtRoot();
        return;
    }

    queue_.clear();     // Stop at the first failure
    showError(error, "Betting Error", "Unable to Remove Bet");
}

//----------------------------------------------------------------
