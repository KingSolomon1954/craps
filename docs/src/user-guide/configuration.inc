.. Conventions are the same as in statistics.inc: **bold** for the first use
.. of a defined term, *italic* for emphasis, ``literal`` for file names,
.. settings and commands, and Notes for things that commonly surprise people.

Configuring |AppName|
=====================

|AppName| needs no configuration. The first time it runs it creates what
it needs and puts you at a table, ready to play. This section is for when
you want something different: always starting as a particular player, for
example, or starting without the other players.

Everything you can set lives in one small text file named ``config.yaml``.

Where the Files Are
-------------------

|AppName| keeps your files in your own home directory. The locations follow
the usual Linux (XDG) conventions, and each can be moved by setting the
environment variable shown.

.. list-table::
   :header-rows: 1
   :widths: 34 66

   * - Location
     - What is there
   * - ``~/.config/royalcraps``
       (``$XDG_CONFIG_HOME``)
     - ``config.yaml``, your settings. This is the only file in this
       section that you edit.
   * - ``~/.local/share/royalcraps``
       (``$XDG_DATA_HOME``)
     - Your players and tables, with their bank balances and statistics.
       |AppName| saves these for you.
   * - ``~/.local/state/royalcraps``
       (``$XDG_STATE_HOME``)
     - ``state.yaml``, which remembers where you left off, and the log
       file ``royalcraps.log``.

.. note::

   Players and tables are changed by the game every time you play. Do not
   edit them by hand while the game is running, and make a copy first if
   you want to experiment.

The ``config.yaml`` File
------------------------

When |AppName| first runs it creates ``config.yaml`` for you. Every setting
in it is *commented out*, so out of the box the file changes nothing. To
use a setting, remove the ``#`` at the start of its line and change the
value. The game reads the file when it starts, so quit and restart the game
for a change to take effect.

|AppName| never rewrites this file. Your edits and your comments are safe.

A few things to know about the file format, which is called YAML:

* Indentation matters. Use spaces, never tabs, and keep each setting lined
  up under the heading it belongs to.
* A line that begins with ``#`` is a comment and is ignored.
* A list is written one item per line, each starting with a dash.
* File names such as ``arthur.yaml`` are the names of the files in your
  players and tables directories. Quotes around them are optional.

If you make a mistake that stops the file from being read, the game tells you
which file and why when it starts. To get a fresh copy, delete
``config.yaml`` and start the game again. It creates a new one, and your
players and tables are not touched.

Choosing the Table and Players
------------------------------

By default |AppName| starts where you last left off: the same table, you
playing as the same player, and with the same other players seated. These
four settings let you override that. Each one is independent of the others,
and any you leave out are decided by where you last left off.

.. code-block:: yaml

   table:
     startWithTable: "las-vegas-strip.yaml"
     startWithUserPlayer: "nathan.yaml"
     startWithBuddyPlayers: true
     startWithTheseBuddyPlayers:
       - "arthur.yaml"
       - "paige.yaml"

``startWithTable``
   Always start at this table.

``startWithUserPlayer``
   Always start as this player, even if you quit last time as someone else.

``startWithBuddyPlayers``
   ``true`` or ``false``, and ``true`` if you leave it out. A **buddy** is
   any other player seated at your table. Set this to ``false`` to always
   start by yourself, no matter who sat with you last time and no matter
   what is listed in ``startWithTheseBuddyPlayers``.

``startWithTheseBuddyPlayers``
   Always start with these buddies, up to five. Leave this out to start
   with the buddies from your last game.

A few examples:

*Always start as nathan, with whoever was at the table last time.*
   Set only ``startWithUserPlayer: "nathan.yaml"``.

*Always start alone.*
   Set only ``startWithBuddyPlayers: false``.

*Always start with the same three players at the Las Vegas table.*
   Set ``startWithTable``, ``startWithUserPlayer`` and
   ``startWithTheseBuddyPlayers``.

How |AppName| Remembers
~~~~~~~~~~~~~~~~~~~~~~~

When you quit, |AppName| writes down your table, your player, your
buddies, your dice animation speed and your roll history format in
``state.yaml``. Next time, any setting you did not make in
``config.yaml`` is taken from there. Your own settings always take
priority over it. If you quit with nobody else at the table, the next
game starts that way too.

The very first time, with no ``state.yaml`` yet, you start at the Las Vegas
Strip table as the player ``unnamed``, with two buddies, ``arthur`` and
``paige``.

``state.yaml`` is only a memory and is always safe to delete. The next game
then starts from your settings, or from the first-time defaults. You never
need to edit it. Anything you want to keep belongs in ``config.yaml``.

.. note::

   The game is built to always start. If ``config.yaml`` names a table or
   player that does not exist, say because of a typing mistake or a player
   you removed, the game ignores that one name, writes a warning to the log
   file and falls back to the next choice (where you left off, then the
   defaults). Check the log if the game does not start as you expected.

Other Settings
--------------

These settings are in ``config.yaml`` too. They are shown commented out with
their default values.

``table: maxSessions``
   How many past sessions each table keeps in its history, for the statistics
   across all sessions. When there are more, the oldest are dropped. The
   default is 50.

``log: debug`` and ``log: trace``
   ``true`` turns on more detailed messages in the log file. This is useful
   when something is wrong and you are asked for the log, and it makes the
   file grow quickly, so leave both ``false`` otherwise.

.. note::

   The file also lists ``rollHistory: scroll``, ``sound`` and
   ``player: maxSessions``
   settings. These are reserved for features that are not in the game yet,
   and changing them has no effect in this version.

Dice Roll Animation Speed
-------------------------

The dice tumble across the table before the roll is shown. The bets are not
settled until the dice land, so you never see a result before the roll. If
you would rather not wait, you can speed the animation up or turn it off.

The speeds are:

``off``
   No animation. The roll shows at once.

``fast``
   Twice as fast as normal.

``normal``
   The usual pace. This is the default.

``slow``
   One and a half times slower than normal.

To change the speed while you play, open the Control menu, choose
Configuration, then Animation Speed (press ``s``). Pick a speed from the
list. It takes effect on the next roll. The current speed is marked.

The speed you pick is remembered in ``state.yaml`` when you quit, so the next
game starts at the same speed. To always start at a particular speed, no
matter what you chose last time, put it in ``config.yaml``::

   animation:
     speed: fast

A speed in ``config.yaml`` is used at the start of every game, and you can
still change it during a game with the menu. The command line option
``--animation-speed`` (see below) overrides ``config.yaml`` for one run.

.. note::

   If the speed is not one of the four words above, the game writes a
   warning to the log file and uses ``normal``. Unquoted ``off`` is also
   understood in ``config.yaml``.

Roll History Format
-------------------

The roll history under the title bar lists the most recent rolls, newest
first. You can choose how each roll is written. The formats run from the most
compact, which fits the most rolls, to the most detailed:

.. list-table::
   :header-rows: 1

   * - Format
     - Looks like
   * - ``1``
     - ``7 8 6 10``
   * - ``2``
     - ``7,8,6,10``
   * - ``3``
     - ``7(34) 8(44) 6(33) 10(65)``
   * - ``4``
     - ``7(3,4) 8(4,4) 6(3,3) 10(6,5)``

The numbers in parentheses are the two dice. Format 1 is the default.

To change the format while you play, open the Control menu, choose
Configuration, then Format Roll History (press ``f``). Press ``1`` to ``4``,
or move to a line and press Enter. The current format is marked. The whole
history is redrawn at once in the new format and keeps its colors for seven
outs, pass line winners and points.

Like the animation speed, the format you pick is remembered in
``state.yaml`` when you quit. To always start with a particular format, put it
in ``config.yaml``::

   rollHistory:
     format: 3

A format in ``config.yaml`` is used at the start of every game, and you can
still change it during a game with the menu.

Overriding Settings Temporarily
-------------------------------

Settings come from several places. When the same setting is given in more
than one, the later one in this list wins:

#. The program's built-in defaults and the settings shipped with it.
#. Your ``config.yaml``.
#. Environment variables.
#. The command line.

The command line is the quickest way to try something once without editing
any file. These options are available:

``--debug`` and ``--trace``
   Turn on the detailed logging described above for this run only.

``--animation-speed``
   Set the dice roll animation speed for this run: ``off``, ``fast``,
   ``normal`` or ``slow``. Anything else stops the game with a message
   listing the choices.

``--usr-config-dir``, ``--usr-data-dir`` and ``--usr-state-dir``
   Use another directory in place of the config, data or state locations
   above. Pointing all three at a new, empty directory gives you a brand new
   game, exactly as on a first run, without touching your real players. This
   is a good way to try the game out safely.

``--help``
   Lists every option.

The environment variable ``ROYALCRAPS_START_TABLE`` chooses the table for
a run, taking priority over ``config.yaml``.
