Betting Purpose
^^^^^^^^^^^^^^^

Betting is the heart of the game. A player puts money on a bet, may change
its amount or take it down while betting is open, and has it settled by the
roll. This page follows one bet from the person's keystrokes to the table
and back to the screen. The features built on it are :ref:`Bet Automation`,
:ref:`Bet Flags` and :ref:`Undo of Bet Changes`.

Betting Vocabulary
^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Term
     - Meaning
   * - Bet name
     - The kind of bet: Pass Line, Come, Don't Pass, Don't Come, Place, Lay,
       Buy, Hardway, Field, Any Craps, Any Seven, Any 7/11, C and E, Horn
       and World.
   * - Pivot
     - The number a bet is on. A place bet on the 8 has the pivot 8. A bet
       with no number has the pivot 0. A Pass Line or Come bet gets its
       pivot when the point or a come point is set.
   * - Contract amount
     - The money in the bet itself. The odds amount is kept separately.
   * - Odds
     - The extra money a Pass Line, Come, Don't Pass or Don't Come bet may
       have behind it once it has a number. The table's rules set how much.
   * - Betting open
     - The time between rolls when bets may be made, changed or taken down.
       Betting closes when the dice are thrown and opens again when the roll
       has been settled.

Betting Participants
^^^^^^^^^^^^^^^^^^^^

* ``MenuBetting`` and the menus and dialogs behind it (``MenuPlaceBet``,
  ``MenuPivot``, ``MenuOddsBet`` and ``DialogBetAmount``) gather the bet
  name, the pivot and the amount. They ask the table, through
  ``CrapsReaders``, whether the bet is allowed and what the limits are, so
  the person is told early. They do not decide anything.
* ``OperationManager`` runs the Make Bet, Remove Bet and Odds operations. It
  sends the request, waits for the matching response and unwinds the menus.
  See :ref:`OperationManager CRC Card`.
* ``EventHandler`` answers ``ReqMakeBet``, ``ReqOddsBet`` and
  ``ReqRemoveBet`` and records an undo step for each change that succeeds.
* ``Player`` makes, changes and removes the bets of one player and moves the
  money. ``CrapsTable`` checks every change against the table's rules.
* ``CrapsBet`` holds one bet and settles itself when the dice are thrown.

Betting Flow
^^^^^^^^^^^^

#. The person picks a bet in ``MenuBetting``. If the bet needs a number the
   person picks it in ``MenuPlaceBet`` or ``MenuPivot``. The person then
   enters an amount in ``DialogBetAmount``, which shows the table's
   minimum, maximum and increment for that bet. The choices are collected
   in a ``CarrierBet``.
#. ``OperationManager`` sends ``ReqMakeBet`` with the player, bet name, pivot
   and amount.
#. ``EventHandler`` calls ``Player::makeOrChangeBet()``. A player has one
   bet for each spot, as at a real table, so asking for a bet the player
   already has changes that bet's amount and does not make a second bet.
#. For a new bet, ``Player`` makes the bet and ``CrapsTable`` checks it, in
   this order: betting is open, the player is at the table, the player does
   not already have it, the amount is not zero, Come and Don't Pass are
   allowed at this time, the amount is within the table's limits, and it is
   a multiple of the increment. The player's money is then taken for the
   bet, plus the commission if the bet pays one up front. If the player does
   not have the money, the bet is refused. A Pass Line bet made after the
   point is set takes the point as its pivot.
#. The bet is on the table. The table and the player publish ``UslBetMade``
   (or ``UslBetChanged``), ``UslTableBetsChanged``, ``UslPlayerBetsChanged``
   and ``UslPlayerBalanceChanged``.
#. ``EventHandler`` records the undo step and the new LastFill, then answers
   with ``RspMakeBet``. It has ``accepted``, the ``betId``, and for a change
   it has ``changed`` and the ``previousAmount``. A refusal has the reason.
#. The View updates the player area from the events and the
   ``OperationManager`` closes the dialogs on the response. A refusal is shown
   to the person in an error dialog and nothing has changed.

Odds are a second request, ``ReqOddsBet``, on a bet that already has its
number. It goes through ``Player::setOddsAmount()``, and the table checks it
against the odds rules. Taking a bet down is ``ReqRemoveBet``. The player's
money comes back to the player.

Betting Rules
^^^^^^^^^^^^^

* Bets can be made, changed and removed only while betting is open. The
  table refuses at the lowest level, so no path can get around it.
* The limits, the increments and the odds allowed are the table's. See
  :ref:`Model Overview`.
* A Pass Line or Come bet that has a number must stay on the table until it
  is decided, and its contract amount cannot be reduced. A Don't Pass or
  Don't Come bet that has a number cannot be increased, but it may be
  reduced or taken down.
* A request is all or nothing. When it is refused nothing has changed, and
  the answer says why in words the person can read.
* The View may know the answer before it asks, but the table decides. A
  disagreement is settled in favor of the table.

Betting Persistence
^^^^^^^^^^^^^^^^^^^

Bets are not kept between games. When the game ends, or the table is left,
every seated player's session ends: the bets come down and the money goes
back to the player. What is saved is the player's money and statistics. See
:ref:`Players and Buddies` and :ref:`Persistence and Configuration`.

Betting Related Features
^^^^^^^^^^^^^^^^^^^^^^^^

* :ref:`Bet Automation` places many bets in one step, through the same path.
* :ref:`Bet Flags` set whether a bet works and for how long.
* :ref:`Undo of Bet Changes` takes back what this page puts on the table.
* :ref:`Table Switching` takes the bets down when the table is left.
