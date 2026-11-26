ErrorPass CRC Card
^^^^^^^^^^^^^^^^^^

An error, carried as a value.

**Responsibilities**

* **Describe a failure:** an error name, an error type, a severity and a
  description, with an optional probable cause, proposed repair and
  additional information.
* **Say where it happened:** the function, file and line where it was set,
  and a time stamp.
* **Grow as it travels.** Each caller may prepend or append words to the
  description, or add its own place to a stack trace, so that the error
  tells the whole story when it is finally shown.
* **Be extended by the application.** Error names and types are strings. A
  standard set covers common cases, and an application may use its own words
  without deriving a class.
* **Say whether it is set.** An empty error name means no error.

An ``ErrorPass`` is used in two ways. In the *value-based* way, a function
returns ``zeus::expected<T, ErrorPass>``, which holds either the value or the
error. In the *call-chain* way, the caller declares an ``ErrorPass`` and hands
it down, and the callee fills it in only if it fails. The macro ``EP_SET``
sets an error with the place filled in, and ``EP_TRACE`` adds the current
place to the trace.

**Collaborators**

* ``zeus::expected``: the carrier in the value-based way
* ``Logger``: errors are logged with ``fullDesc()``
* ``ReturnCode``: the result of call-chain functions
* Every library: the Model refuses a bet with one, and the Controller puts
  one in a response event for the View to show
