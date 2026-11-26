The Console User Interface, in ``lib-cui``, runs in a terminal using
ncurses. It needs a terminal of at least 41 rows by 100 columns. If the
terminal is smaller, the game asks it to resize, and if that fails the game
stops with a message that says what size is needed.

The View is separated from the rest of the game as described in
:ref:`Events`. It sends requests, listens for events, and may read the model
through ``CrapsReaders``. It has no say in the game's rules.

Structure
^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Group
     - Classes
   * - Entry and messaging
     - ``CuiMain``, ``CuiThread``, ``CuiSender``, ``CuiReceiver``,
       ``WorkOrder``, ``WorkOrderDispatcher``
   * - Surfaces
     - ``SurfaceBase`` and its specializations ``ScreenBase``,
       ``MenuBase`` and ``DialogBase``. ``PanelBase`` is for the windows
       inside a screen.
   * - Managers
     - ``SurfaceManager``, ``LocationManager``, ``OperationManager``,
       ``ColorManager``
   * - The table screen
     - ``ScreenCrapsTable`` and its panels
   * - Menus
     - ``MenuBetting``, ``MenuControl``, ``MenuConfigure``,
       ``MenuPlayers``, ``MenuTable``, ``MenuBetFlags``,
       ``MenuNavBarViews``, ``MenuOddsBet``, ``MenuPlaceBet``,
       ``MenuPivot``
   * - Dialogs
     - ``DialogPicker``, ``DialogConfirm``, ``DialogBetAmount``,
       ``DialogQuickBetsSetAdd``, ``DialogTextInput``, ``DialogMessage``,
       ``DialogError``, ``DialogDeepRollHistory``
   * - Support
     - ``RollLog``, ``LineInput``, and the ``Layout`` classes that fix the
       size and place of every window

Surfaces
^^^^^^^^

Anything that takes the screen or the keyboard is a **surface**. A surface
is a screen, a menu or a dialog. ``SurfaceManager`` keeps the surfaces in a
stack. The table screen is at the bottom. A menu opened from it goes above
it, a dialog opened from the menu goes above that, and so on. The top
surface gets the keys. When it is closed the one under it is resumed.
Surfaces are not stacked blindly. ``LocationManager`` places each new
surface on the screen, clear of the others where it can.

Panels are different. They are windows inside the table screen, and they
are not on the stack. The screen draws them and the dispatcher updates
them.

The Table Screen
^^^^^^^^^^^^^^^^

``ScreenCrapsTable`` is made of these panels:

.. list-table::
   :header-rows: 1
   :widths: 28 72

   * - Panel
     - Shows
   * - ``WindowTitleBar``
     - The point, the roll count, the table name and its limits.
   * - ``WindowRollHistory``
     - The recent rolls, newest first, colored for the point, seven out
       and pass line winners.
   * - ``WindowPlayerArea``
     - The betting layout and each player's bets, either all players
       together or one player in detail (``PlayerAreaAllPlayers`` and
       ``PlayerAreaOnePlayer``).
   * - ``WindowAnimation``
     - The shooter, the dice and the last roll.
   * - ``WindowHouseBrief``
     - The house's results and the amount on the table.
   * - ``WindowPlayerBrief``
     - The user player's balance, results and bets.
   * - ``WindowNavBar``
     - The menu keys along the bottom.

How a Key Becomes an Action
^^^^^^^^^^^^^^^^^^^^^^^^^^^

#. ``CuiThread`` reads a key and places a work order for it on its own
   queue.
#. ``WorkOrderDispatcher`` gives it to ``SurfaceManager``, which passes it
   to the top surface.
#. A menu acts on the key. It may open another menu, or start an operation
   in ``OperationManager``.
#. When the operation has what it needs, ``OperationManager`` calls
   ``CuiSender``, which publishes the request.

How an Event Becomes a Display
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#. ``CuiReceiver``, on the event thread, receives the event and places a
   work order for it on the ``CuiThread`` queue.
#. ``WorkOrderDispatcher``, on the CUI thread, either holds it, if the dice
   are rolling, or passes it on. See :ref:`Roll Animation Synchronization`.
#. The dispatcher calls the panels and managers that care, with plain
   values taken from the event, and then asks ``SurfaceManager`` to redraw.
   A response goes to ``OperationManager``.
