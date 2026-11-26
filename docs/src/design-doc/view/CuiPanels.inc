WindowPlayerArea CRC Card
^^^^^^^^^^^^^^^^^^^^^^^^^

The betting layout, and the bets on it.

**Responsibilities**

* **Draw the table layout** and place each player's bets on it, with
  their amounts and, where flags apply, how they look. See
  :ref:`Bet Flags`.
* **Offer two views:** all players together, or one player in detail. The
  user chooses.
* **Follow the roster** as players join, leave or switch.
* **Update from events,** not by polling the model.

**Collaborators**

* ``PlayerAreaAllPlayers``, ``PlayerAreaOnePlayer``: the two views
* ``WorkOrderDispatcher``: what tells it things changed
* ``ColorManager``: each player's color
* ``CrapsReaders``: the players and their bets
* ``LayoutPlayerArea``

WindowAnimation CRC Card
^^^^^^^^^^^^^^^^^^^^^^^^

The dice, the shooter and the last roll.

**Responsibilities**

* **Animate the dice** frame by frame from a timer, at the speed the user
  chose, or not at all.
* **Say when the dice have landed.** It alone knows, and it then releases
  the held events. See :ref:`Roll Animation Synchronization`.
* **Show the shooter** and the last roll.
* **Start over** when the table changes.

**Collaborators**

* ``Gen::TimerManager``: the frame timer
* ``CuiThread``: where its timer queues the drawing
* ``WorkOrderDispatcher``: tells it a roll started and ends the hold
* ``ColorManager``, ``CrapsReaders``

ColorManager CRC Card
^^^^^^^^^^^^^^^^^^^^^

Colors for the game and for each player.

**Responsibilities**

* **Define the colors** the display uses: for the point, seven out, pass
  line winners, dialogs and errors.
* **Give each player a color** and keep it as long as the player is at the
  table.

**Collaborators**

* ``WorkOrderDispatcher``: syncs it with the roster
* The panels and dialogs: ask it for colors
