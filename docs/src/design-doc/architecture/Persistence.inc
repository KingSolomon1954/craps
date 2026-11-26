What Is Saved
^^^^^^^^^^^^^

Everything |AppName| keeps from one run to the next is in YAML files in the
user's directories. There is no database. The directories are described in
:ref:`Directory Layout`.

.. list-table::
   :header-rows: 1
   :widths: 24 24 52

   * - File
     - Owner
     - Contents
   * - ``tables/<name>.yaml``
     - ``TableManager``
     - A table's rules and limits, its bank, and its statistics and the
       history of its sessions.
   * - ``players/<name>.yaml``
     - ``PlayerManager``
     - A player's name, bank and statistics (the ``Player`` section) and
       their preferences (the ``Prefs`` section): Last Fills, QuickBets,
       AutoOdds, and the buddy follow setting.
   * - ``tables.yaml``, ``players.yaml``
     - The managers
     - Manifests. They list the tables and players that exist, with their
       descriptions, and say which players are buddies.
   * - ``state.yaml``
     - ``GameMain``, ``UserState``
     - The table and players in use at quit, and the animation speed and
       roll history format, so the next start can resume.
   * - ``config.yaml``
     - The user
     - Preferences edited by hand. The game never rewrites it.

The packaged files hold the defaults. The first start copies the ones that
are missing into the user's directories, and never overwrites one that is
there.

Reading and Writing
^^^^^^^^^^^^^^^^^^^

All of these files are handled by ``YamlFile``, which holds a file as a
document of named sections:

* The model classes see only their own section, as a YAML node. They know
  how to convert to and from it, and nothing about files. ``CrapsTable``
  reads and writes the ``Table`` section, ``Player`` the ``Player``
  section, ``PlayerPrefs`` the ``Prefs`` section.
* The managers own the ``YamlFile`` objects, and decide which sections a
  file has and when to read and write it.
* A section the game does not know of is kept and written back, so
  something added by hand or by a later version survives.
* ``save()`` writes to a temporary file in the same directory and renames
  it over the real file. A failed or interrupted write leaves the old file
  intact.
* Loading is forgiving where it can be. An entry in a preferences section
  that cannot be read is logged and skipped. The one required section of a
  file must be present, or the file fails to load.

When Files Are Written
^^^^^^^^^^^^^^^^^^^^^^

Files are written at particular moments, not after every change:

* **At quit,** the table, every player, and ``state.yaml``.
* **When the user player changes,** the player who left.
* **When a player leaves the table,** that player.
* **When the table changes,** the old table, and every player.
* **When a player is created,** that player and the manifest.
* **When a player is renamed,** the manifest at once. The player's own
  file is written with the next of these.
* **When the buddy follow setting changes,** the players.

Changes to preferences such as QuickBets or AutoOdds are written with the
next of these. A game that ends without a normal quit, such as a crash or
a kill signal, loses what has changed since the last of them. A failure to
save is logged, and does not stop the game or the other saves.

Order matters at quit. The table first ends each player's session, which
merges it into the player's statistics, and saves itself. Only after that
are the players saved. See :ref:`Shutdown and the Threads`.

Configuration Layers
^^^^^^^^^^^^^^^^^^^^

Settings that are not saved by the game, but supplied to it, come from
``ConfigManager``, built on ``Gen::MultiLayerConfig``. A setting is a
string keyed by a dotted name, such as ``animation.speed``. The layers,
each overriding the one before, are:

#. Built-in defaults.
#. Files: the packaged ``sys-config.yaml``, then the user's ``config.yaml``.
#. Environment variables.
#. The command line.
#. A session layer, the highest, holding settings the game itself changes
   while it runs, such as the animation speed chosen from the Configure
   menu.

A value is looked up by key and the highest layer that has it wins. It is
read as a string, an integer or a boolean. The key names are constants in
``ConfigManager``. The directory settings can be changed from the command
line, which is how a development run uses the source tree, and a test
run uses a scratch directory.

What the last run remembered, in ``state.yaml``, is used only when the
user's ``config.yaml`` does not say. For example, the animation speed in
``config.yaml`` wins, and otherwise the speed from the last game is
used. The same rule applies to the roll history format.

``StartChoices`` turns these settings and the remembered state into the
table and players the game starts with. See
:ref:`Choosing the Table and Players`.
