//----------------------------------------------------------------
//
// File: QuickBet.cpp
//
//----------------------------------------------------------------

#include <controller/QuickBet.h>
#include <controller/CrapsCommands.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <craps/Player.h>
#include <cassert>

using namespace Ctrl;

//----------------------------------------------------------------

QuickBet* QuickBet::instance()
{
    static QuickBet quickBet;
    return &quickBet;
}

//----------------------------------------------------------------

void
QuickBet::applyQuickBet(size_t index)
{
    Gen::ErrorPass ep;
    Craps::BetId betId;
    Gen::ReturnCode rc;
    
    auto* pPlayer = Gbl::pPlayerMgr->getUserPlayer();
    assert(pPlayer != nullptr);
    
    if (index >= bets_.size()) return;  // probably should assert()
    auto& qbe = bets_[index];
    if (qbe.isOddsBet)
    {
        betId = pPlayer->getBet(qbe.betName, qbe.pivot, ep);
        if (betId == 0)
        {
            // TODO handle error
        }
        // Go through CrapsCommands so bet is logged and undoable.
        rc = CrapsCommands::cmdBetSetOddsAmount(betId, qbe.amount, ep);
        if (rc == Gen::ReturnCode::Fail)
        {
            // TODO handle error
        }
    }
    else
    {
        // Go through CrapsInterface so bet is logged and undoable.
        auto correlationId = CrapsCommands::cmdMakeBet(
            pPlayer->getPlayerId(),
            qbe.betName,
            qbe.amount,
            qbe.pivot);

            // TODO
            // pOwning_->onBetFailed(playerId, ep.diag);
            // showDialogAckError(ep.diag);
    }
}

//----------------------------------------------------------------

void
QuickBet::setQuickBet(const QuickBetEntry& qbe)
{
    // Look for an existing entry with same betName, pivot, and isOddsBet
    auto it = std::find_if(bets_.begin(), bets_.end(),
        [&](const QuickBetEntry& existing) {
            return existing.betName   == qbe.betName
                && existing.pivot     == qbe.pivot
                && existing.isOddsBet == qbe.isOddsBet;
        });

    if (it != bets_.end())
    {
        it->amount = qbe.amount;  // Replace amount only
    }
    else
    {
        bets_.push_back(qbe);    // No match: add new entry
    }
}

//----------------------------------------------------------------

void
QuickBet::deleteQuickBet(const QuickBetEntry& qbe)
{
    auto it = std::find_if(bets_.begin(), bets_.end(),
        [&](const QuickBetEntry& existing) {
            return existing.betName   == qbe.betName
                && existing.pivot     == qbe.pivot
                && existing.isOddsBet == qbe.isOddsBet;
        });

    if (it != bets_.end())
    {
        // Erase compacts the vector automatically (no "holes")
        bets_.erase(it);
    }
}

//----------------------------------------------------------------

void
QuickBet::deleteQuickBet(size_t index)
{
    if (index < bets_.size())
    {
        bets_.erase(bets_.begin() + index);
    }
}

//----------------------------------------------------------------

const QuickBet::Bets&
QuickBet::getQuickBets()
{
    return bets_;
}

//----------------------------------------------------------------
