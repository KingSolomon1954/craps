Roll Animation Purpose
^^^^^^^^^^^^^^^^^^^^^^

The Craps Engine resolves a dice roll in a single synchronous burst: it
picks the dice, evaluates every bet, updates balances and statistics, and
publishes the resulting events, all within one call. The CUI, on the
other hand, shows the dice tumbling on screen for a second or two. If the
CUI simply handled each event as it arrived, bets would be cleared and
balances would change while the dice were still in the air, and the
player would see the outcome before the roll is known.

The CUI therefore holds back the results of a roll until the animation
shows the dice landing, then reveals all of them together. The Craps
Engine and the Controller know nothing about this. The synchronization
lives entirely in the CUI.

Participants
^^^^^^^^^^^^

* ``CuiReceiver`` subscribes to the engine's events and converts each
  into a work order placed on the ``CuiThread`` queue, in the order the
  engine published them.
* ``CuiThread`` processes the queue on the CUI thread and hands every
  work order to ``WorkOrderDispatcher``. Its loop also gives the
  dispatcher a regular ``tick()``.
* ``WorkOrderDispatcher`` decides whether an event is handled now or held,
  and replays held events when released.
* ``WindowAnimation`` renders the dice frames from a timer. It alone knows
  when the dice have landed.

Event Ordering
^^^^^^^^^^^^^^

For one roll the engine publishes, in this order:

#. ``UslBettingClosed``
#. ``UslDiceThrowStart``
#. ``UslDiceNewValue``
#. The results: bets resolved, bets removed or changed, balances, session
   and last-roll statistics, point established, seven out, new shooter,
   and ``UslResolveBetsEnd``
#. ``UslBettingOpened``

Because the queue is first-in first-out, ``UslDiceThrowStart`` is always
handled before any of the results.

Holding and Releasing
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: text

    Engine            CuiThread / Dispatcher           WindowAnimation
    ------            ----------------------           ---------------
    ThrowStart  --->  onDiceThrowStart()  ---------->  start frame timer
                      holding = true                   (frame every ~90 ms)
    NewValue    --->  dice to animation (now)
                      roll history, title bar (held)
    results     --->  queued, in order (held)
                                                       ... frames ...
                                                       dice land
                      <-- ReleaseHeldEvents work order
                      replay held events in order
                      holding = false, one full redraw

While holding:

* **Held** are the results of the roll: all bet, balance and statistics
  events, point and seven-out events, shooter changes, roster changes, and
  the roll history and title bar portions of ``UslDiceNewValue``.
* **Not held** are ``UslDiceThrowStart``, the animation's own portion of
  ``UslDiceNewValue`` (it must have the dice to draw them),
  ``UslBettingClosed``, and every ``Rsp`` response event. Responses
  answer requests the player made and must reach ``OperationManager``
  without delay.

Held events are stored as closures and replayed in their original order,
so each panel sees the same sequence of calls it would have seen without
holding. Panels need no knowledge of the mechanism. In particular, panels
that re-read the model after a roll, such as the player area when it
handles ``UslResolveBetsEnd``, do so at release time and therefore never
show post-roll data early.

When ``WindowAnimation`` draws its final frame it enqueues a
``ReleaseHeldEvents`` work order rather than calling the dispatcher
directly. The release is then handled like any other work order, in
sequence, on the CUI thread, so no locking is needed.

Rules
^^^^^

* **The animation always plays out.** A key press does not skip or
  shorten it.
* **Safety net.** If the animation never reports that the dice landed,
  the dispatcher's ``tick()`` releases the held events after ``MaxHold``
  (ten seconds) and logs a warning, so the screen cannot stay frozen.
* **No animation, no hold.** ``WindowAnimation::onDiceThrowStart()``
  returns whether the dice will be animated. If it returns false, nothing
  is held. The dice show immediately and results are handled as they
  arrive.

Animation Speed
^^^^^^^^^^^^^^^

The Animation Speed option on the Control, Configuration menu drives this
mechanism through two small functions in ``WindowAnimation``. Both read
the setting from ``CrapsReaders::readAnimationSpeed()`` at each throw, so a
change applies to the next roll:

* ``animationEnabled()`` is false for Off, so there is no animation and no
  hold. ``onDiceThrowStart()`` returns false and the dispatcher passes the
  roll's results straight through.
* ``frameInterval()`` sets the timer period: 45 ms for Fast, 90 ms for
  Normal and 135 ms for Slow. The length of the animation scales with it.

Slow is about two seconds, well under ``MaxHold``. If an animation can ever
become much slower than that, ``MaxHold`` must be raised to stay above the
longest animation.

The setting lives in the configuration as ``animation.speed``. The session
layer, the highest priority, holds a choice made in the menu, which sends
``ReqSetAnimationSpeed``. Below it are the command line
(``--animation-speed``), config.yaml, and what ``state.yaml`` remembered.
At quit the session choice, or else the remembered value, is written back to
``state.yaml`` under ``settings``. A config.yaml value is never written
there, so removing it from config.yaml restores the remembered speed.
