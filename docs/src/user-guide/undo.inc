.. Conventions are the same as in players.inc.

Undoing Bets
============

Changed your mind? Press ``u`` on the Betting menu to take back the last
change you made to your bets. Press ``u`` again to take back the one before
that, and so on, until you run out of changes to take back.

Undo is on the *Betting Menu*. Press ``b`` for the Betting menu, then ``u``.
From the table screen you can also press ``u`` by itself.

What can be undone
------------------

Each of these is one step that ``u`` takes back:

* Placing a bet. Undo takes the bet down and returns its money.
* Changing a bet's amount. Undo puts back the amount the bet had before.
* Setting or changing odds. Undo puts back the previous odds, or takes the
  odds down if there were none.
* Taking a bet down. Undo puts the bet back on the table, with its odds, and
  takes its money again.
* Taking down all your removable bets at once, with the *all* choice of Remove
  Bets (``x``). All of them are put back together by a single undo.

Money always follows the bets. Undoing a bet you placed returns your money,
and undoing a bet you took down takes it again. If you have since spent the
money, so that it can no longer be put back, the undo is refused with a
message and nothing is changed.

Undo puts a bet back the way it was, even where the rules would not let you
make that change by hand. For example, you can take down a ``DontPass`` bet
after the point is set, and undo will put it back.

What cannot be undone
---------------------

* **Anything before the last roll.** Rolling the dice decides your bets, so
  the list of things you can undo starts empty after every roll. Undo is
  only for the betting you have done since the dice last landed.
* **While the dice are rolling.** Betting is closed then. Wait until betting
  reopens and the steps are still there.
* **Other players' bets.** Undo only takes back the bets of the player you
  are playing as. A buddy's bets are never touched. Each player has their
  own list, so switching players and back does not lose your steps, as long
  as you do not roll in between.
* **Changing tables.** Your list starts empty at a new table.
* **Redo.** There is no way to undo an undo. Place the bet again.
* **Things that are not bets.** Settings, such as Animation Speed, are not
  affected.

.. note::

   If there is nothing to undo, you get a message that says so. If an undo
   is refused for another reason, you get a message with the reason, and
   the list of steps that you could undo is cleared, because your bets may
   no longer match it. Your bets and your money are left as they were.

Undo does not change your Last Fill amounts.
That still remembers the last amount you entered, even if you undid that bet.
