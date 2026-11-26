//----------------------------------------------------------------
//
// File: SettingsOperations.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>

namespace Cui {

//
// The settings that are one pick list and one request. Each shows the
// choices, marks the current one, and on a pick sends the new value to
// the Controller. The picker pops itself, which returns to the menu
// that started it. Cancelling changes nothing.
//

/// Pick what the buddy players do when the user player changes tables.
class BuddyFollowOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
};

/// Pick how fast the dice roll animation plays. The setting applies to
/// the rest of this game and is remembered for the next one.
class AnimationSpeedOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
};

/// Pick how the roll history lists the rolls.
class RollHistoryFormatOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
};

} // namespace Cui

//----------------------------------------------------------------
