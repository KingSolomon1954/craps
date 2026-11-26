The Model is the Craps Engine, in ``lib-craps``. It knows how a craps table
works: who is at the table, what bets are on it, what a roll does to each
bet, who gets paid, and what the statistics say. It knows nothing of the
screen, and nothing of files.

* It never calls the View or the Controller. It reports what happens by
  publishing unsolicited events. See :ref:`Events`.
* It does not read or write files. The classes that have a saved state
  convert themselves to and from YAML nodes, and the Controller decides
  where the YAML is kept. See :ref:`Persistence and Configuration`.
* It does not read the configuration. What it needs is given to it when it
  is built, in a ``TableConfig`` and in the table's rules.
* It is tested on its own, with unit tests that play whole rolls.

Model Structure
^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Group
     - Classes
   * - The table
     - ``CrapsTable``, ``TableRules``, ``TableConfig``, ``OddsTables``
   * - The players
     - ``Player``, ``Bank``
   * - Bets
     - ``CrapsBet``, ``EnumBetName`` (the names of the bets),
       ``DecisionRecord``
   * - Dice
     - ``Dice``
   * - Statistics
     - ``TableStats``, ``PlayerStats``, ``BetStats``, ``RollStats``,
       ``LastRollStats``, ``BankStats``, ``Repeaters``,
       ``SessionHistory``

Money is held in ``Gen::Money``, an amount in cents that is exact and does
not suffer the rounding of floating point.

The Rules Are Data
^^^^^^^^^^^^^^^^^^

How a table pays, and what it allows, is not written into the code of the
bets. It is a value, ``TableRules``, read from the table's file. It holds
the limits for each bet, the odds the table allows, the payouts, whether
there is a commission and how it is collected, and how payouts are
rounded. This is how the Las Vegas Strip and the High Roller table are the
same code with different files. A bet is evaluated against the rules of the
table it is on.

How a Roll Is Resolved
^^^^^^^^^^^^^^^^^^^^^^

``CrapsTable::rollDice()`` does a whole roll in one call, in this order:

#. Betting closes, and the players are told.
#. The dice are thrown. The table keeps the recent rolls and counts the
   roll in its statistics.
#. Every bet on the table is evaluated, one at a time, against the point,
   the dice and the rules. Each answers with a ``DecisionRecord``: won,
   lost, no decision, how much, and whether the bet comes down. If any
   evaluation fails, no decision is applied and the table is left as it was.
#. The decisions are carried out. Winners are paid by the house, and losing
   money goes to the house. The house bank is refilled if it runs low. Bets
   that are finished come off the table, and each player is told.
#. The table moves to its next state: the point is set or ended, and the
   dice go to the next shooter after a seven out.
#. Betting opens.

Events are published at each step, as listed in
:ref:`Roll Animation Synchronization`.
