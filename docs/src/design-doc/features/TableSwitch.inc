Table Switch Purpose
^^^^^^^^^^^^^^^^^^^^

The game comes with several tables, listed in ``tables.yaml``. The user
player can move to another one during play, with or without the buddy
players. The tables are fixed by the installation. Making a table is not a
function of the game.

Table Switch Vocabulary
^^^^^^^^^^^^^^^^^^^^^^^

Manifest
   ``tables.yaml``, the list of the tables that come with the game, with a
   name and a file for each.

Table file
   The file of one table. It holds the table's rules and the table's own
   statistics and session history.

Buddy follow
   The user player's choice of what the buddy players do on a switch:
   ``Follow``, ``Stay`` or ``Ask``.

Retired table
   A table that has been switched away from. It is closed and empty, and is
   kept until the game ends so that a late read by the View is safe.

Table Switch Participants
^^^^^^^^^^^^^^^^^^^^^^^^^

* ``TableManager`` owns the current ``CrapsTable`` and the file it came
  from. ``switchCrapsTable()`` does the whole switch. ``CrapsTable`` knows
  nothing of files.
* ``EventHandler`` answers ``ReqSwitchTable`` with ``RspSwitchTable``, then
  sends ``UslTableSwitched`` and the new table's snapshot events.
* ``BuddyFollow`` (``Follow``, ``Stay``, ``Ask``) is kept in ``PlayerPrefs``
  with the user player. ``ReqSetBuddyFollow`` changes it. With ``Ask`` the
  View asks, then sends the answer in ``ReqSwitchTable``.
* ``OperationManager`` runs the Table operations for ``MenuTable``, and the
  Buddies Follow choice for ``MenuConfigure``.

Table Switch Steps
^^^^^^^^^^^^^^^^^^

#. Check: the table is in the manifest, is not the current one, and betting
   is open. Load the new table file. A file that cannot be read fails the
   request and nothing has changed. After this point the switch does not
   fail.
#. Every seated player ends the session at the old table: bets come down,
   the money is returned and statistics are merged.
#. The undo stacks are cleared. The old table is closed, which records the
   session with the number of players it had, and its file is saved.
#. The new table becomes ``Gbl::pTable``. The user player and, if they follow,
   the buddies join it in the seating order they had. The user is the
   shooter.
#. The players' files are saved and AutoOdds forgets what it handled.

The old table object is retired, not deleted. The View reads the table
directly through ``CrapsReaders``, and it may be in the middle of a read on
the CUI thread as the switch happens. A retired table is closed and empty,
so a late read is safe. Retired tables are released when the game ends.

On ``UslTableSwitched`` the View clears the roll list, resets the title bar
and the dice window, and refreshes the roster. The numbers for the house
and the players arrive in the snapshot events that follow it.

Table Switch Rules
^^^^^^^^^^^^^^^^^^

* Only a table in the manifest can be switched to, and not the table the
  player is at.
* Switching is refused while betting is closed, that is, during a roll.
* The new table is loaded before anything is changed. If its file cannot be
  read, the request fails and the player is still at the old table.
* A player's bets do not go along. They come down and the money returns to
  the player, as when a player leaves a table.
* A player's saved QuickBets, LastFill and AutoOdds are not changed for the
  new table. An amount that is too small for the new table is refused when
  it is used, with the table's reason, and the person may change it.

Table Switch Persistence
^^^^^^^^^^^^^^^^^^^^^^^^

The old table's file is saved when it is closed, with its statistics and the
session just ended. The players' files are saved after the switch. Which
table the game starts at next time is part of the saved state. See
:ref:`Persistence and Configuration`.

Table Switch Related Features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :ref:`Players and Buddies` explains who is seated and what Buddy follow
  does.
* :ref:`Betting` shows what happens to the bets that were on the table.
* :ref:`Undo of Bet Changes` loses its steps at a switch.
* :ref:`Bet Automation` keeps its saved plans across a switch.
