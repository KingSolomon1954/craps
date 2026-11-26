CuiMain CRC Card
^^^^^^^^^^^^^^^^

The entry point of the Console UI.

**Responsibilities**

* **Prepare the terminal:** locale, ncurses, colors and the keyboard, and
  check that the terminal is large enough.
* **Start the View:** create the color manager, the CUI thread and the
  receiver, and show the table screen. Remind a player named "unnamed" to
  choose a name.
* **Know whether the terminal can show Unicode dice pips.**
* **Shut down:** stop the surfaces and the CUI thread, and return the
  terminal to the user.

**Collaborators**

* ``GameMain`` (Controller): starts and stops it
* ``CuiThread``: started and stopped by it, and where its start-up work
  is queued
* ``CuiReceiver``, ``ColorManager``: created by it
* ``SurfaceManager``: shut down by it
* ``ScreenCrapsTable``, ``DialogMessage``: what it first shows
* ``CrapsReaders``: the user player's name

CuiThread CRC Card
^^^^^^^^^^^^^^^^^^

The one thread that may use ncurses.

**Responsibilities**

* **Serialize all View work** on a single queue, in the order it arrived.
* **Read the keyboard** and place each key on the queue.
* **Process the queue,** handing every work order to the dispatcher.
* **Give the dispatcher a regular tick,** so it can give up on a stuck
  animation.
* **Accept work from any thread,** and refuse it once it is shutting down.

**Collaborators**

* ``CuiReceiver``, ``WindowAnimation`` and ``CuiMain``: place work on its
  queue
* ``WorkOrderDispatcher``: processes it

CuiSender CRC Card
^^^^^^^^^^^^^^^^^^

The View's only way to send requests.

**Responsibilities**

* **Build and publish the request events,** one function for each
  request.
* **Return the correlation id** of a request that gets a response, so the
  caller can match it later.

**Collaborators**

* ``OperationManager``: nearly its only caller
* ``Gen::EventManager``: where it publishes
* ``GameEvents``: the requests

CuiReceiver CRC Card
^^^^^^^^^^^^^^^^^^^^

The View's ears. It crosses from the event thread to the CUI thread.

**Responsibilities**

* **Subscribe to the responses and unsolicited events** the View uses.
* **Convert each to a work order** and queue it for the CUI thread, in the
  order it arrived. It does no drawing and holds nothing back.

**Collaborators**

* ``Gen::EventManager``: where it subscribes
* ``CuiThread``: where it queues
* ``WorkOrder``: what it creates

WorkOrderDispatcher CRC Card
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Decides what each piece of View work does.

**Responsibilities**

* **Route work orders:** keys, surface changes, and events.
* **Translate each event** into plain values and call the panels and
  managers that need it. It alone knows which window wants what.
* **Hold the results of a roll** until the dice have landed, and replay
  them in order. Responses and the dice's own events pass straight through.
* **Keep the roster in step.** When players join, leave or change, it
  refreshes the colors and the panels that show players.
* **Reset the displays** when the table changes.

**Collaborators**

* ``CuiThread``: its caller
* ``SurfaceManager``: keys, redraws and surface changes
* ``OperationManager``: responses to requests
* ``WindowAnimation`` and the other panels: what it updates
* ``RollLog``, ``ColorManager``

SurfaceManager CRC Card
^^^^^^^^^^^^^^^^^^^^^^^

Owns the stack of surfaces and the terminal.

**Responsibilities**

* **Keep the stack:** set, push, pop, and unwind to the table screen.
* **Give each surface a place** and its ncurses window, and release them.
* **Send each key to the top surface.**
* **Redraw the whole composition,** or one surface.
* **Close ncurses cleanly** when the View shuts down.

**Collaborators**

* ``WorkOrderDispatcher``: its caller for keys and redraws
* ``LocationManager``: where each surface goes
* ``OperationManager``, menus and dialogs: push and pop surfaces
* ``SurfaceBase`` and its specializations

ScreenCrapsTable CRC Card
^^^^^^^^^^^^^^^^^^^^^^^^^

The full screen on which the game is played.

**Responsibilities**

* **Compose the panels** and own their sizes, from ``LayoutCrapsScreen``.
* **Draw the screen's borders and the junctions** between the panels.
* **Not handle keys.** ``MenuBetting`` and the navigation menus do that.

**Collaborators**

* ``WindowTitleBar``, ``WindowRollHistory``, ``WindowPlayerArea``,
  ``WindowAnimation``, ``WindowHouseBrief``, ``WindowPlayerBrief``,
  ``WindowNavBar``
* ``LayoutCrapsScreen``
* ``SurfaceManager``
* ``MenuBetting``: the menu on top of it
