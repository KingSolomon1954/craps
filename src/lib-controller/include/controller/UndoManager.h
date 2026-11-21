//----------------------------------------------------------------
//
// File: UndoManager.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/UndoBet.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace Ctrl {

class UndoManager
{
public:
    /// @name Lifecycle
    /// @{
   ~UndoManager() = default;
    static UndoManager& instance();
    /// @}
    
    /// @name Modifiers
    /// @{

    /// Records a change the player just made. Call after the change
    /// succeeded. With joinPrevious the change becomes part of the step
    /// on top of the stack instead of a new step, so a batch such as
    /// taking down every removable bet is one undo.
    void push(const Craps::PlayerId& playerId,
              std::unique_ptr<UndoBet> change,
              bool joinPrevious = false);

    /// Undoes the player's most recent step. On success description says
    /// what was done. On failure ep has the reason, and since the player's
    /// bets may no longer match what was recorded, that player's whole
    /// history is dropped. Nothing is undone if there is nothing to undo.
    Gen::ReturnCode undo(const Craps::PlayerId& playerId,
                         std::string& description,
                         Gen::ErrorPass& ep);

    void clear();                                  ///< Everybody
    void clear(const Craps::PlayerId& playerId);   ///< One player
    /// @}

    /// @name Observers
    /// @{
    bool        canUndo(const Craps::PlayerId& playerId) const;
    std::size_t depth  (const Craps::PlayerId& playerId) const;
    /// @}
    
private:
    UndoManager() = default;

    using Stack = std::vector<std::unique_ptr<UndoBet>>;

    mutable std::mutex mutex_;                // Handlers and tests may differ
    std::map<Craps::PlayerId, Stack> stacks_; // One stack per player
};

/*-----------------------------------------------------------*//**

@class UndoManager

@brief Undo of bet changes, a stack for each player

Supported (stack-based undo, limited to bets)

@li Scope: only bet state (no configuration, no gameplay state, no rolls).
@li Per player: each player has their own stack, so undo only ever
    touches the bets of the player who asked. The View asks for the
    user player.
@li Granularity: one bet change = one undo step. A batch can be made one
    step with push(..., joinPrevious=true).
@li Kinds of changes:

  @li new bet placed
  @li existing bet changed (contract or odds amount)
  @li bet removed

@li Ordering: last-in, first-out (classic undo stack).
@li Lifetime: cleared when the dice are rolled, so only decisions made
    since the last roll can be undone, and when the table changes. A
    player whose session ends loses their stack too.
@li Betting must be open. While the dice are rolling there is no undo,
    and a refused undo keeps its step.
@li A failed undo drops that player's history (see undo()).

Not supported (explicit exclusions)

@li Undoing out of order changes (no random-access undo).
@li Redo.
@li Anything outside of bets (like table config, game options).

@internal
@li The stacks hold shared pointers to bets, so a recorded bet outlives
    its removal. That is what lets undo put a removed bet back.
@li Who records: EventHandler, after each successful bet request. Who
    clears: EventHandler on a roll request, TableManager on a table
    switch and PlayerManager when a buddy leaves.

*/

} // namespace Ctrl

//----------------------------------------------------------------
