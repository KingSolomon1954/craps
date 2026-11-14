.. Conventions used in this section
..
..   **Bold**         first use of a defined statistics term
..   *Italic*         emphasis, and words being discussed as words
..   ``Literal``      bet names, YAML keys, and on-screen labels
..   Notes            "Note" admonitions flag nuances that commonly surprise players

Understanding |AppName| Statistics
==================================

|AppName| records a large number of statistics while you play. This section
describes what is recorded, how each statistic is counted, and the nuances
that matter when you interpret the numbers. It assumes you already know
the rules of craps and the standard bets.

Statistics are only as useful as your understanding of what they count.
Several of the statistics in |AppName| deliberately differ from what a
casual reader would assume, for example a Hardway bet's loss is split into
*easy way* and *seven*, and a Place bet is not counted on the come out roll.
Those choices are called out below.

.. note::

   Craps dice are independent. Every statistic here describes the past and
   none of them predicts the next roll. Streaks, repeaters and
   "hot" or "cold" tables are interesting to study, but a number is no more
   likely to appear after a long absence than after a recent hit.

Where statistics are kept
-------------------------

Every statistic belongs to one of two owners and covers one of two time
spans.

**Table statistics** describe the dice and the table: every roll made at
the table, regardless of who bet. They are independent of which players are
seated.

**Player statistics** describe one player: the bets that player made, the
results of those bets, and that player's bank activity. Each player has
their own set, so you can compare how two players fared on the same rolls.

**Current session** statistics cover the time since you started the program.
**All-time** statistics accumulate across every session. When a session
ends, the current session statistics are merged into the all-time statistics
and saved to the table and player YAML files. Because all-time values are
kept in those files, they survive restarts. Current session values start
over at zero each time you launch the game.

.. note::

   The all-time maximum values (largest single win, longest streak and so
   on) are the maximum over all sessions, not the sum of the session maximums.

Roll counters and streaks
-------------------------

Most dice statistics are kept as a **counter**. A counter records two
things:

* the **count**, which is how many times the event occurred, and
* the **max repeats**, which is the longest run of consecutive occurrences
  of that same event.

A run is consecutive *rolls*, not consecutive visits. If the event is
"a 6 is rolled", then a run of three means the dice came up 6 on three
rolls in a row. The first occurrence starts a run and does not count as a
repeat, so a number rolled twice in a row has one repeat.

Dice number counts
~~~~~~~~~~~~~~~~~~

For each total from 2 to 12 the game counts how many times it was rolled
and its longest back-to-back run. These are the basic statistics from which
most players judge whether the dice are behaving as expected. Over a large
number of rolls the counts approach the standard two-dice frequencies
(a 7 on 1 roll in 6, a 6 or 8 on 5 rolls in 36 each, and so on).

Come out and point rolls
~~~~~~~~~~~~~~~~~~~~~~~~

Every roll is either a **come out roll** (no point is established) or a
**point roll** (a point is on). The game counts both, along with how many of
each occurred back to back.

The game also counts, for each total from 2 to 12, how often that total was
rolled *while a point was on*. This matters for bets that are not working on
the come out roll, such as Place, Buy and Lay bets. Those bets can only be
decided by point rolls, so these counts are the right basis for judging them.

Points and seven-outs
~~~~~~~~~~~~~~~~~~~~~

The number of points established is counted overall and per point number
(4, 5, 6, 8, 9 and 10). A **seven-out** is a 7 rolled while a point is on.
Each seven-out ends a shooter's turn, so the game keeps a **shooter roll
count**: the number of rolls in the current shooter's turn, which restarts
at zero on a seven-out. A come out 7 is a natural and does not end the turn.

.. note::

   A shooter's roll count includes the come out roll that established the
   point and any come out rolls after a point is made. It is the length of
   the whole turn, not only the time with a point on.

Come out results
~~~~~~~~~~~~~~~~

On a come out roll the game counts 2, 3, 7, 11 and 12 separately, and also
counts **craps** as a group (2, 3 and 12). Each of these is a counter, so
you can see, for example, the longest string of come out sevens.

The game also counts the come out decisions of the four line bets:

* Pass Line wins on a come out 7 or 11 and loses on 2, 3 or 12.
* Don't Pass wins on 2 or 3 and loses on 7 or 11. A come out 12 is a push
  and is counted as neither a win nor a loss.
* Come and Don't Come are tracked the same way, using the roll made while
  a point is on as *their* come out roll.

Pass and Come points
~~~~~~~~~~~~~~~~~~~~

For each point number the game counts how many times a Pass Line, Come,
Don't Pass and Don't Come bet on that point was won and how many times it
was lost, with repeat streaks. This is what lets you see, for instance, how
often a 4 is made compared with how often a 7 beats it. The ratio of wins to
losses on a given point is expected to approach the true odds (2 to 1 for 4
and 10, 3 to 2 for 5 and 9, 6 to 5 for 6 and 8).

Field results
~~~~~~~~~~~~~

The Field is counted as a win on 2, 3, 4, 9, 10, 11 or 12 and a loss on 5,
6, 7 or 8, each with streaks.

.. note::

   This is a count of rolls where a Field bet *would have* won or lost
   whether or not anyone bet. It does not account for the 2 and 12 payout
   (2 to 1 or 3 to 1 depending on table rules), which only affects the money
   statistics.

Hardway results
~~~~~~~~~~~~~~~

For 4, 6, 8 and 10 the game counts hard way wins (a pair rolled) and losses.
Losses are split by cause:

* **easy way**: the number was rolled any way other than as a pair, and
* **seven**: a 7 was rolled.

This split is worth watching. A Hardway player who is losing mostly to
sevens has a different problem from one losing mostly to easy ways, and the
house edge you are really facing depends on which of the two dominates.

.. note::

   Hardway statistics assume hardways work on the come out roll. When you
   are looking at the table statistics, Hardway counts therefore include
   come out rolls.

Repeaters
---------

A **repeater** statistic answers the question serious players ask most
often: *how many times does a number show up before a 7 ends the run?*

|AppName| records this as a **histogram**. For a given number, the game
stores how many times the number was hit before a 7, and how often each of
those hit counts happened. For example, for the number 6 you might see:

.. code-block:: text

   Hits before 7    Times
   0                 61
   1                 52
   2                 40
   3                 29
   ...

.. note::

   The game stores *hits*, not *repeats*. A number that appeared once before
   the 7 has one hit and zero repeats. Players often say "it repeated twice"
   to mean three hits. Make sure you convert when comparing against
   other sources.

Table repeaters
~~~~~~~~~~~~~~~

The table keeps two kinds, and the difference matters.

**Point repeaters** cover the numbers 2, 3, 4, 5, 6, 8, 9, 10, 11 and 12.
Any 7 ends a run, including a come out 7. All rolls count, including the
roll that establishes the point. This is the raw "hits before a 7" for each
number, and is the statistic that matches how most players describe a
number's behaviour.

**Place repeaters** cover only 4, 5, 6, 8, 9 and 10. Only rolls made while a
point is on count as hits, and only a seven-out ends a run. A come out 7
does not end a Place run because a Place bet is not working on the come out
roll. A run of zero hits is recorded when a seven-out ends a run with
nothing in it, so the histogram's zero entry reflects how often a Place
bet on that number would have lost without ever being paid.

.. note::

   A number rolled on the come out roll to establish the point is a hit for
   point repeaters, and is *not* a hit for place repeaters. That single
   difference accounts for most of the gap between the two histograms for
   the same number.

Player repeaters
~~~~~~~~~~~~~~~~

Each player has their own repeaters, for the numbers 4, 5, 6, 8, 9 and 10
only. These are **participation based**. They measure the runs that the
player's own bets actually took part in, not every roll at the table.

* **Point repeaters** count hits of a number while the player held a Pass
  Line or Come bet on that number. The roll that establishes the point
  counts as a hit, a number is counted at most once per roll, and any 7 ends
  the run.
* **Place repeaters** count hits on the player's winning Place bets.
  A seven-out ends the run.
* **Buy repeaters** do the same for winning Buy bets.

Winning Place, Buy, Pass Line and Come bets are taken down in |AppName|
once paid, so a player who re-places a bet after a win continues the same
run. A run therefore spans the re-placed bets rather than restarting at
each bet.

The histogram's zero entry has a precise meaning for players. A zero is
recorded when the player held a Place or Buy bet on a number, a seven-out
came, and the number was never hit. It is *not* recorded for numbers the
player was not betting. If the number was hit but the bet was not up at the
time, the run closes with the hits it did get.

.. note::

   A come out 7 closes the runs only of Place and Buy bets that actually
   lost on that 7, that is, bets the player had working. It does not close
   runs on bets that are off.

.. note::

   Running tallies of unfinished runs are not saved. If you quit in the
   middle of a run, that partial run is discarded rather than
   being added to the histogram.

Bet statistics
--------------

Each player has bet statistics. These record every bet that reached a
decision: win, lose, or neither (a bet that was taken down or that stayed up
and was **kept**).

For every bet type the game records the **count** of wins and losses, the
**amount** won or lost, and the **amount bet**. You can compare those to see
profit or loss, the average bet, and how a bet type performed overall.

Overall totals are also kept: bets made, won, lost and kept; total
dollars at risk, won, lost and kept; and the largest single bet, win, loss
and keep. For the number of bets and the amount bet on one roll, the game
tracks the current value, the maximum, and the total, which lets you see how
heavily a player bets on a typical roll as well as on their heaviest roll.

Each bet type also records the **distance**, which is how many rolls its bets
stayed up before they were decided. The average distance is the total
distance divided by the count.

Contract amount and odds amount
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pass Line, Come, Don't Pass and Don't Come bets can have odds. The amount
won, lost and bet is stored as a *combined* figure, and the odds part is
also stored separately. So you can see the profit on the line bet itself
and the profit on the odds.

.. note::

   Odds are not a separate bet type in the statistics. They appear inside
   their parent bet. The "amount bet" for a Pass Line bet includes any odds
   behind it, which is why the Pass Line's dollars bet look larger than the
   contract bets alone.

   When judging odds, look at the odds portion. It is the only part of the
   wager with a zero house edge, and it is the only part that tells you
   whether odds were worth taking.

Statistics by point and by roll
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Bets are recorded by name and, where it matters, by the number that
decided them:

* **Come out versus point**: line bet decisions made on the come out roll
  are stored separately from decisions made after a point is established, so
  you can see how much of a Pass Line player's profit comes from the come
  out and how much from the points.
* **By point**: Pass Line, Come, Don't Pass, Don't Come, Place, Buy, Lay and
  Hardway statistics are stored per number (4, 5, 6, 8, 9, 10), so you can see
  which numbers made or lost money.
* **Do and Don't side**: the game can combine all "do" bets
  (Pass Line and Come) and all "don't" bets (Don't Pass and Don't Come)
  into a single figure each, to show how the right-way and wrong-way sides
  compare.
* **Field by number**: Field results are stored per rolled number, so you can
  see how much of the profit came from 2 and 12 versus the other
  winners.

One roll bets
~~~~~~~~~~~~~

Any Craps, Any Seven, Any 7/11, C&E, Horn and World are decided on the next
roll. In addition to their totals, these are stored by the number that was
rolled. For example, you can see how many times a Horn bet won on a 12 versus
a 3.

Horn, C&E and World are combination bets. Part of the wager wins and the rest
loses, so a single result may be a net win even though some of the components
lost. The game classifies each decision:

* **Entire loss**: the whole wager lost, for example a World bet on a point
  number.
* **Partial win**: a net win with losing components, for example a Horn bet
  that hits a 2 or a C&E bet on 11.
* **Break even**: a component won but net profit is zero, for example a
  World bet on a 7.
* **Clean win**: a win with no losing parts, as with Field, Any Seven and
  Any Craps.

.. note::

   For combination bets, the amount won or lost is stored **net of the
   losing components**. A Horn bet of $4 total that wins on a 2 does not
   count as a win of $30 plus three losses of $1. It counts as the net
   figure. If you want to know how often a Horn hit, use the by-roll
   statistics.

Hardway bets
~~~~~~~~~~~~

Hardway losses are stored split by cause, as in the table statistics above:
the easy way versus a seven. This means you can see for each hardway number
whether a player is being beaten by sevens or by the easy way.

Simulated ("would have") statistics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Besides what a player actually bet, the game can show how often each bet
*would have* won or lost given the rolls seen at the table. These are the
**simulated counts**. They answer questions like "how often would a
Place 6 have won if I had kept it up the whole time?"

They are derived from the table roll statistics rather than from any
player's bets, so they apply even for bets the player never made. They make
the following assumptions, which you should keep in mind:

* Place, Buy and Lay bets are counted from point rolls only, since those
  bets are off on the come out roll.
* Hardway bets are counted on every roll, as if they work on the come out.
* One roll bets are counted on every roll.

.. note::

   Simulated counts describe the dice and are not a measurement of your own
   performance. If your actual win rate on a bet type is well away from the
   simulated rate, the difference comes from when and how you bet, not from
   luck at the table.

Roll activity
~~~~~~~~~~~~~

The game also tracks activity per roll: how many bets were up, won and lost
on a single roll, and the busiest roll seen so far. The **last roll statistics**
describe only the most recent roll: the dollars and bets on the table
before the roll, and the dollars and bets won and lost on it.

Bank statistics
---------------

Each player keeps a record of bank activity:

* the number and total amount of **deposits**,
* the number and total amount of **withdrawals**,
* the number and total amount of **refills**, and
* the largest total deposited and the largest total withdrawn in a
  single session, each with the date it happened.

.. note::

   A refill is money the bank adds automatically when a player's balance
   falls below the configured refill threshold. It is tracked apart from
   deposits so that your deposits show money you chose to put in, and your
   refills show how often the bank ran low.

Session history
---------------

The game keeps a summary for each past session: its date and duration, how
many players took part, how many bets were made, and the total intake and
payout. From these it reports the number of sessions, the date of the first
session, the longest session, and the results of each session in order, oldest
first.

**Intake** is the money bet and **payout** is the money returned to players,
so ``intake - payout`` is the amount the table kept. A table that runs
long enough shows the house edge in these figures.

Reading the numbers
-------------------

A few cautions that apply to every statistic above.

**Sample size.** Craps results swing widely over short samples. A few hundred
rolls is too few to say anything about a bet's performance. Expect numbers
to approach their theoretical values only after thousands of rolls, and
dollar results are noisier still because of varying bet sizes.

**Table versus player statistics.** Table statistics describe the dice.
Player statistics describe the dice *and* the player's timing, bet sizing
and choices. Always compare a player's numbers against the table's before
drawing a conclusion about the player.

**House edge.** The house edge of a bet is the average loss per dollar
resolved. The game's bet dialogs list each bet's edge, so compare your own
results to them. Single-roll bets resolve every roll and quickly show their
edge. Line bets resolve slowly and take far more bets to settle.

**What is not counted.** Pushes (such as Don't Pass on a come out 12) are
not wins and losses. Bets that are kept or taken down do not contribute to
win or loss counts.

.. note::

   Screenshots of the statistics screens will be added to this section
   as the full-screen statistics windows are completed.
