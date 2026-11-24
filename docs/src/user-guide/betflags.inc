.. Conventions are the same as in players.inc.

Bet Flags: Turning Bets Off and On
==================================

A bet that is *working* can win or lose on the next roll. A bet that is
*not working* is left alone on that roll: it stays on the table, and it
neither wins nor loses. Bet Flags lets you say which of your bets work.

At a real table the dealer decides by the house rule: Place, Buy and Lay
bets, and odds, are off on the come out roll, and Hardways are always on.
Bet Flags lets you change that for a roll, or until you change it back.

Which bets can be flagged
-------------------------

* Place, Buy, Lay and Hardway bets.
* The **odds** on a Pass Line, Don't Pass, Come or Don't Come bet. The
  flag is for the odds only, never for the bet itself. If the odds are not
  working on the roll that decides the bet, the bet still wins or loses and
  the odds are returned to you.

Pass Line, Don't Pass, Come and Don't Come bets with no odds, the Field and
the one roll bets cannot be flagged.

The three states
----------------

* **Table Default** follows the house rule above. Every bet starts this way.
* **Working** means the bet is on for every roll, the come out too.
* **Not-Working** means the bet is off for every roll.

A **Next Roll** choice changes only the next roll, then goes away on its own.
It leaves the state you set alone. For example, a Place bet that is Working
and set to Not-Working Next Roll sits out one roll and is Working again after.

Setting flags
-------------

Press ``b`` for the *Betting Menu*, then ``g`` for *Bet Flags*. The menu
offers only the kinds of bets you have:

* ``i`` *Individual Bet*. A list shows each bet with its flags now. Pick one.
* ``a`` *All Bets*
* ``o`` *Odds Bets*
* ``p`` *Place Bets*
* ``b`` *Buy Bets*
* ``l`` *Lay Bets*
* ``h`` *Hardway Bets*

Each choice opens a list of five changes, for example for place bets:

* ``a`` *Set Place Bets Not-Working*
* ``b`` *Set Place Bets Not-Working Next Roll*
* ``c`` *Set Place Bets Working*
* ``d`` *Set Place Bets Working Next Roll*
* ``e`` *Set Place Bets To Table Default*

Pick one with the arrow keys and Enter, or press its letter. ``.`` or Esc
goes back one step. A notice then says what was done, for example
*Set Place bets to Working: 2 bets changed, 1 already set*. If you have no
bet that can take a flag, the menu says so instead.

Setting Working, Not-Working or Table Default also cancels a Next Roll choice
you made earlier. Taking the odds down to $0 sets that bet back to Table
Default.

Seeing the flags
----------------

The amounts in your bet area tell you:

* An amount that is **dim** is not working on the next roll.
* An amount that is **underlined** has a flag set by you, or a Next Roll
  choice.

So a Place bet that is Not-Working shows dim and underlined. A Place bet that
is on its house rule shows dim during the come out and normal while a point
is set. You can change flags whenever betting is open.

Undoing flag changes
--------------------

A flag change is a single step for Undo, however many bets it changed. Press
``u`` once and each bet gets back the flags it had.

Flags are not saved between games. Bets start on Table Default.
