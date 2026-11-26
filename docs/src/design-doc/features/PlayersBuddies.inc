Players and Buddies Purpose
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Up to six players share a table. One of them is the **user player**, the
one the person at the keyboard is playing as. The others are **buddy
players** who come with the game and sit at the table for company. Players
keep their money, their bets' statistics and their preferences from one game
to the next.

Players and Buddies Vocabulary
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Term
     - Meaning
   * - User player
     - A player the person can become, rename and delete. The person may
       make as many as they like. A new player is always a user player.
   * - Buddy player
     - A player delivered with the game. A buddy can sit at the table and
       leave it, but never becomes the user player and is never renamed or
       deleted.
   * - Manifest
     - The index of the players, with the id, kind, name, description and
       file name of each.
   * - Preferences
     - What the game remembers about a player apart from the player's
       money: QuickBets, AutoOdds, LastFill and, for the user player,
       whether the buddies follow to a new table.
   * - Session
     - The time a player is at a table. It ends when the player leaves or
       the table is left, and its results are added to the player's
       history.

Players and Buddies Participants
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``PlayerManager`` knows who the players are, loads them, owns their files
  and preferences, and knows who the user player is. It is the one place
  that creates, renames, deletes and switches players.
* ``PlayerManifest`` is the index. It is read and written as one file.
* ``Player`` is the Model's player. It converts itself to and from YAML
  and knows nothing of files. See :ref:`Model CRC Cards`.
* ``PlayerPrefs`` holds the preferences and is saved with the player, in
  its own section of the player's file.
* ``EventHandler`` answers ``ReqManagePlayer`` and ``ReqSwitchPlayer``.
* ``MenuPlayers`` and ``OperationManager`` run the operations for the person:
  pick a player, confirm, send the request, show the answer.

Players and Buddies Flow
^^^^^^^^^^^^^^^^^^^^^^^^

**Starting.** The manifest is read, and the players named in the starting
choices are loaded from their files and seated. The user player is the one
named in the saved state. See :ref:`Startup, Shutdown and Directory Layout`.

**Switching the user player.** ``ReqSwitchPlayer`` names the player to
become. ``PlayerManager::switchPlayer()`` checks that the player is known,
is not already the user and is not already at the table, and loads the
player if needed. The table then swaps the two players. The old user's
session ends, the old user's bets come off the table and the money goes back
to the old user, and the new user takes the seat and the dice if the old
user had them. The player who left is written to his file. If anything fails
before the swap, nothing has changed and the old user is still playing. The
View asks the person to confirm first if there are bets on the table.

**Managing players.** ``ReqManagePlayer`` carries an action.

* *Create* makes a user player with a fresh id, a name, an optional short
  description and the usual starting bank. The player file and the manifest
  are written at once. It does not switch to the new player.
* *Rename* changes the name. Names are trimmed, 1 to 16 printable ASCII
  characters and unique ignoring case. Only the name changes. The id and the
  file name stay, because the saved state refers to players by file name.
  The manifest is written at once.
* *Delete* applies to a user player who is not the current user and is not
  at the table. The file is moved to a trash folder and not erased.
* *BuddyJoins* and *BuddyLeaves* seat a buddy or take one away. A leaving
  buddy's bets come off the table and the money goes back to the buddy.

Each is answered by ``RspManagePlayer``. When it succeeds the table and the
player publish the usual events, and a rename also publishes
``UslPlayerRenamed`` so that the View drops the names it kept.

**At a new table.** When the user switches tables, ``BuddyFollow`` decides
what the buddies do: ``Follow`` takes them along, ``Stay`` leaves them out,
and ``Ask`` has the View ask the person at the time. See :ref:`Table
Switching`.

Players and Buddies Rules
^^^^^^^^^^^^^^^^^^^^^^^^^

* At most six players sit at a table.
* A player is at one table at a time, and has at most one bet for each spot.
* The user player and buddy players are told apart by kind in the manifest,
  and the kind is checked by the manager, not trusted from the View.
* The manager's lists are guarded by a mutex. ``getPlayers()`` hands out a
  reference and is not protected, so it is not to be called while a switch
  can be running.
* A player who is switched out stays loaded. If he is switched in again he
  starts a new session.

Players and Buddies Persistence
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each player has a file in the players folder, with a section for the player
and a section for the preferences. The manifest is a separate file. The
player files are written when the game ends, when the user player switches,
when a player leaves the table or the table is switched, and when a
preference such as buddy follow is changed. A rename or a new player writes
the manifest at once and the player's file at the next save moment.
Session results are merged into the player before the player is saved, so
the manager always saves after the table has shut the players down.

Players and Buddies Related Features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :ref:`Table Switching` moves the players to another table.
* :ref:`Bet Automation` keeps its settings in each player's preferences.
* :ref:`Statistics` are kept for each player and for each table.
* Buddy players that place bets of their own are not built yet. The buddies
  now sit and watch.
