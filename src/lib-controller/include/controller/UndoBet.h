//----------------------------------------------------------------
//
// File: UndoBet.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/CrapsTypes.h>
#include <craps/CrapsBet.h>
#include <gen/ErrorPass.h>
#include <memory>
#include <string>
#include <vector>

namespace Ctrl {

class UndoBet
{
public:
    virtual ~UndoBet() = default;

    /// Puts things back as they were before the change. Success, or Fail
    /// and ep has the reason. A failure changes nothing.
    virtual Gen::ReturnCode undo(Gen::ErrorPass& ep) = 0;

    /// What undoing does, in words, e.g. "Took down your $10 Place 6 bet"
    virtual std::string describe() const = 0;
};

//----------------------------------------------------------------

/// A bet was made. Undo takes it down.
class UndoBetAdded : public UndoBet
{
public:
    explicit UndoBetAdded(Craps::BetPtr pBet);
    Gen::ReturnCode undo(Gen::ErrorPass& ep) override;
    std::string describe() const override;

private:
    Craps::BetPtr pBet_;
};

//----------------------------------------------------------------

/// A bet was taken down. Undo puts it back with its amounts.
class UndoBetRemoved : public UndoBet
{
public:
    explicit UndoBetRemoved(Craps::BetPtr pBet);
    Gen::ReturnCode undo(Gen::ErrorPass& ep) override;
    std::string describe() const override;

private:
    Craps::BetPtr pBet_;
};

//----------------------------------------------------------------

/// The contract amount or the odds amount of a bet changed. Undo puts
/// both back. Make this before making the change.
class UndoBetModifiedAmount : public UndoBet
{
public:
    explicit UndoBetModifiedAmount(Craps::BetPtr pBet);
    Gen::ReturnCode undo(Gen::ErrorPass& ep) override;
    std::string describe() const override;

private:
    Craps::BetPtr pBet_;
    Craps::CrapsBet prevState_;
};

//----------------------------------------------------------------

/// Several changes that are one step to the player, such as taking down
/// all removable bets. Undone together, last change first.
class UndoGroup : public UndoBet
{
public:
    void add(std::unique_ptr<UndoBet> change);
    std::size_t size() const { return changes_.size(); }
    Gen::ReturnCode undo(Gen::ErrorPass& ep) override;
    std::string describe() const override;

private:
    std::vector<std::unique_ptr<UndoBet>> changes_;
};

/*-----------------------------------------------------------*//**

@class UndoBet

@brief One undoable change to a bet, and how to take it back

Each remembers what is needed to put things back and does that through
the same Player and table paths as any other change, so the money,
the house bank, the player's bets and the events that update the screens
all follow. Undo does not repeat the rules for making or changing a bet:
the change was allowed once and undo restores the earlier, allowed,
state. It only needs betting to be open and the money to be there.

@li UndoBetAdded:          a new bet was placed
@li UndoBetModifiedAmount: contract or odds amount changed
@li UndoBetRemoved:        a bet was taken down
@li UndoGroup:             several of these as one step

See UndoManager for the stack, who owns it and when it is cleared.

*/

} // namespace Ctrl

//----------------------------------------------------------------
