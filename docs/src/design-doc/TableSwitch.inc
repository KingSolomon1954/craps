Table Switch Purpose
^^^^^^^^^^^^^^^^^^^^

The game comes with several tables, listed in ``tables.yaml``. The user
player can move to another one during play, with or without the buddy
players. The tables are fixed by the installation. Making a table is not a
function of the game.

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
