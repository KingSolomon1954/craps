//----------------------------------------------------------------
//
// File: UndoManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <vector>
#include <controller/UndoBet.h>

namespace Ctrl {

class UndoManager
{
public:
    void push(std::unique_ptr<UndoBet> ubet); 
    bool canUndo() const;
    void undo();
    void clear();
    
private:
    std::vector<std::unique_ptr<UndoBet>> stack_;
};

/*-----------------------------------------------------------*//**

@class UndoManager

@brief Manages undo operations on bets

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
