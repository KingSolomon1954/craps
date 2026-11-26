Error Handling
^^^^^^^^^^^^^^

Errors are reported in three ways, depending on where they happen.

**Error passing.** Most of the engine and the Controller report an
expected failure, such as a bet that the table does not allow, with
``Gen::ErrorPass``. An ``ErrorPass`` holds an error name and type, a
severity, a description, and where the error arose. There are two forms:

* **Value-based.** A function returns ``zeus::expected<T, ErrorPass>``,
  which holds the value or the error. The engine uses this, for example in
  ``Player::makeOrChangeBet()`` and ``Player::joinTable()``.
* **Call-chain.** The caller passes in an ``ErrorPass`` that the function
  fills in on failure, and the function returns a ``ReturnCode``. The
  Controller's managers use this.

A function that passes an error up the call chain adds its own context with
``EP_TRACE``, so the error carries the path it took. The names and types are
strings, so the application can use its own as well as the standard ones.

**Exceptions.** Code that cannot continue throws, and in particular code
that reads files. ``YamlFile`` throws on a file that cannot be read or
written. The managers catch these, log them, and turn them into an
``ErrorPass`` or carry on, as the case calls for. A handler running on the
event thread must not let one escape. The bus drops it. See :ref:`The Event Bus`.
Exceptions during startup are not caught until ``main()``, which prints the
message and ends the program.

**Logging.** A failure the user does not need to hear about is only logged.

How an Error Reaches the User
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An error from a request goes back to the View in the response event, which
carries the ``ErrorPass``. ``OperationManager`` ends the operation and
shows the error in a dialog that says what failed and why. The operation
is finished when the user dismisses the dialog. A success may show a
notice. The View never sees an exception from the
Controller, because requests cross the event bus.

.. figure:: /images/pub/ErrorSeq.png
   :alt: Error Sequence Diagram

   **Figure:** Error Sequence Diagram

Logging
^^^^^^^

``Gen::Logger`` is a singleton, usable from any thread. There are five
levels: debug, trace, info, warn and error. Each log line has a timestamp and a level.

* Messages go to the log file in the user state directory, and until the
  screen takes over, also to the console. ``GameMain`` turns console output
  off before the View starts, since the console belongs to ncurses.
* Before the file is known, messages are held and written to it first, when
  it is set. See :ref:`Logging During Startup`.
* Debug and trace messages are compiled in only in a debug build. A debug
  build writes them only when the ``log.debug`` and ``log.trace`` settings
  are on. Informational, warning and error messages are always kept.
* The macros ``LOG_TRACE``, ``LOG_DEBUG``, ``LOG_INFO``, ``LOG_WARN`` and
  ``LOG_ERROR`` are used in the code. Each entry into a handler logs a
  trace message with the name of the function.

Timers
^^^^^^

``Gen::TimerManager`` runs any number of one-shot or repeating timers on a
single thread. A timer is created with a callback, armed, and cancelled or
restarted by its id.

A timer callback runs on the timer thread, so it must be short, and it must
not touch anything that belongs to another thread. The dice animation shows
the rule. Its callback only queues a draw request for the CUI thread, and
the drawing is done there. See :ref:`Thread Inventory`.
