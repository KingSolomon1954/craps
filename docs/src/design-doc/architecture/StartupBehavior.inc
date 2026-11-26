Startup Behavior
^^^^^^^^^^^^^^^^

|AppName| must bring the user straight into a working game, with no
configuration steps, on a fresh installation and on every start after.
Packaged files are read-only and shared, so everything the game writes
(players, tables, preferences, what was last played) lives in the user's
own directories and is created from the packaged defaults when missing.

Directory Layout
~~~~~~~~~~~~~~~~

The layout follows the XDG Base Directory conventions. Each ``$XDG_*``
variable, when set, replaces the default shown.

.. list-table::
   :header-rows: 1
   :widths: 30 20 50

   * - Directory
     - Access
     - Contents
   * - ``<exe dir>/../share/royalcraps``, else
       ``/usr/local/share/royalcraps``, else
       ``/usr/share/royalcraps``
     - read-only
     - Packaged files: ``config/sys-config.yaml``,
       ``config/default-config.yaml``, ``tables/``, ``players/``,
       ``audio/``, ``images/``. The first location holding
       ``config/sys-config.yaml`` is used, so an unpacked tarball works
       wherever it is unpacked.
   * - ``$XDG_CONFIG_HOME/royalcraps`` (``~/.config/royalcraps``)
     - read/write
     - ``config.yaml``: the user's preferences. Hand edited; never
       rewritten by the game. Delivered with every setting commented
       out.
   * - ``$XDG_DATA_HOME/royalcraps`` (``~/.local/share/royalcraps``)
     - read/write
     - ``players/`` and ``tables/`` (each with its manifest,
       ``players.yaml`` and ``tables.yaml``), ``audio/``, ``images/``.
   * - ``$XDG_STATE_HOME/royalcraps`` (``~/.local/state/royalcraps``)
     - read/write
     - ``state.yaml``: last table and players, written at quit; a
       memory only, see `Choosing the Table and Players`_.
       Also the log file.

The configuration is resolved in layers, each overriding the one before:
built-in defaults, the files (packaged ``sys-config.yaml``, then the
user's ``config.yaml``), environment variables, and finally the command
line. The directories themselves can be overridden
with ``--sys-config-dir``, ``--usr-config-dir``, ``--usr-data-dir`` and
``--usr-state-dir``. Pointing the user directories at a scratch location
gives a genuine first startup every time, which is how first startup is
tested; pointing them at the packaged files is how development runs
from a source tree.

Logging During Startup
~~~~~~~~~~~~~~~~~~~~~~

The log file is in the user state directory, which is not known until
configuration is resolved, yet startup itself needs to log. The logger
therefore holds messages in memory (and still echoes them to the
console) until it is given its file, then writes them to the file ahead
of everything else with their original timestamps. If startup fails
before then, the held messages have already appeared on the console.
Debug and trace messages are only kept once those levels are known, so
the earliest messages are informational, warnings and errors.

First Startup
~~~~~~~~~~~~~

First startup is detected by the absence of the user's ``config.yaml``.
The same population work also runs if the user's ``players.yaml`` or
``tables.yaml`` is missing, so a user who deletes their data directory
gets a working game back rather than an error.

|AppName| performs the following steps:

#. Creates the user directories.
#. Copies the packaged files into the user directories. A file is
   copied only if it does not already exist there, so existing players
   and their statistics are never overwritten. Each file is copied to a
   temporary name and renamed into place. These include:

   * player ``unnamed.yaml``.
   * packaged **buddy** player YAML files.
   * packaged table YAML files.
   * the ``players.yaml`` and ``tables.yaml`` manifests.

#. Copies ``default-config.yaml`` to the user's ``config.yaml``. This is
   done last, so an interrupted first startup is simply repeated on the
   next start.
#. The delivered ``config.yaml`` says nothing about the table or
   players, so the packaged defaults in ``sys-config.yaml`` apply: table
   ``las-vegas-strip.yaml``, user player ``unnamed.yaml`` and two buddy
   players.
#. The delivered ``unnamed.yaml`` specifies a starting bank of $10,000.
#. |AppName| continues start up as normal.
#. Game begins with full screen :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. If the user player's name is ``unnamed`` then a message reminds the
   user to change the name. Until the main screen has a message area
   this is an acknowledged dialog.

Subsequent Startups
~~~~~~~~~~~~~~~~~~~

#. Reads ``config.yaml`` from the user's config directory.
#. Reads ``state.yaml`` from the user's state directory, if there is one.
#. Decides the table and players as described in
   `Choosing the Table and Players`_.
#. Loads the user player, the table and the other players from the
   user's data directory.
#. Game begins with full screen :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. If the user player's name is ``unnamed`` then the same reminder is
   shown.

At quit, after the table and players are saved, the table, the user
player and the other players seated at the table are written to
``state.yaml``.

Choosing the Table and Players
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Three sources can say which table and players to start with, in this
order of authority: what the user wrote in ``config.yaml``, what the
last game left in ``state.yaml``, and the packaged defaults in
``sys-config.yaml``. The user's ``config.yaml`` always wins;
``state.yaml`` only fills in what it leaves out. Each setting is
decided on its own.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``config.yaml`` setting
     - Meaning
   * - ``startWithTable``
     - Always start at this table. If absent, the last table, else the
       packaged default.
   * - ``startWithUserPlayer``
     - Always start as this player, whoever the user played last time.
       If absent, the last user player, else the packaged default.
   * - ``startWithBuddyPlayers``
     - ``true`` (the default) or ``false``. ``false`` means start with
       no buddy players, regardless of anything else, including the
       buddies of the last game.
   * - ``startWithTheseBuddyPlayers``
     - A list of up to five buddy players to always start with. If
       absent, the buddies of the last game, else the packaged default
       buddies. Ignored when ``startWithBuddyPlayers`` is ``false``.

The packaged defaults are ``defaultTable``, ``defaultUserPlayer`` and
``defaultBuddyPlayers`` in ``sys-config.yaml``. They are separate keys
because a value present in ``sys-config.yaml`` under the names the user
uses could not be told apart from something the user wrote. A game that
was left with no buddies is remembered as having none; the default
buddies are only for when there is no ``state.yaml``.

The game must always be able to start, so a file name that does not
exist in the user's tables or players directory (a typing mistake, or a
player that was deleted) is skipped with a warning in the log and the
next source is tried; for buddies, just that one buddy is dropped. The
user player is never also a buddy. A ``state.yaml`` that cannot be read
is ignored, and deleting it is always safe.

The result is stored under the keys the rest of the game reads:
``table.startTable``, ``table.userIsPlayer`` and
``table.startPlayers``.

.. note::

   Packaged files added by a later release (a new table or buddy) are
   not copied into an existing installation, and are not in the user's
   manifests. A way to deliver them is not yet defined.

Player Configuration Options
""""""""""""""""""""""""""""

At any time, the player can select ``Control -> Configure`` to perform
the following operations available from the ``Configure`` menu:

Rename Player
    Prompts for a new name and a brief player description.
    
    Renames the currently active player to the new name.
    
    The currently active player YAML file is renamed accordingly.
    
    Game carries on. Screens are refreshed with new name.

New Player
    Prompts for the name and a brief player description.
    
    Creates a new player YAML file.
    
    Name must be unique among the player files.
    
    Does *not* automatically switch to new player.

    Consider option for UI to ask to switch to it immediately.
    
    New player starts with these defaults:
    
    * Bank: ``$10,000``
    * Empty stats and session history

Choose Player
    Displays a list of available players and prompts the player to
    select one.

    Selecting the already current player is a No-op.

    When switching players:

    * Clears current player active bets if any.
    * Saves the current player to YAML file.
    * Loads the selected player's YAML file.
    * Game continues using the currently active table.

