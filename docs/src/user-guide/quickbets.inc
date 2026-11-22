.. Conventions are the same as in players.inc.

QuickBets
=========

A QuickBets entry is a named bundle of bets that you can place with a few
key presses. For example, an entry called *Place Hi* can hold a Place bet on
the 8, 9 and 10, so that one pick puts all three on the table.

Applying a QuickBets entry
--------------------------

Press ``b`` for the *Betting Menu*, then ``q``. A list shows your entries,
each with its name and the bets it holds. Pick one with the arrow keys and
Enter, or press its number, and the bets are placed. Esc leaves the list
without placing anything.

You can apply an entry whenever betting is open.

What happens to each bet
------------------------

* **One bet per spot.** If you already have a bet on a spot that the entry
  covers, that bet is set to the entry's amount. It is not added to. If the
  amount is already the same, nothing changes.
* **Bets that cannot be placed are skipped.** Perhaps you do not have the
  money, or the table does not allow that amount. The other bets in the entry
  are still placed, and a message tells you which ones were skipped and why.
  When every bet is placed there is no message.
* **Line odds.** An entry may hold odds on a Pass Line or Don't Pass bet.
  The odds are placed after the bet they sit on. Odds can only be placed once
  the point is set, so on the come out roll they are skipped. Odds on Come
  and Don't Come bets cannot be part of an entry.
* **Your Last Fill amounts** are updated, just as if you had placed each
  bet yourself.

Undo
----

Applying an entry is a single step for Undo. Press ``u`` once and every bet
that the entry placed or changed is put back the way it was. See
:ref:`Undoing Bets`.

Setting up entries
------------------

The Configuration menu does not yet have pages to create or change
QuickBets. For now, an entry is written in your player file, in the
``Prefs`` section, while |AppName| is not running:

.. code-block:: yaml

   Prefs:
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

``betName`` is the name of the bet, as it appears in your player file's bet
list. ``pivot`` is the point or number for bets that have one, such as Place,
Buy, Lay and Hardway bets, and may be left out for the others. ``isOdds`` is
only for the odds on ``PassLine`` and ``DontPass``, and may be left out.

Some limits keep the lists readable. You can have up to 20 entries and an
entry can hold up to 12 bets. An entry with no name or no usable bets, or
with more entries than the limit, is ignored when the game starts, and the
reason is written to the log.
