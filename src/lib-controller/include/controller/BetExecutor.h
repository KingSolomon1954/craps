//----------------------------------------------------------------
//
// File: BetExecutor.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/BetSpec.h>
#include <controller/LastFill.h>
#include <controller/UndoManager.h>
#include <craps/Player.h>
#include <cstddef>
#include <string>
#include <vector>

namespace Ctrl {

class BetExecutor
{
public:
    enum class Status
    {
        Placed,           ///< A new bet is on the table
        Changed,          ///< The bet was there; its amount changed
        AlreadyInPlace,   ///< The bet was there with this amount
        Skipped           ///< Not placed; see the reason
    };

    struct Outcome
    {
        BetSpec     spec;
        Status      status = Status::Skipped;
        std::string reason;     ///< Why it was skipped; empty otherwise
    };

    struct Result
    {
        std::vector<Outcome> outcomes;   ///< In the order they were applied

        std::size_t count(Status status) const;
        bool        anyApplied() const;  ///< Something placed or changed
        std::string headline() const;    ///< "3 placed, 1 skipped"
        std::string skippedDetail() const; ///< One line per skipped bet
    };

    /// Applies the specs for the player, best effort: a spec that cannot
    /// be placed is skipped and the rest go on. Contract bets are applied
    /// before odds bets, whatever the order given. Everything applied is
    /// one step for undo. lastFill, if given, is updated as for a bet the
    /// player placed by hand.
    static Result apply(Craps::Player& player,
                        const std::vector<BetSpec>& specs,
                        UndoManager& undo,
                        LastFill* lastFill = nullptr);
};

/*-----------------------------------------------------------*//**

@class BetExecutor

@brief Turns a list of bet specs into bets for one player

The one place that applies QuickBets, and later AutoOdds and buddy
automation. Each placement uses Player::makeOrChangeBet() and
Player::setOddsAmount(), the same as a bet request from the View, so
the table's rules, limits and funds checks apply unchanged and the
usual bet and balance events follow.

A player has one bet per spot, so a spec for a spot that is occupied
sets that bet's amount. A spec whose amount is already there changes
nothing and is not recorded for undo. All the changes made by one call
are a single undo step.

The caller checks that betting is open; if it is not, every spec is
refused by the table and comes back skipped with the table's reason.

Not a singleton and holds no state. Tests give it a Player directly.

*/

} // namespace Ctrl

//----------------------------------------------------------------
