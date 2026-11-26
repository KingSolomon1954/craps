Global Data
^^^^^^^^^^^

Global data is kept to a short list, held as static pointers in the class
``Gbl`` (``Globals.h``). ``GameMain`` creates each one at startup, in the
order shown, and owns it until shutdown.

.. list-table::
   :header-rows: 1
   :widths: 24 76

   * - Global
     - Purpose
   * - ``Gbl::pBuildInfo``
     - Version and build information for the banner and the logs.
   * - ``Gbl::pConfigMgr``
     - The resolved configuration. See :ref:`Configuration Layers`.
   * - ``Gbl::pEventHandler``
     - Subscribes to the View's requests. See :ref:`Events`.
   * - ``Gbl::pTableMgr``
     - Owns the tables and their files.
   * - ``Gbl::pTable``
     - The table being played. The Controller sets it when the game
       starts and when it switches tables.
   * - ``Gbl::pPlayerMgr``
     - Owns the players, their preferences and their files.

There are also a few singletons, each created on first use: ``EventManager``,
``TimerManager``, ``Logger``, ``UndoManager``, and in the View ``CuiMain``,
``CuiThread``, ``CuiSender``, ``CuiReceiver``, ``SurfaceManager``,
``OperationManager`` and the individual windows, menus and dialogs.

Nothing else is global. Data that more than one object needs travels in
events, or is read through ``CrapsReaders``. See :ref:`Thread Rules` for who
may read what, and from which thread.

Game State
^^^^^^^^^^

The state of the game lives in the Craps Engine, in ``CrapsTable``. A table
is always in one of these states:

.. code-block:: text

    Come-out roll  ---- 4,5,6,8,9,10 ------>  Point on
         ^   |                                  |   |
         |   +-- 2,3,7,11,12: stays            |   |
         |                                      |   |
         +-------- point made (pass line wins) -+   |
         +-------- seven out (next shooter) --------+

* **Come-out roll** (no point). A roll of 4, 5, 6, 8, 9 or 10 sets the
  point and publishes ``UslPointEstablished``. Any other number leaves the
  table in the come-out state.
* **Point on.** A roll equal to the point publishes ``UslPassLineWinner``.
  A 7 publishes ``UslSevenOut`` and passes the dice to the next shooter.
  Either ends the point, and the table goes back to the come-out roll.

Independently of that, betting is **open** or **closed**. Betting is open
except for the length of a roll. ``UslBettingClosed`` and
``UslBettingOpened`` bracket every roll. The Controller refuses requests that
change bets while betting is closed, and refuses a table switch.

Only the engine changes this state, and only when it handles a request
or a roll. Everything else in the application learns of it from events.

View State
^^^^^^^^^^

The View keeps its own state, none of which the engine knows of:

* **The screen.** ``SurfaceManager`` holds the stack of what is on the
  screen. The table screen is at the bottom and the menus and dialogs
  stack above it. Keys go to the top surface.
* **The operation in progress.** ``OperationManager`` tracks at most one
  multi-stage user operation, and the stage it has reached. See
  :ref:`OperationManager CRC Card`.
* **Held events.** ``WorkOrderDispatcher`` may be holding events while the
  dice are rolling. See :ref:`Roll Animation Synchronization`.
* **Display caches.** Windows keep the numbers they show, such as the house
  figures and the roll list, and update them from events.

Between them these cover everything the user sees. When the game changes
tables, the Controller sends ``UslTableSwitched`` and then a snapshot of
the new table's state, and the View resets its caches from them.

The Application's Own State
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The process itself is in one of three states, owned by ``GameMain``:

#. **Starting.** The constructor creates the globals, then the View. See
   :ref:`Startup, Shutdown and Directory Layout`.
#. **Running.** The main thread waits for a termination signal or for
   ``ReqTerminate``.
#. **Stopping.** ``explicitShutdown()`` stops the View, then saves the
   table, the players and the user state. See :ref:`Thread Inventory`.

A failure while starting, such as an invalid configuration, ends the
process with a message and a non-zero exit status. A failure once the game
is running is reported and logged, and play goes on. See
:ref:`Error Handling`.
