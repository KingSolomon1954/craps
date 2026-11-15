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
       rewritten by the game.
   * - ``$XDG_DATA_HOME/royalcraps`` (``~/.local/share/royalcraps``)
     - read/write
     - ``players/`` and ``tables/`` (each with its manifest,
       ``players.yaml`` and ``tables.yaml``), ``audio/``, ``images/``.
   * - ``$XDG_STATE_HOME/royalcraps`` (``~/.local/state/royalcraps``)
     - read/write
     - ``state.yaml``: last table and players, written at quit.
       Also the log file.

The configuration is resolved in layers, each overriding the one before:
built-in defaults, the files (packaged ``sys-config.yaml``, the user's
``config.yaml``, then ``state.yaml``), environment variables, and
finally the command line. The directories themselves can be overridden
with ``--sys-config-dir``, ``--usr-config-dir``, ``--usr-data-dir`` and
``--usr-state-dir``. Pointing the user directories at a scratch location
gives a genuine first startup every time, which is how first startup is
tested; pointing them at the packaged files is how development runs
from a source tree.

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
#. The delivered ``config.yaml`` specifies table ``las-vegas-strip.yaml``,
   the user player ``unnamed.yaml`` and the players at the table
   (``unnamed.yaml`` and two buddies).
#. The delivered ``unnamed.yaml`` specifies a starting bank of $10,000.
#. |AppName| continues start up as normal.
#. Game begins with full screen :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. If the user player's name is ``unnamed`` then a message reminds the
   user to change the name. Until the main screen has a message area
   this is an acknowledged dialog.

Subsequent Startups
~~~~~~~~~~~~~~~~~~~

#. Reads ``config.yaml`` from the user's config directory.
#. Reads ``state.yaml`` from the user's state directory, if there is
   one. Its table, user player and players override those in
   ``config.yaml``. The file is checked before use: if it is
   unreadable, or names a table or player file that no longer exists,
   or its user player is not among its players, it is ignored in its
   entirety and ``config.yaml`` applies. A damaged ``state.yaml`` can
   therefore never prevent the game from starting, and deleting it is
   always safe.
#. Loads the user player, the table and the other players from the
   user's data directory.
#. Game begins with full screen :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. If the user player's name is ``unnamed`` then the same reminder is
   shown.

At quit, after the table and players are saved, the table, the user
player and the players seated at the table are written to
``state.yaml``.

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

