CrapsTable CRC Card
^^^^^^^^^^^^^^^^^^^

One table, and everything that happens at it.

**Responsibilities**

* **Seat the players** and know who is shooting. Pass the dice to the next
  shooter after a seven out.
* **Accept or refuse bets** against the table's rules and limits, and keep
  every bet on the table.
* **Run a roll:** throw the dice, have each bet decide, pay the winners and
  collect from the losers, and take finished bets off the table.
* **Keep the table's state:** the point, the shooter, whether betting is
  open, and the recent rolls.
* **Keep the house bank** and its statistics.
* **Keep statistics** for the current session and for all sessions, and
  close a session when the table is left.
* **Support undo,** by putting a bet back as it was without applying the
  rules a second time.
* **Convert itself to and from YAML.**
* **Publish events** for all of the above.

**Collaborators**

* ``Player``: the players seated at it
* ``CrapsBet``: the bets it holds
* ``TableRules``, ``TableConfig``: what it was built with
* ``Dice``, ``Bank``
* ``TableStats``, ``BankStats``, ``LastRollStats``, ``SessionHistory``
* ``Gen::EventManager``: where it publishes
* ``TableManager`` (Controller): creates it, saves it and retires it

Player CRC Card
^^^^^^^^^^^^^^^

One player, their money and their bets.

**Responsibilities**

* **Hold the player's money** in a bank, with its statistics.
* **Make, change and remove the player's bets.** A player has at most one
  bet for each spot, as at a real table. Making a bet that already exists
  changes its amount.
* **Join and leave a table.**
* **Receive the results of a roll,** adding wins to the balance and
  recording the statistics.
* **End a session** by taking the bets down, returning the money, merging
  the statistics and recording the session. The player can then join a
  table again and start a fresh session with the same balance.
* **Convert itself to and from YAML,** and know nothing of files.

**Collaborators**

* ``CrapsTable``: where the player sits
* ``CrapsBet``: the player's bets
* ``Bank``: the player's money
* ``PlayerStats``, ``SessionHistory``: results
* ``PlayerManager`` (Controller): creates it and saves it

CrapsBet CRC Card
^^^^^^^^^^^^^^^^^

One bet on one spot.

**Responsibilities**

* **Know what it is:** its name, its amount, its pivot (the number it is on,
  such as the 8 for a place bet), the odds on it, and when it was made.
* **Evaluate itself** for a roll, against the point, the dice and the
  table's rules, and answer with a ``DecisionRecord``.
* **Know its flags:** whether it is working or off, and for one roll or
  until changed. See :ref:`Bet Flags`.
* **Know whether it is live** for a roll, which depends on the point and on
  its flags.
* **Refuse to be made wrong,** by checking that its name, amount and pivot
  agree with each other.

Only ``CrapsTable`` and ``Player`` change a bet's amounts. Everyone else
reads them.

**Collaborators**

* ``CrapsTable``, ``Player``: hold it and change it
* ``TableRules``, ``OddsTables``: how it pays
* ``Dice``: the roll it is evaluated against
* ``DecisionRecord``: its answer
* ``EnumBetName``: its name

Dice CRC Card
^^^^^^^^^^^^^

Two dice.

**Responsibilities**

* **Roll,** with a good random number generator.
* **Set a value,** for tests.
* **Report** each die and the total.

**Collaborators**

* ``CrapsTable``: rolls them
* ``CrapsBet``, ``TableStats``: read them

Bank CRC Card
^^^^^^^^^^^^^

A pile of money, for a player or for the house.

**Responsibilities**

* **Deposit and withdraw,** refusing a withdrawal that is more than the
  balance.
* **Refill the house.** A bank given a threshold and an amount adds the
  amount when the balance falls to the threshold, and counts the refills.
* **Keep statistics** for the session and for all time: the amounts put in
  and taken out, and how often.
* **Convert itself to and from YAML.**

**Collaborators**

* ``Player``, ``CrapsTable``: each has one
* ``BankStats``

TableRules CRC Card
^^^^^^^^^^^^^^^^^^^

The rules of a table, as data.

**Responsibilities**

* **Hold the limits** (minimum, maximum and increment) for each bet.
* **Hold the odds the table allows,** as a flat multiple or a schedule for
  each point, and the caps on odds.
* **Hold the payouts,** the rounding, and the commission: on what it is
  charged, and when it is collected.
* **Convert itself to and from YAML.**

A ``TableRules`` holds no state of the table. The table evaluates its bets
against it.

**Collaborators**

* ``CrapsTable``: owns it
* ``CrapsBet``, ``OddsTables``: use it
* ``TableManager`` (Controller): its file supplies it

TableStats and PlayerStats CRC Card
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

What has happened, counted.

**Responsibilities**

* **Count wins, losses and keeps** for each kind of bet, including the odds
  part, and the money that moved.
* **Count the dice:** every number, the numbers that were points, the
  repeats of a number before a seven, and the hardway wins.
* **Keep two sets:** the current session, and all time. A session is merged
  into the all-time set when it ends.
* **Keep a history of sessions,** up to a limit.
* **Convert themselves to and from YAML.**

``PlayerStats`` is the same idea for one player. The classes that hold the
counts are ``BetStats``, ``RollStats``, ``LastRollStats`` and
``Repeaters``.

**Collaborators**

* ``CrapsTable``, ``Player``: own them and feed them
* ``DecisionRecord``: what they record
* ``BankStats``, ``SessionHistory``
