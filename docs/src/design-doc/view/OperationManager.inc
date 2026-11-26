
Purpose
^^^^^^^

``OperationManager`` runs the multi-stage tasks that the person starts from
the CUI, one at a time.

A task, called an **operation**, may involve navigating a chain of menus and
dialogs, obtaining the person's confirmation, issuing an asynchronous request
to the Craps Engine, processing the corresponding response, and presenting
any resulting error.

The work is split in two:

* Each operation is its own class, derived from ``Operation``, in
  ``lib-cui/src/operations/``. It knows its own steps, what to show next, and
  what request to send. It exists only while its operation is running.
* ``OperationManager`` starts an operation, remembers what it is waiting for,
  and hands it each dialog answer and each response. It knows nothing of what
  any operation does.

``OperationManager`` is not specific to betting. It supports any CUI
operation that needs coordinated interaction across menus, dialogs and
asynchronous request/response events.

Use Cases
^^^^^^^^^

Making a bet
~~~~~~~~~~~~

A player navigates through one or more betting menus to configure a bet. When
the player has completed the required selections, the menu calls
``OperationManager::makeBet()``, which starts a ``MakeBetOperation``. The
operation issues a ``ReqMakeBet`` request event to the Craps Engine and waits.

* **Success:** The betting menu chain is unwound, returning the player to the
  appropriate underlying screen, and the operation finishes.
* **Failure:** An error dialog is presented. After the player acknowledges the
  error, the operation finishes and the player is left where the operation
  says: for a bet, back at the table.

Quitting the game
~~~~~~~~~~~~~~~~~

When the player requests to quit, the menu calls
``OperationManager::confirmQuit()``. A ``QuitOperation`` presents a
``DialogConfirm``.

* **Confirmed:** The operation publishes the quit request.
* **Cancelled:** The operation finishes without issuing the request, and the
  player returns to the preceding screen.

Other operations follow the same pattern.

Responsibilities
^^^^^^^^^^^^^^^^

* **Start an operation.** Make the ``Operation`` the menu asked for, if none
  is running, and tell it to start.

* **Track what it is waiting for.** Hold the stage (a response, a pick, a text
  entry, a confirmation or an error acknowledgement) and the correlation ID
  of the request in flight.

* **Pass on dialog answers.** Hand a pick, a text, a confirmation or an
  error acknowledgement to the running operation, if it is the kind of answer
  it is waiting for.

* **Correlate requests and responses.** Pass a response to the running
  operation only if it carries the correlation ID of the request in flight.
  Anything else is logged and ignored.

* **Show errors and notices.** Present the error dialog or the notice an
  operation asks for.

* **Release finished operations.** Keep the one that has just finished until
  the hook that finished it returns, then drop it when the next one starts.

* **Keep the AutoOdds status line.** The one piece of state that does not
  belong to an operation.

Operation CRC Card
^^^^^^^^^^^^^^^^^^

One multi-stage task, from the first dialog to the last.

**Responsibilities**

* **Say what the first dialog is** (``start()``).
* **Decide what each answer means** and what comes next: another dialog, a
  request, a notice or the end. It overrides only the hooks its task can
  receive: ``onPick``, ``onText``, ``onConfirm``, ``onQuickBetsChoice``,
  ``onErrorAcknowledged`` and the ``onRsp`` hook for each response.
* **Own its state.** The choices offered, the step it is on and what the
  person has picked are members of the operation, so a finished operation
  leaves nothing behind.
* **Say where the person is left** after success, cancel and error: back on
  the menu that started it, or unwound to the table.

An operation reaches the screen only through the helpers the base class
provides (``pickFrom``, ``askText``, ``askConfirmation``, ``showError``,
``showNotice``, ``awaitResponse``, ``finish`` and ``finishAtRoot``). A hook
that is not overridden logs a warning and ignores the response, or ends the
operation if it is a dialog answer, so that the person is never left stuck.

**Collaborators**

* ``OperationManager``: starts it and passes it what happens
* ``CuiSender``: publishes its request
* ``CrapsReaders``: what it needs to know to offer choices
* ``SurfaceManager``: unwinds the menus
* The dialogs: ``DialogPicker``, ``DialogTextInput``, ``DialogConfirm``,
  ``DialogError``

Operations
^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Operation
     - Started from
   * - ``MakeBetOperation``, ``RemoveBetOperation``, ``UndoBetOperation``
     - The betting menus and ``DialogBetAmount``. See :ref:`Betting`.
   * - ``BetFlagsOperation``
     - ``MenuBetFlags``. See :ref:`Bet Flags`.
   * - ``ApplyQuickBetsOperation``, ``SaveToQuickBetsOperation``,
       ``ManageQuickBetsOperation``, ``ConfigureAutoOddsOperation``,
       ``SetAutoOddsOperation``
     - The betting menu, ``DialogBetAmount`` and ``MenuConfigure``. See
       :ref:`Bet Automation`.
   * - ``ShowPlayersOperation``, ``SwitchPlayerOperation``,
       ``RenamePlayerOperation``, ``CreatePlayerOperation``,
       ``DeletePlayerOperation``, ``BuddyJoinsOperation``,
       ``BuddyLeavesOperation``
     - ``MenuPlayers``. See :ref:`Players and Buddies`.
   * - ``ListTablesOperation``, ``SwitchTableOperation``
     - ``MenuTable``. See :ref:`Table Switching`.
   * - ``BuddyFollowOperation``, ``AnimationSpeedOperation``,
       ``RollHistoryFormatOperation``
     - ``MenuConfigure``.
   * - ``QuitOperation``
     - The main menu.

Adding an Operation
^^^^^^^^^^^^^^^^^^^

#. Derive a class from ``Operation``, in a header under
   ``include/cui/operations/`` and a source file under ``src/operations/``,
   and add the source to ``CMakeLists.txt``.
#. In ``start()``, show the first dialog with ``pickFrom()``, ``askText()`` or
   ``askConfirmation()``, or send the request with ``awaitResponse()``.
#. Override the hook for each answer the operation can receive. Keep each
   hook short: a ``switch`` on a ``Step`` that calls a method named for what
   it does.
#. End with ``finish()``, or ``finishAtRoot()`` to unwind to the table. Do
   not touch the manager's state.
#. Add a function to ``OperationManager`` that calls
   ``begin(std::make_unique<YourOperation>())``.
#. A new kind of response needs an ``onRsp`` hook in ``Operation`` and a
   forwarding function in ``OperationManager``, which names the hook once.

Collaborators
^^^^^^^^^^^^^

* ``Operation`` and its subclasses: the work that is started and run
* ``CuiSender``: publishes the requests the operations issue
* ``WorkOrderDispatcher``: delivers the responses and results to it
* ``SurfaceManager``: pushes, pops and unwinds the menus and dialogs
* Menus, such as ``MenuBetting`` and ``MenuPlayers``: start operations
* Dialogs, such as ``DialogPicker``, ``DialogConfirm``, ``DialogBetAmount``
  and ``DialogError``: collect choices and confirmations, and report
  results

Operating Constraints
^^^^^^^^^^^^^^^^^^^^^

* **One active operation at a time.** The CUI may have at most one operation in progress. A request to start another is refused and does nothing.

* **Asynchronous request/response processing.** Issuing a request does not imply that the operation has succeeded. The operation remains active until its result has been processed and any required follow-up interaction has completed.

* **Correlation-based response matching.** A response must be associated with the request that produced it. Unmatched, stale, or unrelated responses must not advance or complete the active operation.

* **CUI thread ownership.** All operation-state transitions and resulting CUI actions must respect the CUI's thread-ownership rules. Incoming events from other threads must be routed to the CUI thread before they cause UI or operation-state changes.

* **Separation of responsibilities.** ``OperationManager`` starts operations and routes to them; each ``Operation`` decides what happens next; menus collect choices; dialogs communicate information and collect confirmation or acknowledgement; ``SurfaceManager`` manages the surface stack; and the Craps Engine executes game operations and reports their results.

Completion
^^^^^^^^^^

An operation is complete when its intended outcome has been reached and all required user interaction and CUI cleanup have finished.

Depending on the operation, completion may follow a successful response, cancellation of a confirmation, or acknowledgement of an error.

Once complete, the operation is released and the manager is available to start the next one.
