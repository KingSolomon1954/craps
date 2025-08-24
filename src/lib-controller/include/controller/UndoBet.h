//----------------------------------------------------------------
//
// File: UndoBet.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/CrapsTypes.h>
#include <craps/CrapsBet.h>

namespace Ctrl {

class UndoBet
{
public:
    virtual void undo() = 0;
    virtual ~UndoBet() = default;
};

class UndoBetAdded : public UndoBet
{
public:
    UndoBetAdded(Craps::BetPtr pBet);
    void undo() override;
    
private:
    Craps::BetPtr pBet_ = nullptr;
};

class UndoBetRemoved : public UndoBet
{
public:
    UndoBetRemoved(Craps::BetPtr pBet);
    void undo() override;
    
private:
    Craps::BetPtr pBet_ = nullptr;
};

class UndoBetModifiedFlags : public UndoBet
{
public:
    UndoBetModifiedFlags(Craps::BetPtr pBet);
    void undo() override;
    
private:
    Craps::BetPtr pBet_ = nullptr;
    Craps::CrapsBet prevState_;
};

class UndoBetModifiedAmount : public UndoBet
{
public:
    UndoBetModifiedAmount(Craps::BetPtr pBet);
    void undo() override;
    
private:
    Craps::BetPtr pBet_ = nullptr;
    Craps::CrapsBet prevState_;
};

/*-----------------------------------------------------------*//**

@class UndoBet

@brief Base class for undo bet capability

Remembers previous bet before change so it can be
restored.

Undo Capability

Supported (stack-based undo, limited to bets)

@li Scope: only bet state (no configuration, no gameplay state, no rolls).
@li Granularity: one atomic bet change = one undo step.
@li Kinds of changes:

  @li new bet placed
  @li existing bet modified (amount, odds, flags)
  @li bet removed

@li Ordering: last-in, first-out (classic undo stack).
@li Lifetime: undo history is cleared after each roll (so you can only undo the current betting decisions before the dice are rolled).

Not supported (explicit exclusions)

@li Undoing out of order changes (no random-access undo).
@li Redo.
@li Anything outside of bets (like table config, game options).

*/

} // namespace Ctrl

//----------------------------------------------------------------
