Startup Behavior
^^^^^^^^^^^^^^^^

|AppName| determines its startup behavior by checking whether an always
present file exists. Looks for ``$HOME/.local/share/royalcraps/config.yaml``

First Startup
~~~~~~~~~~~~~

If no ``config.yaml`` exists, |AppName| performs the
following steps:

#. Creates the user directory structure for the game in $HOME.
#. Populates the directory structure with packaged default YAML files.
   These include:
   
   * default ``config.yaml``.
   * default player ``unnamed.yaml``.
   * packaged **buddy** player YAML files.
   * packaged table YAML files.
     
#. default ``config.yaml`` specifies player ``unnamed.yaml``.
#. default ``config.yaml`` specifies table ``Las Vegas Strip``.
#. default ``unnamed.yaml`` specifies starting bank at $10,000.
#. |AppName| continues start up as normal
#. Game begins with full screen :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. If user-player name == ``unnamed`` then message area displays a
   reminder to change name.

Subsequent Startups
~~~~~~~~~~~~~~~~~~~

#. Reads ``$HOME/.local/share/royalcraps/config.yaml``.
#. Loads the last player YAML.
#. Loads last table YAML.
#. Loads last set of buddy players.
#. Game begins with full screen :ref:`ScreenCrapsTable <ScreenCrapsTable CRC Card>`.
#. if player name == ``unnamed`` then message area displays a
   reminder to change name.

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

