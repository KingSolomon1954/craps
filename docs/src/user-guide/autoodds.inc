.. Conventions are the same as in players.inc.

AutoOdds
========

AutoOdds puts odds behind your contract bets for you. You say what odds you
want for each kind of bet and each point, and after each roll AutoOdds places
them. The odds are your money, so AutoOdds is off until you turn it on.

What you can set
----------------

For each of Pass Line, Don't Pass, Come and Don't Come, and for each of the
points 4, 5, 6, 8, 9 and 10, you can set either:

* an **amount**, such as $50, which is placed as it is; or
* a **multiple**, such as 5x, which is that many times the contract bet. A $10
  Pass Line bet with 5x on the 6 gets $50 odds when 6 is the point.

A point with nothing set gets no automatic odds.

Configuring AutoOdds
--------------------

Press ``C`` for *Control*, ``c`` for *Configure* and then ``a`` for
*AutoOdds*. A list offers:

* **Turn on** or **Turn off**. Turning it off keeps what you set.
* **List Bets**. A notice lists what is set for each bet type.
* **Set AutoOdds Bet**. Pick a bet type, pick a point or *All points*, and
  type an amount like ``50`` or ``$50``, or a multiple like ``3x``. Typing
  ``0`` clears it.
* **3-4-5 preset**. Sets 3x for the 4 and 10, 4x for the 5 and 9, and 5x for
  the 6 and 8, for all four bet types.
* **Clear all**. After you confirm, removes everything you set.

Setting AutoOdds while you enter odds
-------------------------------------

While you enter an odds bet, press ``a`` for *AutoOdds - Set or Unset*.
AutoOdds for that bet type and point is set to exactly the amount you have
typed, in dollars. If you have typed nothing or 0, it is unset. Pressing
``a`` does not place the bet. You can press Enter afterwards to place the
odds you typed.

If AutoOdds is off, the notice reminds you to turn it on.

When it acts
------------

After each roll, once the point and the Come bets have settled and betting is
open again, AutoOdds looks at your contract bets that have a point and no
odds. It places the amount you set for that bet type and point:

* **The table's limits apply.** The amount is rounded down to the table's odds
  increment and is no more than the table allows. If it is less than one
  increment, the odds are skipped and a notice tells you why.
* **Each bet is tried once.** If you take odds down by hand, or the table
  refuses them, AutoOdds leaves that bet alone from then on.
* **Not enough money** skips the odds. They are not tried again.
* **One undo.** Everything AutoOdds places after a roll is one step for
  Undo (``u``).

While AutoOdds is on, the bottom bar shows *AutoOdds on*. After it places odds
it shows how many, such as *AutoOdds: 2 placed*, until the next roll.

AutoOdds works for the player you are playing as, and its settings are kept
in that player's file with the rest of the player's settings.
