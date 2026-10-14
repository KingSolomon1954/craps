Startup Behavior
^^^^^^^^^^^^^^^^

RoyalCraps determines its startup behavior by checking whether a saved
current game exists.

First Startup
~~~~~~~~~~~~~

If no saved current game is found, RoyalCraps performs the following steps:

#. Create a new player named ``Unnamed``.
#. Set the player's bank to ``10,000``.
#. Select the default table, ``Las Vegas Daytime``.
#. Display :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.

The game is automatically saved under the name ``Unnamed`` when the game
ends.

Player Configuration During First Startup
"""""""""""""""""""""""""""""""""""""""""

During the first session, the player can use
``Control -> Configure`` to perform the following operations.

Rename Player
    Prompts for a new player name and renames the current player.
    The associated save file or files are renamed accordingly.

New Player
    Prompts for the name of a new player.

    Before switching players, RoyalCraps saves the current player.  It
    then creates and activates the new player with the following defaults:

    * Bank: ``10,000``
    * Table: the currently selected table
    * Game state: reset for the new player

Choose Player
    Because no other players exist during the first session, RoyalCraps
    displays the following message and makes no change:

    ``No other players to choose from at this time.``

Subsequent Startups
~~~~~~~~~~~~~~~~~~~

If a saved current game exists, RoyalCraps:

#. Loads the last saved game.
#. Restores the associated player, bank, table, and game state.
#. Displays :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. Allows the player to continue from the point at which the game was
   last saved.

Player Configuration During Subsequent Startups
"""""""""""""""""""""""""""""""""""""""""""""""

During subsequent sessions, the player can use
``Control -> Configure`` to perform the following operations.

Choose Player
    Displays the list of available players and prompts the player to
    select one.

    Before switching players, RoyalCraps saves the current player.  It
    then loads the selected player's saved game, including that player's
    table and game state.

New Player
    Prompts for the name of a new player.

    The current player is saved before the new player is created.  The
    new player is initialized with a bank of ``10,000`` and uses the
    currently selected table with a reset game state.

Rename Player
    Prompts for a new player name and renames the current player.
    The associated save file or files are renamed accordingly.
