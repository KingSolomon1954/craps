Why Events
^^^^^^^^^^

The Model, Controller and View do not call each other's code to make
things happen. A layer that wants something done publishes an event, and a
layer that wants to know what happened subscribes to the events it cares
about. The layers share only the event definitions. A graphical View can
therefore take the place of the console one without changes to the
engine, and the engine can be tested without a View.

The View also reads state directly, through ``CrapsReaders``, for example
the table name or a player's balance. Events carry changes. Readers answer
questions about how things are now. See :ref:`Thread Rules`.

The Event Bus
^^^^^^^^^^^^^

``Gen::EventManager`` is the event bus, a thread safe publish and
subscribe queue.

* Events are plain structs. A subscriber names the struct type it wants,
  and gets only that type.
* ``publish()`` copies the event onto a queue and returns at once. It never
  runs handlers itself.
* One worker thread, the event thread, takes events off the queue in the
  order they were published and calls each subscriber, one after another.
* A handler that publishes new events adds them to the back of the queue.
  They are delivered after every event already waiting.
* A handler that throws does not stop the bus. The exception is dropped.
  Handlers should catch and report their own errors.
* Subscriptions last as long as the process. There is no unsubscribe.

Event Categories
^^^^^^^^^^^^^^^^

Every event is a struct derived from ``GameEvent`` and is one of three
kinds, named by prefix. The full list is ``Ctrl::EventType`` in
``GameEvents.h``.

.. list-table::
   :header-rows: 1
   :widths: 14 18 68

   * - Prefix
     - Direction
     - Meaning
   * - ``Req``
     - View to Controller
     - Please do something. Examples: ``ReqRollDice``, ``ReqMakeBet``,
       ``ReqUndoBet``, ``ReqSwitchTable``, ``ReqTerminate``.
   * - ``Rsp``
     - Controller to View
     - The answer to one request, carrying success or an error. Examples:
       ``RspMakeBet``, ``RspSwitchTable``, ``RspApplyQuickBets``.
   * - ``Usl``
     - Any layer to whoever listens
     - Unsolicited. Something happened. Examples: ``UslBettingOpened``,
       ``UslDiceNewValue``, ``UslBetResolved``, ``UslTableSwitched``.

Not every request has a response. A request whose outcome is shown by the
unsolicited events that follow it, such as ``ReqRollDice``, has none, and
neither does a request that cannot fail, such as ``ReqSetAnimationSpeed``.

The unsolicited events fall into groups:

* **The roll.** Betting closed, dice thrown, dice value, bets resolved
  start and end, point established, seven out, pass line winner, new
  shooter, betting opened.
* **Bets.** Bet made, removed, changed, resolved, or failed evaluation.
* **State for displays.** Table and player bets, session stats, last roll
  stats and balances. These let a display keep a cache without polling.
* **Roster and settings.** Player joined or left the table, user player
  switched, player renamed, table switched, roll history format changed.
* **Automation.** AutoOdds applied.

Event Sources and Correlation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each event records where it came from (``EventSource``: Model, View or
Controller) and carries two numbers:

* A unique **event id**.
* A **correlation id**. A new event starts with its own unique value. The
  handler that answers a request copies the request's correlation id into
  the response. The sender uses it to tell which of its requests a
  response answers.

Who Sends and Who Listens
^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 24 38 38

   * - Layer
     - Publishes
     - Subscribes to
   * - View (``lib-cui``)
     - ``Req*``, through ``CuiSender``
     - ``Rsp*`` and ``Usl*``, through ``CuiReceiver``, which turns each into
       a work order for the CUI thread
   * - Controller (``lib-controller``)
     - ``Rsp*``, and ``Usl*`` for what it does itself (for example a table
       switch)
     - ``Req*``, through ``EventHandler``, and the engine events it
       reacts to (for example betting opening, for AutoOdds)
   * - Model (``lib-craps``)
     - ``Usl*``, for everything that happens at the table
     - Nothing

The event structs are defined in the Controller library, and the engine
includes that header to publish them. That is a deliberate relaxation of
strict MVC layering. The engine does not call the Controller.

Delivery Order
^^^^^^^^^^^^^^

The order guarantees are simple, and the design depends on them:

#. Events are delivered in the order they were published, to every
   subscriber of their type.
#. One handler runs to completion before the next event is delivered.
#. A response is therefore always delivered after anything the handler
   published before it.
#. The CUI thread's work queue is also first in, first out. The View sees
   events in the order the engine published them. ``CuiReceiver`` does not
   reorder, and the View can choose to hold some back. See
   :ref:`Roll Animation Synchronization`.

Adding an Event
^^^^^^^^^^^^^^^

A new event touches these places:

#. ``GameEvents.h``: the struct and an ``EventType`` value.
#. The publisher, and for a request the ``EventHandler`` subscription and
   handler.
#. For the View: ``WorkOrder.h`` (the variant), ``CuiReceiver`` (the
   subscription and handler), ``WorkOrderDispatcher`` (``process()``, and
   the pass-through list if it is a response), and ``CuiSender`` if it is a
   request.
