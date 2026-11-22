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

Saving a bet as a QuickBets entry
---------------------------------

The easiest way to build an entry is while you are entering a bet. In the
dialog where you type the amount, press ``q`` for *QuickBets - Set or Add*.
The dialog shows the bet and the amount you have typed, for example
*Place bet 6 $24*, and offers these choices:

* ``1`` *Set as new QuickBets entry* asks for a name and makes a new entry
  that holds this one bet.
* ``a``, ``b``, ``c`` and so on, one for each entry you already have, add the
  bet to that entry. If the entry already has a bet on that spot, its amount is
  changed to the amount you typed.

Esc leaves without saving. After a choice, a message says what was saved and
you are back at the amount dialog, where you can still place the bet.

The bet is saved as it is: you do not have to place it first. Press ``q``
again with a different bet to keep building the entry.

Some rules:

* An entry's name can be up to 24 characters and must not match another
  entry's name, whatever the capital letters.
* You can have up to 20 entries, and an entry can hold up to 12 bets. If more
  entries than fit on the screen exist, the Up and Down keys show the rest.
* Entries can hold odds only for Pass Line and Don't Pass bets. For other
  odds bets the message says so.

Changing your QuickBets entries
-------------------------------

Press ``c`` for the *Control Menu*, then ``c`` for *Configure*, then ``q``
for *QuickBets*. A list shows your entries. Pick one, then pick what to do
with it:

* **Rename** asks for a new name, starting with the old one.
* **Change an amount** lists the bets in the entry. Pick one and type its new
  amount.
* **Remove a bet** lists the bets in the entry. Pick one to take it out. An
  entry always keeps at least one bet, so to get rid of the last one,
  delete the entry.
* **Delete** asks you to confirm and then deletes the whole entry.

A message tells you what changed, and you are back at the Configure menu. Esc
leaves any list or question without changing anything.

New entries, and more bets for an entry, are made from the bet amount
dialog as described above, since that is where the bet is already chosen.

Changes are saved with your player when you leave the game.

Writing entries in the player file
----------------------------------

Entries can also be written in your player file, in the ``Prefs`` section,
while |AppName| is not running:

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
