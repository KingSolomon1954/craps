TimerManager CRC Card
^^^^^^^^^^^^^^^^^^^^^

Many timers on one thread.

**Responsibilities**

* **Make timers,** armed at once or armed later. A timer has an id.
* **Fire timers.** A timer fires once, or repeats and re-arms itself.
* **Cancel and restart** a timer by its id, at any time.
* **Run every callback on one dedicated thread.** All calls are thread safe.

A callback runs on the timer thread, so it has to be short and has to leave
the data of other threads alone. The game's callbacks only queue a request
for the thread that owns the data.

**Collaborators**

* ``WindowAnimation`` (View): creates the dice animation timer, and its
  callback queues the drawing for the CUI thread
