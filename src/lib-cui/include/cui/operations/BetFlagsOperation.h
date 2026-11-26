//----------------------------------------------------------------
//
// File: BetFlagsOperation.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>
#include <controller/BetFlags.h>

namespace Cui {

/// Bet Flags, from the Bet Flags menu. The scope is what the menu key
/// meant: a category, all bets, or one bet (Scope::Bet).
///
/// A category or all bets goes straight to the list of five changes. One
/// bet first asks which bet. Each list pops itself; backing out of either
/// returns to the Bet Flags menu, which is still on the stack. The change
/// is one request. See "Bet Flags" in the design document.
class BetFlagsOperation : public Operation
{
public:
    explicit BetFlagsOperation(Ctrl::BetFlags::Scope scope) : scope_(scope) {}

    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
    void onRspSetBetFlags(const Ctrl::RspSetBetFlags& rsp) override;

private:
    enum class Step { PickBet, PickChange };

    void pickBet();
    void pickChange();
    std::string changeListTitle() const;
    void sendChange(Ctrl::BetFlags::Change change);

    const Ctrl::BetFlags::Scope scope_;
    Step         step_  = Step::PickChange;
    Craps::BetId betId_ = 0;       // The one bet, for Scope::Bet
};

} // namespace Cui

//----------------------------------------------------------------
