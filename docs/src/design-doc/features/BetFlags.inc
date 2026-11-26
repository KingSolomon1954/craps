Bet Flags Purpose
^^^^^^^^^^^^^^^^^

A bet flag says whether a wager is live for the next roll. A live bet can
win or lose on the roll. A bet that is not live stays on the table and
neither wins nor loses. Casinos let a player call certain bets off and on
at any time, and the flags give the player the same control.

Two things are controlled, for one bet or for a category of bets:

* A **standing** state, which lasts until the player changes it.
* A **one-roll override**, which applies to the next roll only and then
  goes away, leaving the standing state as it was.

Bet Flag Vocabulary
^^^^^^^^^^^^^^^^^^^

Standing state
   One of *Default*, *Working* and *Off*.

   *Default* is the house rule that is in force today. Place, Buy, Lay and
   odds bets are not live on a come-out roll and are live on every other
   roll. Hardway bets are live on every roll. A bet starts as Default.

   *Working* means live on every roll, including come-out rolls. *Off* means
   not live on any roll until the player changes it.

One-roll override
   One of *None*, *Off next roll* and *Working next roll*. Off next roll
   makes the bet not live on the next roll whatever its standing state is.
   Working next roll makes it live on the next roll, for example a Place bet
   that the player wants working on the coming come-out. The override is
   cleared after every roll, and a bet starts with None.

Live
   A bet is live on a roll when the override says so. With no override,
   Working is live, Off is not, and Default follows the house rule above.

Category
   A group of bets that a flag change can be applied to together: *All*,
   *Odds*, *Place*, *Buy*, *Lay* and *Hardways*.

Bet Flag Participants
^^^^^^^^^^^^^^^^^^^^^

* ``CrapsBet`` holds the standing state and the one-roll override and
  answers whether it is live on a roll.
* ``CrapsTable::setBetFlags()`` is the one way to change flags in the running
  game. It checks legality and that betting is open, then publishes the
  bet changed event. ``Player::setBetFlags()`` is the player's side of it.
* ``BetFlags`` (in the Controller) names the scopes and the changes, says
  which bets are in a scope, and turns a change into a new state.
* ``EventHandler`` answers ``ReqSetBetFlags`` with ``RspSetBetFlags`` and
  records the undo group.
* ``UndoBetFlagsChanged`` keeps the previous state of one bet.
* ``MenuBetFlags`` and ``OperationManager`` run the menus and the request.

Bet Flag Legality
^^^^^^^^^^^^^^^^^

Flags follow casino practice, so they are accepted only for bets that a
player may legally call off and on:

.. list-table::
   :header-rows: 1
   :widths: 30 20 50

   * - Bet
     - Flag allowed
     - What the flag controls
   * - Odds on Pass Line, Don't Pass, Come, Don't Come
     - Yes, when there are odds
     - Only the odds. The contract bet is always live.
   * - Place, Buy, Lay
     - Yes
     - The whole bet.
   * - Hardway
     - Yes
     - The whole bet.
   * - Pass Line, Don't Pass, Come, Don't Come contract bets
     - No
     - Not applicable; a player cannot call these off.
   * - Field, Any Craps, Any Seven, Any 7 or 11, C and E, Horn, World
     - No
     - They are decided on the next roll, so there is nothing to call off.

The table refuses a flag change that is not legal, and refuses any change
when betting is closed, with the same errors it gives for other illegal bet
changes. Changes that apply to a category skip the bets that cannot take a
flag, and the response says how many were changed and how many skipped.

Bet Flag Rules in the Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^

``CrapsBet`` holds the standing state and the one-roll override in place of
the single ``offComeOutRoll_`` flag. It answers one question, whether it is
live on a roll given the point, and the evaluation functions use that
answer in place of testing ``point == 0 && offComeOutRoll_``.

* **Place, Buy, Lay and Hardway** are Keep when not live, and are decided as
  they are today when live.
* **Odds** on a contract bet follow the same answer. Today the odds are
  returned when they are off on a come-out roll that decides the bet. The
  same happens whenever the odds are not live on the roll that decides the
  bet: the contract bet wins or loses as usual and the odds go back to the
  player without a win or loss.
* The existing ``setOffComeOutRoll``, ``setOnComeOutRoll``, ``setHardwayOff``
  and ``setHardwayOn`` stay, so existing code and tests keep working. They
  set the standing state: Default for the first and the fourth (a Hardway
  that is Default is working), Working for the second, Off for the third. ``offComeOutRoll()`` and ``hardwayWorking()``
  remain as observers and answer for the standing state with no override.
* ``CrapsTable`` clears every bet's one-roll override after it resolves the
  roll, before betting opens again.
* A change to a standing state replaces the bet's one-roll override: the
  Controller sends both together, so "Set Working" leaves no override behind.
  A "Next Roll" change leaves the standing state alone. Taking the odds off
  a bet takes its flags with it.
* ``CrapsTable::setBetFlags`` is the only way to change flags in the
  running game. It checks legality and that betting is open, then publishes
  the same bet changed notification that an odds change does, so the
  screens redraw. Flags are not saved with the player, since bets are not.

Bet Flag Menus
^^^^^^^^^^^^^^

The *Bet Flags* entry of the Betting menu (``g``) opens a menu of
categories. A category appears only if the player has bets on the table that
can take a flag in it, and the whole menu is refused with a notice if there
are none.

.. code-block:: text

   ┌──────────────────────┐
   │ Bet Flags            │
   ├──────────────────────┤
   │ [i] Individual Bet   │
   │ [a] All Bets         │
   │ [o] Odds Bets        │
   │ [p] Place Bets       │
   │ [b] Buy Bets         │
   │ [l] Lay Bets         │
   │ [h] Hardway Bets     │
   │ [. or esc] Back      │
   └──────────────────────┘

A category opens a pick list with the same prompts for every category, for
example:

.. code-block:: text

   ┌─────────────────────────────────────────┐
   │ Bet Flags - Odds Bets                   │
   ├─────────────────────────────────────────┤
   │ [a] Set Odds Bets Not-Working           │
   │ [b] Set Odds Bets Not-Working Next Roll │
   │ [c] Set Odds Bets Working               │
   │ [d] Set Odds Bets Working Next Roll     │
   │ [e] Set Odds Bets To Table Default      │
   │ [. or esc] Back                         │
   └─────────────────────────────────────────┘

*Individual Bet* opens a pick list of the bets that can take a flag, such as
*Pass Line 6 odds $30*, *Place 9 $20* and *Hardway 4 $5*, with each bet's
current flags in the description. Picking one opens the same five prompts
for that bet. The fifth prompt returns a bet to the Default standing state,
because Working and Not-Working would otherwise replace the house rule for
good.

The category pages and the bet pages are ``DialogPicker`` lists, so they
use letters in order. Every pick list ends with the line ``[. or esc] Back``
like the menus do. The hint of every pick list now reads
``Up/Down, Enter or a-e. [. or esc] Back``, replacing ``Esc cancels``;
the arrow keys and Enter still work. The category menu has mnemonic keys,
like the other menus, and shows only the kinds of bets the player has.

Bet Flag Workflow
^^^^^^^^^^^^^^^^^

Every choice sends one request and shows the answer:

#. The View sends ``ReqSetBetFlags`` with the player, a scope (one bet, All,
   Odds, Place, Buy, Lay or Hardways), the bet id when the scope is one bet, and
   the change (Not-Working, Not-Working Next Roll, Working, Working Next
   Roll or Table Default).
#. The Controller applies it through ``CrapsTable::setBetFlags`` for each bet
   in scope that can take it, as one undo group, and answers with
   ``RspSetBetFlags``: accepted or an error, a message such as
   "Odds bets set Not-Working: 2 changed", and the counts.
#. On success the View unwinds the menus and shows the message in a notice.
   On failure an error dialog says why, as for other bet requests.

Bet Flag Undo
^^^^^^^^^^^^^

A flag change is undoable with the Undo key. ``UndoBetFlagsChanged`` keeps
each bet's previous standing state and override, and restores them through
``CrapsTable::setBetFlags``, so the table checks the bet and the events are
published. The undo stack and its rules are unchanged: it is cleared when
the dice are rolled.

Bet Flag Display
^^^^^^^^^^^^^^^^

The amounts in the player's bet area are bold, in the player's color. A bet
that is **not live on the next roll** is drawn dim instead, and a bet that
has a standing state other than Default or has an override is **underlined**.
So a Place bet that is Off shows dim and underlined; a Place bet set to
Working next roll shows bold and underlined until the roll, and a Place bet
with no flags shows dim on the come-out, by the house rule, and bold
otherwise. No room is taken, so large amounts are not affected. The bet
dialogs already say that a bet is off on the come-out roll by default, and
that text is kept. If a terminal cannot dim, the bet is shown without bold.

Bet Flag Persistence
^^^^^^^^^^^^^^^^^^^^

None. Flags belong to a bet, and bets are not saved between games, so a
new bet always starts as Default with no override.

Bet Flag Related Features
^^^^^^^^^^^^^^^^^^^^^^^^^

* :ref:`Betting` makes the bets that carry the flags.
* :ref:`Undo of Bet Changes` takes back a flag change.
* :ref:`Bet Automation` does not set flags. A bet it places starts as
  Default.
