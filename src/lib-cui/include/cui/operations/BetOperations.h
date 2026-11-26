//----------------------------------------------------------------
//
// File: BetOperations.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>
#include <gen/ErrorPass.h>
#include <vector>

namespace Cui {

/// Make a bet, or put odds on one, from what is in the CarrierBet. One
/// request. On success the betting menus unwind to the table.
class MakeBetOperation : public Operation
{
public:
    void start() override;
    void onRspMakeBet(const Ctrl::RspMakeBet& rsp) override;
    void onRspOddsBet(const Ctrl::RspOddsBet& rsp) override;

private:
    void onResult(bool accepted, const Gen::ErrorPass& error);
};

/// Take back the player's latest bet change. One request. The bet and
/// balance events that follow redraw the screens as for any bet change.
class UndoBetOperation : public Operation
{
public:
    void start() override;
    void onRspUndoBet(const Ctrl::RspUndoBet& rsp) override;
};

/// Lists what can come down from the user player's bets and lets the
/// person pick one entry, or all of them.
///
///   - A bet the table allows to be removed: "remove the whole bet".
///   - A bet with odds: "remove the odds only", leaving the contract
///     bet. That is the only way to take down the odds on a Pass Line or
///     Come bet once its point is set, because those contract bets must
///     stay.
///
/// What is picked is sent as one request for each bet, one at a time.
/// Each answer sends the next. The first failure stops the rest.
class RemoveBetOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
    void onRspRemoveBet(const Ctrl::RspRemoveBet& rsp) override;
    void onRspOddsBet(const Ctrl::RspOddsBet& rsp) override;

private:
    /// One thing the person can take down
    struct Removal
    {
        Craps::BetId betId    = 0;
        bool         oddsOnly = false;   // Contract bet stays
    };

    /// What the list holds, to word the "All removable" line and the
    /// message when the list is empty
    struct Tally
    {
        std::size_t numBets     = 0;
        std::size_t numOddsOnly = 0;
        std::size_t numLocked   = 0;   // Cannot come down, no odds to remove
    };

    // Building the list
    void addEntries(Craps::BetId betId,
                    std::vector<DialogPicker::Item>& items, Tally& tally);
    void addAllEntry(std::vector<DialogPicker::Item>& items,
                     const Tally& tally);
    void showNothingToRemove(const Tally& tally);

    // Doing what was picked
    static std::vector<Removal> onePerBet(const std::vector<Removal>& choices);
    void sendNext();
    void onResult(bool accepted, const Gen::ErrorPass& error);

    Craps::PlayerId      playerId_;
    std::vector<Removal> choices_;      // What the list offered, by tag
    std::vector<Removal> queue_;        // Still to send
    bool                 started_ = false;   // Later removals join the first's undo step
};

} // namespace Cui

//----------------------------------------------------------------
