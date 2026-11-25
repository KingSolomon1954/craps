.. Conventions are the same as in statistics.inc: **bold** for the first use
.. of a defined term, *italic* for emphasis, ``literal`` for file names,
.. settings and commands, and Notes for things that commonly surprise people.

Tables
======

|AppName| comes with more than one **table**. Each table has its own rules,
its own bet limits and odds, and its own bank, and it keeps its own
statistics from one visit to the next. You play at one table at a time.

Everything in this section is in the *Table Menu*. Press ``C`` for the
Control menu, then ``t``.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Key
     - What it does
   * - ``l``
     - **List Tables** shows every table with a short description. The one
       you are at is marked ``(current)``.
   * - ``s``
     - **Switch to Table** moves you to another table.
   * - ``.`` or Esc
     - Back to the Control menu.

Switching Tables
----------------

Pick the table you want from the list. The table you are at is not on it.

You can switch only while betting is open, between rolls. While the dice are
rolling the switch is refused with a message, and you can try again when the
roll is over. A point does not carry over. The new table starts with a
come-out roll and you are the shooter.

When you switch:

* Every bet on the table you leave is taken down and the money goes back to
  the player who made it. If you have bets on the table, you are asked to
  confirm first.
* The table you leave records the session, and its statistics and bank are
  saved.
* Your undo history is cleared, because the bets it would undo are gone.
* AutoOdds forgets the bets it had handled, and starts fresh at the new
  table.
* The roll history and the title bar start over for the new table.

.. note::

   Your player, with its bank and statistics, goes with you. Only the table
   changes.

Your Buddies
------------

The buddy players at the table can go with you or not. The choice is yours,
and you make it in the Configure menu, under *Buddies Follow* (press ``b``):

``Follow``
   The buddies go to the new table with you. This is the usual setting.

``Stay``
   The buddies leave the game when you switch. Their bets are returned and
   their results are saved. You can seat them again at the new table with
   Buddy Joins Table.

``Ask``
   You are asked each time you switch, if any buddy is at the table.
   Answer Yes for the buddies to come along, or No for them to leave.
   Esc cancels the switch.

The setting belongs to the player you are playing as and is kept with that
player. Buddies who go along keep their seats in the same order.

After the switch, a message tells you how many buddies came along and how
many left the game, and how many bets were returned.

Your Saved Bets and Other Tables
--------------------------------

Your QuickBets, your Last Fill amounts (the amount you used last time for
each kind of bet) and your AutoOdds settings belong to your player, not to
a table. They go with you when you switch. Tables differ in their limits
and odds, so something that worked at one table can be refused at another:

* A bet amount below the new table's minimum, or above its maximum, is
  refused, and the message gives the table's limits. A QuickBets entry
  places the bets that fit and skips the ones that do not.
* An odds amount larger than the new table allows is refused.
* An AutoOdds setting given as a multiple, such as ``3x``, follows each
  table's own odds limit. One given as a dollar amount does not.

|AppName| does not change your saved bets for you. If you will play at the
new table for a while, change your QuickBets and AutoOdds settings to suit
it, and place your bets by hand once so that Last Fill follows. Keep in
mind that they then replace what you had, and they are what you will have
when you go back. If you are only visiting, place your bets by hand and
leave your saved ones as they are.

.. note::

   The new table's limits are shown in the title bar, after the table name.

Where the Tables Come From
--------------------------

The tables come with the game, like the buddy players do. They are listed
in ``tables.yaml`` in the ``tables`` folder, and each table has a file
there, such as ``las-vegas-strip.yaml`` and ``las-vegas-high-roller.yaml``.
There is no way to make a table in the game in this version.

.. note::

   A table file holds the table's rules and its statistics. If you have
   played before this version, a new table file is added to your ``tables``
   folder only if it is not already there. Your existing tables, and the
   statistics in them, are never replaced.

The game opens at the table you were at when you last quit.
