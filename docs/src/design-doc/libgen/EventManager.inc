EventManager CRC Card
^^^^^^^^^^^^^^^^^^^^^

An asynchronous publish and subscribe bus.

**Responsibilities**

* **Take subscriptions.** A handler is subscribed for one event type. Any
  number of handlers may be subscribed for a type.
* **Take events.** ``publish()`` copies the event into a queue and returns at
  once, from any thread. It does not run a handler.
* **Deliver events.** A single worker thread takes the events in the order
  they were published and calls each handler for the type. Handlers run one
  at a time on that thread, so one that blocks holds up all that follow.
* **Finish what was queued.** At shutdown, events already queued are
  delivered before the manager ends. A publish after shutdown has begun
  throws.
* **Hold subscriptions for life.** There is no unsubscribe.

The queue and the subscriber map are protected by one mutex. Handlers are
copied under the lock and called after it is released, so a handler may
subscribe or publish without deadlock.

**Collaborators**

* Every publisher and subscriber, by event type
* ``GameEvents`` (Controller) defines the types the game uses. See
  :ref:`Events`.
