//----------------------------------------------------------------
//
// File: UndoBet.cpp
//
//----------------------------------------------------------------

#include <controller/UndoBet.h>
#include <craps/Player.h>
#include <craps/CrapsBet.h>
#include <gen/ErrorPass.h>

using namespace Ctrl;

//----------------------------------------------------------------

UndoBetAdded::UndoBetAdded(Craps::BetPtr pBet)
{
    pBet_ = pBet;
}

//----------------------------------------------------------------

void
UndoBetAdded::undo()
{
    // We're undoing a bet that was added.
    // We use removeBet() to undo it.
    Gen::ErrorPass ep;
    (void) pBet_->player().removeBet(pBet_->betId(), ep);
}

//----------------------------------------------------------------

UndoBetRemoved::UndoBetRemoved(Craps::BetPtr pBet)
{
    pBet_ = pBet;
}

//----------------------------------------------------------------

void
UndoBetRemoved::undo()
{
    // We're undoing a bet that was removed.
    // We add it back in using restoreBet() pathway.
    Gen::ErrorPass ep;
    pBet_->player().restoreBet(pBet_, ep);
}

//----------------------------------------------------------------

UndoBetModifiedFlags::UndoBetModifiedFlags(Craps::BetPtr pBet)
    : pBet_(pBet)
    , prevState_(*pBet_)
{
}

//----------------------------------------------------------------

void
UndoBetModifiedFlags::undo()
{
    // Just restore the prior bet state
    *pBet_ = prevState_;
}

//----------------------------------------------------------------

UndoBetModifiedAmount::UndoBetModifiedAmount(Craps::BetPtr pBet)
    : pBet_(pBet)
    , prevState_(*pBet_)
{
}

//----------------------------------------------------------------

void
UndoBetModifiedAmount::undo()
{
    // We're undoing a change amount that was made to a bet.
    pBet_->player().restoreAmounts(pBet_, prevState_);
    // For all cases we just restore the prior bet state
    *pBet_ = prevState_;
     
}

//----------------------------------------------------------------
