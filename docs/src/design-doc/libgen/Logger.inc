Logger CRC Card
^^^^^^^^^^^^^^^

One log for the whole program.

**Responsibilities**

* **Write log lines** with a time stamp and a level. The levels are debug,
  trace, info, warn and error.
* **Write to a file,** and to the console until the console is turned off.
  Console output is in color by level.
* **Hold early messages.** A message logged before there is an output file is
  formatted at once, so its time stamp is true, and held. The next output
  file gets them first. At most 1000 are held.
* **Switch the extra levels.** Debug and trace messages are compiled in only
  in a debug build, and are written only when switched on.
* **Be safe from any thread,** with a mutex.

The macros ``LOG_DEBUG``, ``LOG_TRACE``, ``LOG_INFO``, ``LOG_WARN`` and
``LOG_ERROR`` are what the code uses.

**Collaborators**

* ``rang``: the console color
* ``ConfigManager`` (Controller): the ``log.debug`` and ``log.trace``
  settings
* ``GameMain`` (Controller): sets the file, and turns the console off before
  the screen starts
