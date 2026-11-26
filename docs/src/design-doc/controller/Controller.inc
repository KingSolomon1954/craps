GameMain CRC Card
^^^^^^^^^^^^^^^^^

The top of the application. It builds everything, waits, and takes it all
down.

**Responsibilities**

* **Create the globals in order.** Build information, configuration, the
  event handler, the table manager and the player manager, and publish them
  through ``Gbl``.
* **Set up logging** once the configuration says where the log file is, and
  turn off console logging before the View starts.
* **Start the selected View**, console or graphical.
* **Wait.** The main thread blocks until a termination signal arrives or
  the user quits.
* **Shut down in order:** the View, then the table, then the players, then
  the user state.
* **Remember who was playing,** and where, for the next start.

**Collaborators**

* ``ConfigManager``: tells it which View, and where the log goes
* ``EventHandler``, ``TableManager``, ``PlayerManager``: created and shut
  down by it
* ``CuiMain`` (View): started and stopped by it
* ``UserState``: written at quit
* ``Gen::SignalHandler``, ``Gen::Logger``, ``Gen::BuildInfo``

EventHandler CRC Card
^^^^^^^^^^^^^^^^^^^^^

The Controller's front door. It is where the View's requests are turned
into action.

**Responsibilities**

* **Subscribe to the View's requests** and to the few engine events the
  Controller must react to.
* **Carry out each request** by calling the engine or a manager, and check
  that the table's state allows it, for example that betting is open.
* **Answer** with a response event whose correlation id matches the request,
  carrying the result or the error.
* **Announce what the Controller itself does,** such as a table switch or a
  player change, with unsolicited events.
* **Record undo steps** after each successful bet change, and clear them
  when a roll starts.
* **Run bet automation** when betting opens, and publish what was applied.
* **End the application** when asked to terminate.

**Collaborators**

* ``Gen::EventManager``: where it subscribes and publishes
* ``GameEvents``: the events it handles and sends
* ``CrapsTable`` and ``Player`` (Model): the work itself, through ``Gbl::pTable``
* ``PlayerManager``, ``TableManager``: players, tables, switching
* ``UndoManager``: undo steps
* ``BetExecutor``: applying QuickBets and AutoOdds
* ``PlayerPrefs``: the player's saved settings
* ``GameMain``: asked to end the application

CrapsReaders CRC Card
^^^^^^^^^^^^^^^^^^^^^

A set of read-only functions through which the View asks the model how
things are now, without sending requests and waiting for responses.

**Responsibilities**

* **Answer questions** such as the table's name and limits, the point, the
  players and their bets and balances, and the settings.
* **Never change anything.** A reader returns a copy and has no side
  effects.
* **Report failure** through an ``ErrorPass`` rather than an exception.

**Collaborators**

* The View, its only user, from the CUI thread. See :ref:`Thread Rules`.
* ``Gbl::pTable``, ``PlayerManager``, ``TableManager``, ``ConfigManager``:
  where the answers come from
* ``CrapsTable``, ``Player`` (Model)

PlayerManager CRC Card
^^^^^^^^^^^^^^^^^^^^^^

Owns the players, their files and their preferences, and knows which one
is the user.

**Responsibilities**

* **Load the starting players** and know every player the game has, from
  the players manifest.
* **Own each player's file and preferences,** and decide when they are
  read and written. A ``Player`` knows nothing about files.
* **Know the user player** and switch it, so that a failed switch leaves
  the current user playing.
* **Create, rename and delete user players.** Buddy players come with the
  game and cannot become the user.
* **Seat and remove buddies,** returning a leaving buddy's bets.
* **Save every player** at quit, after the table has ended their sessions.

**Collaborators**

* ``Craps::Player``: the Model objects it owns
* ``PlayerPrefs``: the settings it keeps for each player
* ``YamlFile``, ``PlayerManifest``: the files it reads and writes
* ``CrapsTable`` (through ``Gbl::pTable``): seating players
* ``UndoManager``: cleared when a buddy leaves
* ``ConfigManager``: the directories and the starting players
* ``EventHandler``: its caller

TableManager CRC Card
^^^^^^^^^^^^^^^^^^^^^

Owns the tables and their files, and moves the game from one table to
another.

**Responsibilities**

* **Load the starting table** and know which tables the game has, from
  the tables manifest.
* **Own the table's file.** A ``CrapsTable`` knows nothing about files.
* **Save the table** at quit and when leaving it.
* **Switch tables,** with the user and, if they follow, the buddies. See
  :ref:`Table Switch Steps`.
* **Retire old tables** rather than delete them while the game runs.

**Collaborators**

* ``CrapsTable`` (Model): the tables it owns
* ``YamlFile``, ``TableManifest``: the files it reads and writes
* ``PlayerManager``: players are saved and moved with the table
* ``UndoManager``: cleared on a switch
* ``ConfigManager``: the directories and the session limit
* ``EventHandler``: its caller

PlayerPrefs CRC Card
^^^^^^^^^^^^^^^^^^^^

One player's settings that the Model knows nothing about.

**Responsibilities**

* **Hold the preferences:** ``LastFill`` (the amount last used for each
  bet), ``QuickBets`` (named bundles of bets), ``AutoOdds`` (the odds to
  take automatically) and the buddy follow choice.
* **Convert to and from YAML,** as the ``Prefs`` section of the player's
  file, forgivingly. A bad or missing entry is skipped, never fatal.

These are plain data. Placing the bets they describe is the job of
``BetExecutor``.

**Collaborators**

* ``PlayerManager``: owns one for each player
* ``EventHandler``: edits them on request
* ``BetExecutor``: applies QuickBets and AutoOdds
* ``BetSpec``: the description of one bet to place
* ``BuddyFollow``

UndoManager CRC Card
^^^^^^^^^^^^^^^^^^^^

Remembers recent bet changes so a player can take them back.

**Responsibilities**

* **Keep a last-in first-out stack for each player** of bet changes: a bet
  placed, changed or removed.
* **Undo the latest step,** only while betting is open.
* **Group a batch into one step,** so applying a QuickBets entry is
  undone in one go.
* **Clear** when the dice are rolled, when a table switch happens, and when
  a player's session ends.

**Collaborators**

* ``EventHandler``: records steps and clears on a roll
* ``BetExecutor``: records the steps of a batch
* ``TableManager``, ``PlayerManager``: clear on a switch or a departure
* ``CrapsBet``, ``Player`` (Model): what it puts back

BetExecutor CRC Card
^^^^^^^^^^^^^^^^^^^^

The one place that turns a list of bet specifications into placed bets.

**Responsibilities**

* **Place each bet** through the player, the same way as a request from
  the View, so the table's rules, limits and funds checks apply.
* **Report** what was placed and what was skipped, and why.
* **Record one undo step** for everything a call changed.

It holds no state of its own.

**Collaborators**

* ``Player`` (Model): makes the bets
* ``BetSpec``, ``QuickBets``, ``AutoOdds``: what it is asked to place
* ``UndoManager``: where the undo step goes
* ``EventHandler``: its caller

ConfigManager CRC Card
^^^^^^^^^^^^^^^^^^^^^^

The game's configuration, specialized from the general multi-layer
configuration.

**Responsibilities**

* **Build the layers** and fill them: defaults, command line, environment,
  then the files. It also adds a session layer, above the rest, for
  settings the game changes while it runs.
* **Name the settings** as constants, and give typed access to them, such
  as the animation speed and the roll history format, with a safe default
  for a value that cannot be read.
* **Fail startup** with a clear message if the configuration cannot be
  resolved.

**Collaborators**

* ``Gen::MultiLayerConfig``: the mechanism it extends
* ``ConfigDefaults``, ``ConfigCommandLine``, ``ConfigEnv``, ``ConfigFiles``:
  fill the layers
* ``ConfigSeed``, ``StartChoices``: used by ``ConfigFiles`` to prepare the
  user's files and decide the starting table and players
* ``GameMain``, the View: its users

StartChoices CRC Card
^^^^^^^^^^^^^^^^^^^^^

Decides which table and players the game starts with.

**Responsibilities**

* **Combine the three sources** in a fixed order of authority: the user's
  ``config.yaml``, what the last game remembered in ``state.yaml``, and the
  packaged defaults.
* **Decide each part separately:** the table, the user player and the
  buddies.
* **Skip a name that does not exist,** with a warning, and fall back to
  the next source, so the game always starts.
* **Publish the answer** under the configuration keys the rest of the
  game reads.

**Collaborators**

* ``ConfigFiles``: its caller
* ``UserState``: the remembered table, players and settings
* ``ConfigManager`` keys: where the answer is stored
* ``PlayerManifest``: which players are buddies
