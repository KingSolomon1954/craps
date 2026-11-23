Automation Purpose
^^^^^^^^^^^^^^^^^^

Three features place bets for a player without the player entering each
bet by hand:

* **QuickBets**: a named bundle of bets that the user applies with a key.
* **AutoOdds**: odds that are placed automatically on a contract bet when it
  becomes eligible for them.
* **Automated bets**: bets that a buddy player places on its own, according
  to a plan. Buddies are the main reason for this feature, and it is not
  built yet.

They differ in when they act, not in what they do. All of them turn a list of
bets into bet requests for a player. This section describes the one mechanism
they share, so that the parts built first, QuickBets and AutoOdds, are the
parts that buddy automation will need.

The earlier AutoFill and single QuickBet features are removed. LastFill
covers what AutoFill did, and a QuickBets entry holding one bet covers what a
single QuickBet did.

Automation Vocabulary
^^^^^^^^^^^^^^^^^^^^^

Bet spec
   One bet to place: the bet name, the pivot if the bet has one, whether it
   is an odds bet, and an amount. This is what ``QuickBetEntry`` and
   ``AutoFillEntry`` already were, as two copies of one struct.

Bet plan
   A named, ordered list of bet specs. A QuickBets entry is a bet plan.

Trigger
   The moment something is applied. Manual (the user presses a key), betting
   opened after a roll, come-out, point established, and so on.

Rule
   A trigger joined to an action: apply a bet plan, or apply the AutoOdds
   settings. Only rules for buddies need a general form.

Automation Participants
^^^^^^^^^^^^^^^^^^^^^^^

* ``BetExecutor`` (Controller) applies a list of bet specs for a player and
  reports what happened. It is the only place that turns specs into bets.
* ``QuickBets`` (Controller) holds a player's bet plans. Plain data, one per
  player, held in ``PlayerPrefs``, as the older classes were.
* ``AutoOdds`` (Controller) holds a player's odds multiples and the master
  switch, also in ``PlayerPrefs``, and decides which bets need odds.
* ``EventHandler`` receives the requests from the View, calls the executor,
  and watches for the trigger events for AutoOdds.
* ``UndoManager`` records each application as one undo step.
* The View has the dialogs: a picker to apply a QuickBets entry, the Set or
  Add dialog and an AutoOdds entry in ``DialogBetAmount``, and management
  dialogs on the Configuration menu.

Applying a Plan
^^^^^^^^^^^^^^^

``BetExecutor`` applies the specs of a plan, or of an AutoOdds pass, in order:

* **Best effort.** A bet that cannot be placed is skipped and the rest go
  on. The player is told what was placed and what was skipped and why, in
  one summary, rather than an error dialog per bet. This is chosen over
  refusing the whole plan because a plan made for one table may not fit
  another table's limits and multiples, and a plan that places most of its
  bets is more useful than one that does nothing.
* **Set, not add.** A player has at most one bet per spot. A spec for a spot
  that already has a bet sets that bet to the spec's amount. A spec whose
  amount equals the bet's amount changes nothing and is counted as already
  in place.
* **Order.** In a plan, a contract bet is placed before the odds on it.
  Odds specs are limited to PassLine and DontPass odds, whose point is
  implied. Odds on a Come or DontCome bet exist only after the bet has
  moved to a number, so they are handled by AutoOdds and cannot be saved in
  a plan.
* **Same paths as the View.** Each placement goes through the same
  ``Player::makeOrChangeBet`` and ``setOddsAmount`` that bet requests use,
  so all the table's rules, limits, funds checks and events apply
  unchanged. The player's balance and the screens follow from the usual
  events.
* **One undo step.** The placements of one application are recorded with
  ``undoJoinsPrevious``, so a single undo takes back the whole application.
  This holds for automation too: pressing undo right after a roll takes
  back what AutoOdds did for that roll.
* **Summary.** The result lists, for each spec, placed, changed, already in
  place or skipped with the reason. It goes to the View as an event. The
  View puts a short line in the Message area when there is one, and the
  handler always writes it to the log.

Automation Request Flow
^^^^^^^^^^^^^^^^^^^^^^^

For QuickBets, as built:

#. ``MenuBetting`` ``q`` calls ``OperationManager::applyQuickBets()``, which
   reads the entries with ``CrapsReaders::readQuickBets()`` and shows
   them in ``DialogPickPlayer``. The pick list carries each entry's name, so
   the request still names the right entry if the list changes.
#. The pick sends ``ReqApplyQuickBets`` (player id and entry name).
#. ``EventHandler::onReqApplyQuickBets()`` refuses when betting is closed or
   the entry is not found. Otherwise it calls ``BetExecutor::apply()`` with the
   player, the entry's specs, ``UndoManager`` and the player's ``LastFill``,
   and always answers with ``RspApplyQuickBets``.
#. The response carries the counts, a headline such as "3 placed, 1 skipped"
   and one line for each skipped bet. ``accepted`` means the entry was
   applied, even if some bets were skipped. A refused request has an error
   instead.
#. ``OperationManager::onRspApplyQuickBets()`` unwinds the menus, and shows a
   notice only when something was skipped.

The bet and balance events that the placements cause redraw the screens in
the usual way. The View does not need to know which bets were placed.

Saving a bet into QuickBets, as built:

#. ``DialogBetAmount`` ``q`` puts the typed amount into ``CarrierBet`` and
   calls ``OperationManager::quickBetsSetOrAdd()``. That makes a ``BetSpec``
   from the carrier. Line odds get pivot 0, because they sit on the line bet.
   A bet that cannot be saved, such as Come odds, is refused with a notice.
#. ``DialogQuickBetsSetAdd`` offers a new entry (``1``) or the existing
   entries (``a``, ``b``, ...). It reports with
   ``OperationManager::onQuickBetsChoice()``. A new entry asks for a name in
   ``DialogTextInput``, whose validator uses ``QuickBets::checkName()`` and
   refuses duplicates ignoring case.
#. ``ReqEditQuickBets`` carries the action (create or add), the name and the
   spec. ``EventHandler::onReqEditQuickBets()`` calls ``QuickBets::create()``
   or ``QuickBets::addBet()``, which return "" or the reason, and always
   answers with ``RspEditQuickBets``.
#. On success the View shows the response's message in a notice. The player is
   back at ``DialogBetAmount``, since the operation has no menus to unwind.

Managing entries, as built. ``MenuConfigure`` ``q`` calls
``OperationManager::manageQuickBets()``. It is a chain of steps, each of which
pops itself and reports back, so ``manageStep_`` says which one answered:

#. Pick the entry in ``DialogPickPlayer``.
#. Pick the action: rename, change an amount, remove a bet or delete.
#. Depending on the action: a name in ``DialogTextInput``, a bet in
   ``DialogPickPlayer`` and then an amount in ``DialogTextInput``, a bet to
   remove in ``DialogPickPlayer``, or a confirmation in ``DialogConfirm``.
#. One ``ReqEditQuickBets`` with the action's fields (``newName``, ``index``
   and ``amount``). ``QuickBets::rename()``, ``setAmount()``, ``removeBet()``
   and ``removeEntry()`` each return "" or the reason, and the response
   carries the message.

The entry is named in the request, and the bet by its position in the entry.
Nothing else changes entries between the pick and the request, so the
position is still right. If it is not, the controller refuses with "That bet is
no longer in the entry". An entry keeps at least one bet. Creating an entry
and adding bets stay in the bet amount dialog, where the bet is already
chosen.

The entries live in ``PlayerPrefs`` and are written to the player file when
the players are saved, as for Last Fill. No lock guards them. They change
only in answer to the View's own request, and the View reads them only before
or after that request, so the two do not overlap until buddy automation
reads them from another thread.

QuickBets Rules
^^^^^^^^^^^^^^^

* A QuickBets entry has a name chosen by the user and a list of bet specs.
  The View shows the name and a generated description of the bets.
* At most 20 entries and 12 specs in an entry, so that the lists fit the
  screen and each entry can have a letter key.
* The Betting menu key ``q`` shows the entries to apply. The Configuration
  menu lists, creates, renames, adds to, trims, changes the amounts of and
  deletes entries.
* ``DialogBetAmount`` has ``q`` for "QuickBets, set or add". The dialog
  then offers "Set as new QuickBets entry", which asks for a name, or "Add
  to an existing entry". Adding a spec whose spot is already in the entry
  replaces the amount, which matches one bet per spot.
* Amounts are fixed dollar amounts, validated when applied. They are not
  converted when the table changes.
* Order of the entries matters and is kept in the player file.

AutoOdds Rules
^^^^^^^^^^^^^^

AutoOdds places odds on a contract bet for the user, in a size that depends
on the kind of bet and the point it is on.

* **Settings.** A table with an entry for each of PassLine, DontPass,
  Come and DontCome and each of the points 4, 5, 6, 8, 9 and 10. An entry is
  either a multiple *n*, so the odds amount is *n* times the contract amount,
  or an exact dollar amount. A missing entry means no
  automatic odds for that bet type and point. This is needed because odds
  size depends on the point as well as on the bet. A player may want much
  more behind a DontPass bet on the 10 than on the 6. Typical examples are
  3, 4 and 5 times for the 4 and 10, the 5 and 9 and the 6 and 8. The
  Configuration menu offers that as a preset and can also set one multiple
  for every point of a bet type.
* **Master switch.** There is one on and off switch for each player, so
  AutoOdds can be paused without losing the table of multiples. When it is on,
  the View shows a visible indicator, and each automatic placement is
  reported in the Message area, since the feature spends the player's money
  unprompted.
* **When.** AutoOdds runs on ``UslBettingOpened``, after the roll's results
  are settled. It looks at the player's bets rather than following events,
  so it does not matter whether the bet became eligible because the point
  was set, a Come bet traveled to a number, or the table was switched.
* **Which bets.** A contract bet that has a point or pivot, has no odds, and
  has not been handled by AutoOdds before. The controller remembers the ids
  of the bets it has handled. This is needed so that odds the player takes
  down by hand stay down. Without it AutoOdds would put them back after the
  next roll. The set is pruned of ids that are no longer on the table.
* **Amount.** The exact amount, or *n* times the contract, rounded down to the table's required
  odds multiple and not above the table's maximum odds for that bet. The
  table already knows both, from its odds limit rules. If the result is
  below one odds increment, the bet is skipped and the reason is reported.
* **Failures.** Insufficient funds or a refusal from the table skips the
  bet. AutoOdds does not retry within the same pass. It has already marked
  the bet as handled.
* **From DialogBetAmount.** ``a`` sets the entry for the bet type and point
  of the odds bet being entered to the exact amount entered, and reports it.
  A zero amount removes the entry. The
  line is shown only when the dialog is for an odds bet and is blank
  otherwise, so that the dialog does not change shape.

As built:

* ``AutoOdds`` holds the multiples, the on and off switch and the handled
  ids. ``AutoOdds::plan()`` is the pass. It takes candidate bets, each with
  the table's odds limits already looked up, so it is tested without a
  table. ``EventHandler::onUslBettingOpened`` makes the candidates from the
  user player's bets, applies the plan with ``BetExecutor::apply`` and
  publishes ``UslAutoOddsApplied``.
* A bet is marked handled as soon as the pass has a multiple for it, whether
  or not the odds could be placed. A bet that already has odds is also
  marked, so odds the player later takes down stay down.
* AutoOdds is applied for the user player only. Odds are not LastFill
  entries, since the player did not choose them.
* ``ReqEditAutoOdds`` and ``RspEditAutoOdds`` carry every change: the
  switch, one entry or all points of a bet type (a point of 0), the 3-4-5
  preset and clearing. The Set or Unset of ``DialogBetAmount`` is a ``Set``
  with the entered amount.
* Until the Message area exists, the middle of the bottom bar shows
  *AutoOdds on* while it is on, and *AutoOdds: n placed* after a pass until
  the next roll. Odds that could not be placed are listed in a notice, unless
  the player is in the middle of another dialog; the log has them all.

Buddy Automation
^^^^^^^^^^^^^^^^

Not built yet. This is what the other parts are shaped for.

* A buddy is an ordinary player with its own ``PlayerPrefs``. Its plans,
  AutoOdds and rules are kept in that file, as the user's are.
* A rule is a trigger and an action. The first triggers are betting opened
  with no point (come-out), betting opened with a point, and betting opened
  after any roll. The actions are to apply a named plan, or to apply AutoOdds.
  Conditions such as the player's balance and progressions that change
  amounts after wins and losses come later and fit between the trigger and
  the action.
* ``EventHandler`` runs the rules for every player that has them at the
  same ``UslBettingOpened``, through the same executor.
* A buddy's undo stack is never used. It is cleared at each roll as every
  stack is, so it cannot grow.

Automation Persistence
^^^^^^^^^^^^^^^^^^^^^^

In the player file under ``Prefs``. ``QuickBets`` replaces the old
one-bet form of the same name, and ``AutoFills`` is gone. ``pivot`` and
``isOdds`` may be left out, and default to 0 and false:

.. code-block:: yaml

   QuickBets:
     - name: Place Hi
       bets:
         - { betName: Place, pivot: 8,  amount: 24 }
         - { betName: Place, pivot: 9,  amount: 20 }
         - { betName: Place, pivot: 10, amount: 20 }
     - name: Line with odds
       bets:
         - { betName: PassLine, amount: 10 }
         - { betName: PassLine, isOdds: true, amount: 30 }
   AutoOdds:
     enabled: true
     multiples:
       PassLine:
         4: 3x
         5: 4x
         6: $50

Loading follows the Prefs rules already in force: an entry that cannot be
read is logged and skipped, and loading never throws. A loaded plan is
made valid by ``QuickBets::tidy()``: bets with no usable bet name or a zero
amount are dropped, odds on bets other than PassLine and DontPass are
dropped, a later bet for the same spot replaces the earlier, contract bets
are moved before odds, and a plan is cut to 12 bets. A plan with no name or
no bets left is skipped. Old-form entries, which have no ``name``, are
skipped the same way.

Automation Staging
^^^^^^^^^^^^^^^^^^

#. Remove AutoFill and the single QuickBet, with their tests, events,
   dialog entries, YAML and documentation.
#. ``BetSpec``, bet plans and ``BetExecutor``, with the single undo step,
   the summary event, ``QuickBets`` data and persistence, and the Betting
   menu key to apply an entry. Done.
#. The Set or Add dialog in ``DialogBetAmount`` and the Configuration menu
   pages for QuickBets. Done. Entries are made in the bet amount dialog and
   changed in the Configuration menu.
#. AutoOdds: settings and persistence, the pass on ``UslBettingOpened``,
   the ``DialogBetAmount`` entry, the Configuration menu pages, the master
   switch and its indicator. Done. Only the user player is automated.
#. Buddy automation rules and their triggers.

Automation Open Items
^^^^^^^^^^^^^^^^^^^^^

* Whether AutoOdds should also offer a "maximum the table allows" setting
  in place of a multiple.
* A place for the Message area to show summaries before the Message area
  exists. Until then the summary of an application that skipped bets is
  shown in a notice dialog, and a clean application shows nothing. The
  executor logs every application. AutoOdds shows a short status in the
  middle of the bottom bar.
* Whether QuickBets entries should be shared between a user and the buddies,
  or only per player.
