## New Features / Enhancements

### Disallow Come/DontCome early

- Early detect legal use of Come/DontCome and improve user experience
  to prevent going down chain of menus only to be rejected after
  entering an amount in the final DialogBetAmount.
- Same for DontPass
  
### All time Bests
- Top n players (winnings)
-- name, winnings, num sessions, play time
-- name, winnings, num sessions, play time
- Top Shooters
-- top n longest shooter rolls ever
-- name, date, num rolls, duration
-- name, date, num rolls, duration

### Sounds on/off
- master on/off
- dice rolling
- players sounds
- croupier sounds
- background music/noise

### MultiBet

I've been contemplating a new convenience feature for betting.  The user
can pre-configure and later apply multiple bets with a single menu
selection. There are probably a handful of useful use cases for this,
such as:

- Place all numbers minus the point.
- Place the three Hi numbers
- Place the three Lo numbers
- Place the 6 and 8
- Make a Passline and a C&E bet
- Make a DontPass and a Any711 bet
- All the hardways
- Buy the 4 & 10
- Lay the 4 & 10

The user would pre-configure the exact bets to apply, and
give the collection a unique string name.

The "MultiBet" menu, which would be built dynamically, allows the user
to select from the user's pre-configured MultiBets. The mechanism would
work similar to the current QuickBet feature. Thinking about it further,
a QuickBet is really a MultiBet with just a single pre-configured
bet. Maybe MultiBet replaces QuickBet entirely.

The difficulty I forsee with this is error handling and possibly roll
back.  What to do if a MultiBet encounters errors during playback. Say
it successfully puts down a field bet and then a PassLine bet is
rejected (perhaps wrong time for it). Does it keep going in the face of
errors? Does it stop and roll back any already successfully applied
bets? Maybe the user has insufficient funds in the middle of applying a
sequence.

Thinking about MultiBet leads me to now consider how in the future the
game might support automation. The Automation capability, in my earlier
thinking, was meant primarily for buddy players such that buddies can
react to various conditions on the table that trigger betting, applying
randomization to making different bets, timer triggered betting and so
on. Making "n" number of bets in sequence, such as a MultiBet might be
considered just a variant of Automation that uses a manual trigger
without any spacing between bets.

There are many open items that need tackling way before this. And if you
adhere to the incremental development philosophy, I should probably add
MultiBet first, then worry about Automation later.

Wondering if you have thoughts on this?

#### AI Response

I think MultiBet is the right next step. Your instinct to build it
before Automation is also right, as long as the MultiBet is shaped so
Automation can reuse it. I looked at `QuickBet` first, and the existing
structure fits well.

**MultiBet replacing QuickBet**

Replacing it works. `QuickBet` is already an ordered list of `{betName,
pivot, isOddsBet, amount}` entries held per player in `PlayerPrefs`. A
MultiBet is a named list of those same entries, so the entry struct and
`BetEntryYaml` carry over unchanged. A single-bet QuickBet becomes a
one-entry MultiBet. The change is a name, a vector of entries, and
index-based apply becoming apply-by-name. A migration isn't needed,
since you don't want backward compatibility during development.

**The error question**

Rollback is possible but I'd avoid it. Your own rules point the same
way: when a bet is placed, it goes down. Rolling back would mean
removing bets, and that fights the undo and one-bet-per-spot logic. It
also makes `makeOrChangeBet` replacements hard to reverse, because the
earlier amount would have to be restored. I'd make the behavior apply
best-effort and then report the result:

- A bet that can't be placed is skipped, and the sequence continues. That matches how a person would behave at the table.
- The result is a per-entry list, with the entry, success or failure, and the reason, such as "wrong time", "insufficient funds" or "over limit".
- A one-line summary goes to the player, like "5 of 6 bets placed; Passline rejected (point is on)". Full details can go to the log or a detail dialog.
- A pre-check before placing anything is an optional extra. It would validate everything first, including whether the total fits the bank, and refuse the whole MultiBet if it fails. I'd leave it out of the first version. The check is harder than it looks, because bets interact: the Place numbers depend on the current point, and a Field bet placed while one already exists is a change, not an addition. Best-effort with clear reporting covers most of the value.

Insufficient funds mid-sequence follows the same rule. Earlier bets stay
and later ones are skipped with that reason. If you'd rather have
all-or-nothing as an option, it could later be a per-MultiBet flag, and
I'd then implement it with a dry-run check rather than rollback.

**The "minus the point" case**

"Place all numbers minus the point" makes the entry list dynamic, so a
plain list of fixed entries won't express it. I'd handle it with a small
number of entry flavors rather than a rule language. A plain `Place`
entry that targets the current point could be skipped automatically,
with the reason "number is the point". Most of your examples are plain
lists. Place all but the point, the Hi numbers, the Lo numbers and 6/8
are plain Place entries, so the only special rule is skipping a number
that is the point. That seems simple enough to get right.

**Relation to Automation**

Your framing matches how I'd build it. Automation would be made of three
parts: a trigger (a table condition, a timer or a random roll), a
selection (which bets), and the applier. A MultiBet is a manual trigger
plus a selection with no delay between bets. If the applier is a
function that takes a list of entries and returns the per-entry results,
with no CUI code inside, then manual MultiBet and buddy Automation use
the same code. The design choice to make now is that the applier lives
in the controller or model and returns structured results. Display
belongs in the View.

**Suggested incremental steps**

1. Replace `QuickBet` with `MultiBet`: named sets in `PlayerPrefs`, YAML
   under `Prefs`, and the apply function returning per-entry
   results. Only the pre-configuration path would exist at first, and
   existing QuickBet behavior is covered by a one-entry set.
2. Add the apply-by-name event and the response message that carries the
   results.
3. Build the dynamic menu from the player's MultiBets, with a result
   line after each apply.
4. Add a way to create and edit sets. This is probably the largest piece
   of CUI work, since it needs a name prompt and a bet-and-amount
   picker. You may want to hand-edit YAML at first to test steps 1–3.

Three decisions would help me before I start:

- Is best-effort with a summary acceptable, with all-or-nothing left for
  later?
- When two MultiBets overlap, or a MultiBet touches a spot that already
  has a bet, is the one-bet-per-spot rule enough (set the amount, don't
  add)?
- Do you want the skip-the-point rule in the first version, or only
  plain lists first?

### Betting automation
- play pass each roll (amount)
- play odds each roll (amount)
- play dont each roll (amount)
- play dont odds each roll (amount)
- play come each roll (amount)
- play come odds each roll (amount)
- play dont come each roll (amount)
- play dont come odds each roll (amount)
- play place each come out roll (number, amount)
- play dont place each come out roll (number, amount)
- play hardways each come out roll (number, amount)

## Done

### Streaks - New feature - Counts the same number repeats before 7
- example (4:0:57)  the 4 repeated 0 times before a 7 out 57 times
- example (4:1:6)   the 4 repeated 1 time  before a 7 out  6 times
- example (4:3:1)   the 4 repeated 3 times before a 7 out  1 times 

### Stats for Place, Lay, Buy, Any7, CandE, Horn
- track stats for these
- report on them

### Persist Player Bet Preferences
- Save and reload the player's AutoFills, QuickBets, and LastBets across
  game sessions.
- Load saved preferences when the game starts.
- Save changes when appropriate, using the project's existing
  persistence conventions.

### LastBet
- Track the amount of the player's most recently accepted bet.
- Track amounts by bet identity, including bet type, point where applicable, and contract bet versus odds bet, consistent with AutoFill's existing distinctions.
- Update LastBet only after the bet has been successfully accepted on the table.
- When pre-filling a new bet amount, use this priority:
  1. Matching AutoFill amount, if configured.
  2. Matching LastBet amount, if available.
  3. Zero otherwise.
- Consider implementing LastBet as a separate class, with responsibilities similar to AutoFill.

