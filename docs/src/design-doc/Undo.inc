Undo Purpose
^^^^^^^^^^^^

A player can take back bet changes made since the last roll: a bet placed,
a bet's amount or odds changed, or a bet taken down. Undo is limited to
bets. Settings, rolls and the state of the game are never undone.

Undo Participants
^^^^^^^^^^^^^^^^^

* ``UndoBet`` is one recorded change that knows how to take itself back
  and how to describe that in words. Its subclasses are ``UndoBetAdded``,
  ``UndoBetRemoved`` and ``UndoBetModifiedAmount``. ``UndoGroup`` holds
  several as one step.
* ``UndoManager`` is a singleton holding a stack of steps for each player.
* ``EventHandler`` records a step after every bet request that succeeds,
  answers ``ReqUndoBet`` with ``RspUndoBet``, and clears the stacks when a
  roll is requested.
* ``Player`` and ``CrapsTable`` provide the restore paths that undo uses:
  ``Player::restoreBet()``, ``Player::restoreAmounts()``, and
  ``CrapsTable::restoreBet()`` and ``CrapsTable::restoreBetAmounts()``.
* ``OperationManager`` runs the Undo operation for ``MenuBetting``.

Undo Recording Steps
^^^^^^^^^^^^^^^^^^^^

The View knows nothing of the stack. The recording happens entirely in the
Controller, so any View, and later any automation that goes through the
same requests, is undoable the same way.

* ``ReqMakeBet`` for a spot with no bet records ``UndoBetAdded``. For a
  spot that already has a bet the request changes the bet's amount, which
  records ``UndoBetModifiedAmount`` if the amount really changed. A
  ``UndoBetModifiedAmount`` is built before the change, because it copies
  the bet's state as it is then.
* ``ReqOddsBet`` records ``UndoBetModifiedAmount``, whether it sets, changes
  or takes down odds.
* ``ReqRemoveBet`` records ``UndoBetRemoved``. The stack keeps a shared
  pointer to the removed bet, which is what lets it be put back.
* A change that is refused records nothing.
* Taking down several bets at once sends one request per bet. All but the
  first carry ``undoJoinsPrevious``, which makes ``UndoManager`` merge the
  change into the step on top of the stack, wrapping that step in an
  ``UndoGroup`` first if it is not one already. The batch is then undone
  in one step, last removal first.

Undo Restore Paths
^^^^^^^^^^^^^^^^^^

Undo does not run a change backward through the normal bet functions. The
change being undone was allowed, and undoing it can look like a change that
is not: reducing a ``PassLine`` bet after the point, increasing a
``DontPass`` bet, or putting back a ``DontPass`` bet after the point. The
normal rules would refuse these.

The restore paths therefore skip the rules for making or changing a bet,
and check only what must be true for undo to be safe: betting is open, the
bet is on the table (or, for a restore, is not already there) and the player
has the money. They go through the table, so the player's wallet, the house
bank, placement commission and the bet, balance and table events all update
as for a normal change. Without those events the screens would not follow
the undo. The bet's original id is kept, so the screens treat a restored
bet as the same bet.

A bet placed by the player is taken down the normal way by
``UndoBetAdded``, through ``Player::removeBet()``, since that is what it is.

Undo Rules and Limits
^^^^^^^^^^^^^^^^^^^^^

* Last in, first out. There is no random access and no redo.
* Each player has a stack, and an undo request applies to its own player.
  The View asks for the user player. This keeps the bets of a buddy
  safe from the user's undo, and a player who switches away and back keeps
  their steps.
* The stacks are cleared when a roll is requested, because the roll decides
  the bets, when the table is switched, and when a buddy leaves the table.
  They are cleared before the roll, in the request handler, so the clearing
  is ordered with the bet requests and no timing between threads is
  involved.
* Undo is refused while betting is closed. The refusal keeps the steps.
* A failed undo drops that player's whole history and reports the reason.
  The step that failed is no longer known to match the table, so undoing
  the steps below it would be unsafe. Nothing is changed by the failed step
  itself.
* A mutex protects the stacks, since requests and tests may use them from
  different threads.

Undo Workflow
^^^^^^^^^^^^^

``MenuBetting`` calls ``OperationManager::undoLastBet()`` for ``u``. The
operation sends one ``ReqUndoBet`` for the user player and waits for the
``RspUndoBet`` with the matching correlation id.

* **Success:** the betting menus unwind to the table screen, as after any
  bet. The bet and balance events that the undo caused have already been
  sent, and redraw the player areas and the balances as they do for any bet
  change. The response carries a description such as "Took down your $10
  Place 6 bet", which is logged.
* **Failure:** an error dialog shows the reason. Nothing to undo and betting
  closed are the common ones. After the player acknowledges it, the menus
  unwind.

Changes to a bet's flags, such as a hardway being off, are not recorded,
because nothing changes those flags yet; the Bet Flags menu entry is
still a stub. The earlier ``UndoBetModifiedFlags`` class was removed
because it was never used and restored a bet by overwriting it, which skipped
the table and its events. When flag changes are implemented, they need their
own ``UndoBet`` subclass that restores through the table.
