//----------------------------------------------------------------
//
// File: UndoBet.cpp
//
//----------------------------------------------------------------

#include <controller/BetFlags.h>
#include <controller/UndoBet.h>
#include <craps/EnumBetName.h>
#include <craps/Player.h>
#include <gen/MoneyUtils.h>

using namespace Ctrl;

namespace {

//----------------------------------------------------------------
//
// "$10 Place 6 bet", "$10 PassLine bet with $20 odds"
//
std::string
betText(const Craps::CrapsBet& bet)
{
    std::string s = Gen::MoneyUtils::toString(bet.contractAmount()) + " " +
                    EnumBetName::toString(bet.betName());
    if (bet.pivot() != 0)
    {
        s += " " + std::to_string(bet.pivot());
    }
    s += " bet";
    if (bet.oddsAmount() > 0)
    {
        s += " with " + Gen::MoneyUtils::toString(bet.oddsAmount()) + " odds";
    }
    return s;
}

} // namespace

//----------------------------------------------------------------

UndoBetAdded::UndoBetAdded(Craps::BetPtr pBet)
    : pBet_(std::move(pBet))
{
}

//----------------------------------------------------------------

Gen::ReturnCode
UndoBetAdded::undo(Gen::ErrorPass& ep)
{
    // The bet was added, so take it down the normal way. That returns
    // its money and sends the events that clear it from the screens.
    auto result = pBet_->player().removeBet(pBet_->betId());
    if (!result)
    {
        ep = std::move(result).error();
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

std::string
UndoBetAdded::describe() const
{
    return "Took down your " + betText(*pBet_);
}

//----------------------------------------------------------------

UndoBetRemoved::UndoBetRemoved(Craps::BetPtr pBet)
    : pBet_(std::move(pBet))
{
}

//----------------------------------------------------------------

Gen::ReturnCode
UndoBetRemoved::undo(Gen::ErrorPass& ep)
{
    return pBet_->player().restoreBet(pBet_, ep);
}

//----------------------------------------------------------------

std::string
UndoBetRemoved::describe() const
{
    return "Put back your " + betText(*pBet_);
}

//----------------------------------------------------------------

UndoBetModifiedAmount::UndoBetModifiedAmount(Craps::BetPtr pBet)
    : pBet_(std::move(pBet))
    , prevState_(*pBet_)
{
}

//----------------------------------------------------------------

Gen::ReturnCode
UndoBetModifiedAmount::undo(Gen::ErrorPass& ep)
{
    return pBet_->player().restoreAmounts(pBet_, prevState_, ep);
}

//----------------------------------------------------------------

std::string
UndoBetModifiedAmount::describe() const
{
    return "Changed your bet back to " + betText(prevState_);
}

//----------------------------------------------------------------

UndoBetFlagsChanged::UndoBetFlagsChanged(Craps::BetPtr pBet)
    : pBet_(std::move(pBet))
    , standing_(pBet_->flagStanding())
    , oneRoll_(pBet_->flagOneRoll())
{
}

//----------------------------------------------------------------

Gen::ReturnCode
UndoBetFlagsChanged::undo(Gen::ErrorPass& ep)
{
    return pBet_->player().setBetFlags(pBet_, standing_, oneRoll_, ep);
}

//----------------------------------------------------------------

std::string
UndoBetFlagsChanged::describe() const
{
    return "Put back the flags of your " + BetFlags::betLabel(*pBet_) + " bet";
}

//----------------------------------------------------------------

void
UndoGroup::add(std::unique_ptr<UndoBet> change)
{
    changes_.push_back(std::move(change));
}

//----------------------------------------------------------------
//
// Last change first. Stops at the first one that fails. The ones undone
// before it stay undone; the caller reports the failure.
//
Gen::ReturnCode
UndoGroup::undo(Gen::ErrorPass& ep)
{
    while (!changes_.empty())
    {
        if (changes_.back()->undo(ep) == Gen::ReturnCode::Fail)
        {
            return Gen::ReturnCode::Fail;
        }
        changes_.pop_back();
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

std::string
UndoGroup::describe() const
{
    if (changes_.size() == 1)
    {
        return changes_.front()->describe();
    }
    return "Undid " + std::to_string(changes_.size()) + " bet changes";
}

//----------------------------------------------------------------
