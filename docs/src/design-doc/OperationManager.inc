
Purpose
^^^^^^^

``OperationManager`` coordinates the lifecycle of multi-stage user operations in the CUI.

An operation represents a user-initiated task that may involve navigating a chain of menus, obtaining user confirmation, issuing an asynchronous request to the Craps Engine, processing the corresponding response, and presenting any resulting errors.

The manager maintains the state of the active operation and determines what action should occur as the operation progresses through its stages.

Although betting is a primary use case, ``OperationManager`` is not specific to betting. It supports any CUI operation that requires coordinated interaction across menus, dialogs, and asynchronous request/response events.

Use Cases
^^^^^^^^^

Making a bet
~~~~~~~~~~~~

A player navigates through one or more betting menus to configure a bet. When the player has completed the required selections, the CUI issues a ``ReqMakeBet`` request event to the Craps Engine.

``OperationManager`` tracks the operation, associates the request with its eventual response, and determines the next action based on the result.

* **Success:** The betting menu chain is unwound, returning the player to the appropriate underlying screen.
* **Failure:** An error dialog is presented. After the player acknowledges the error, the error dialog is dismissed and the betting menu chain is unwound or otherwise restored according to the intended interaction.

Quitting the game
~~~~~~~~~~~~~~~~~

When the player requests to quit, the CUI presents a ``DialogConfirmation``. The manager associates the confirmation response with the pending quit operation.

* **Confirmed:** The manager initiates the quit request by publishing the appropriate asynchronous request event.
* **Cancelled:** The operation is abandoned, and the player returns to the preceding interface without issuing the quit request.

Other operations requiring confirmation or multi-stage interaction follow the same general pattern.

Responsibilities
^^^^^^^^^^^^^^^^

* **Track the active operation.** Maintain the identity, type, and current stage of the operation in progress.

* **Coordinate multi-stage workflows.** Track progress through menu selection, confirmation, request submission, response processing, error acknowledgement, and completion as applicable.

* **Initiate asynchronous requests.** Publish the appropriate request event when the operation reaches the stage at which the request should be issued. For example, publish ``ReqMakeBet`` when a bet has been fully configured.

* **Correlate requests and responses.** Retain the correlation ID and any necessary operation context so that an incoming response event can be matched to the active request and operation.

* **Process operation results.** Determine whether the response indicates success or failure and advance the operation accordingly.

* **Coordinate error handling.** On failure, request presentation of the appropriate error dialog, retain the operation context while the dialog is active, and process the player's acknowledgement.

* **Manage surface-stack transitions.** Initiate the appropriate dismissal or unwinding of menus and dialogs when the operation succeeds, is cancelled, or finishes error handling. Leave actual surface-stack manipulation to ``SurfaceManager``.

* **Handle confirmation outcomes.** Associate confirmation responses with the operation that requested them and proceed according to the player's decision.

* **Complete or cancel operations.** Release the active operation's state when its lifecycle has finished, ensuring that subsequent operations can begin.

Operating Constraints
^^^^^^^^^^^^^^^^^^^^^

* **One active operation at a time.** The CUI may have at most one operation managed by ``OperationManager`` in progress. A new operation must not overwrite or interfere with an existing operation.

* **Asynchronous request/response processing.** Issuing a request does not imply that the operation has succeeded. The operation remains active until its result has been processed and any required follow-up interaction has completed.

* **Correlation-based response matching.** A response must be associated with the request that produced it. Unmatched, stale, or unrelated responses must not advance or complete the active operation.

* **CUI thread ownership.** All operation-state transitions and resulting CUI actions must respect the CUI's thread-ownership rules. Incoming events from other threads must be routed to the CUI thread before they cause UI or operation-state changes.

* **Separation of responsibilities.** ``OperationManager`` coordinates operations; menus collect choices; dialogs communicate information and collect confirmation or acknowledgement; ``SurfaceManager`` manages the surface stack; and the Craps Engine executes game operations and reports their results.

Completion
^^^^^^^^^^

An operation is complete when its intended outcome has been reached and all required user interaction and CUI cleanup have finished.

Depending on the operation, completion may follow a successful response, cancellation of a confirmation, or acknowledgement of an error.

Once complete, the manager releases the operation's state and becomes available to coordinate the next operation.
