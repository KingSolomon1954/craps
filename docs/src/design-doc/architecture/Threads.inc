Thread Inventory
^^^^^^^^^^^^^^^^

|AppName| is one process with four threads, plus the signal handling done
by the main thread. Each thread has a single job.

.. list-table::
   :header-rows: 1
   :widths: 18 22 60

   * - Thread
     - Started by
     - What it does
   * - Main
     - The operating system
     - ``GameMain`` builds the application in its constructor, then blocks
       in ``SignalHandler::waitForTerminate()`` until a termination signal
       arrives or the user quits. It then runs the shutdown.
   * - Event
     - ``Gen::EventManager``, the first time it is used
     - Takes each published event off the queue and runs its handlers, one
       event at a time. All the work of the Controller and the Craps
       Engine that responds to the View happens here: handling requests,
       placing and resolving bets, rolling the dice, saving files.
   * - CUI
     - ``Cui::CuiThread``, when the Console UI starts
     - Owns the screen. It reads the keyboard, runs the work queue
       (keys, events from the engine, drawing requests), and draws. Only
       this thread uses ncurses and the View's windows and menus.
   * - Timer
     - ``Gen::TimerManager``
     - Runs every timer from one thread. The only timer user today is the
       dice animation. Its callback does no drawing. It places a draw
       request on the CUI thread's work queue.

The main thread installs ``SignalHandler`` first, before any other thread
exists, so that every thread inherits a mask that blocks the signals. Only
the main thread waits for them.

Thread Rules
^^^^^^^^^^^^

#. **Only the CUI thread touches the screen.** Anything that wants
   something drawn, from any thread, queues a work order for the CUI
   thread.
#. **Events are the way across threads.** The View sends requests with
   ``CuiSender``, which publishes onto the event queue. The engine and
   Controller answer by publishing, and ``CuiReceiver`` turns what the
   View subscribed to into work orders for the CUI thread. See
   :ref:`Events`.
#. **The Controller and the Craps Engine are not thread safe in
   themselves.** They get away with it because nearly all of their work
   runs on the single event thread, and the event thread runs one handler
   at a time. A handler can therefore assume nothing else is changing the
   engine while it runs. ``PlayerManager`` also has a mutex of its own.
#. **The View may read the model without going through events.** It does
   this through ``CrapsReaders``, from the CUI thread, while the event
   thread may be changing the model. These readers take no locks. The
   design relies on these habits to make that safe enough:

   * The View only reads. It never changes the model except by sending a
     request.
   * Displays that need consistent numbers keep a cache, updated by events,
     instead of reading the engine on every draw.
   * An object the View might be reading is not deleted while the game
     runs. For example, when the game switches tables, the old table is
     retired and kept until the game ends.

#. **A handler does not wait.** Handlers on the event thread must finish
   quickly, and must not block on the CUI thread, because every other
   event waits behind them.

Shutdown and the Threads
^^^^^^^^^^^^^^^^^^^^^^^^

Quitting from the game sends ``ReqTerminate``. Its handler asks ``GameMain``
to release the main thread, as a signal does. The main thread then shuts
down in this order:

#. The View stops first, which stops the CUI thread and gives the terminal
   back. Nothing is reading the model after this. Events published later
   are dropped by the View.
#. The table shuts its players down and saves itself.
#. ``PlayerManager`` saves the players, and the user state is saved.
#. The event and timer threads end when their singletons are destroyed as
   the process exits. The event queue is emptied first.

Processing Flow of a Bet
^^^^^^^^^^^^^^^^^^^^^^^^

A bet shows the pattern that most user actions follow.

.. code-block:: text

    CUI thread                      Event thread
    ----------                      ------------
    key pressed
    menu, dialogs, OperationManager
    CuiSender::reqMakeBet()  ---->  queue --> EventHandler::onReqMakeBet
                                              CrapsTable adds the bet
                                              publishes UslBetMade, ...
                             <----  queue <-- publishes RspMakeBet
    CuiReceiver: work order
    WorkOrderDispatcher
    OperationManager shows the result
    windows redraw from the events

The View does not wait for the answer. ``OperationManager`` remembers the
operation in progress and finishes it when the response arrives, matching
it by correlation id. The next key is handled in the meantime.

Processing Flow of a Roll
^^^^^^^^^^^^^^^^^^^^^^^^^

Rolling the dice follows the same path, but the engine does the whole roll
in a single call on the event thread. ``ReqRollDice`` clears the undo
history, since the bets are about to be decided, and then calls the table's
``rollDice()``. That call publishes the events for the roll, in order, from
betting closed to betting opened. There is no response event. The results
come back as the unsolicited events. The CUI holds them until the dice have
landed. See :ref:`Roll Animation Synchronization`.
