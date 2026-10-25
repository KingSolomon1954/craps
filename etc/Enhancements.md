## New Features / Enhancements

### Disallow Come/DontCome early

- Early detect legal use of Come/DontCome and improve user experience
  to prevent going down chain of menus only to be rejected after
  entering an amount in the final DialogBetAmount.
- Same for DontPass
  
### Persist Player Bet Preferences
- Save and reload the player's AutoFills, QuickBets, and LastBets across
  game sessions.
- Load saved preferences when the game starts.
- Save changes when appropriate, using the project's existing
  persistence conventions.

### Streaks - New feature - Counts the same number repeats before 7
- example (4:0:57)  the 4 repeated 0 times before a 7 out 57 times
- example (4:1:6)   the 4 repeated 1 time  before a 7 out  6 times
- example (4:3:1)   the 4 repeated 3 times before a 7 out  1 times 

### Stats for Place, Lay, Buy, Any7, CandE, Horn
- track stats for these
- report on them

### All time Bests
- Top n players (winnings)
-- name, winnings, num sessions, play time
-- name, winnings, num sessions, play time
- Top Shooters
-- top n longest shooter rolls ever
-- name, date, num rolls, duration
-- name, date, num rolls, duration

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

### Sounds on/off
- master on/off
- dice rolling
- players sounds
- croupier sounds
- background music/noise


## Done

### LastBet
- Track the amount of the player's most recently accepted bet.
- Track amounts by bet identity, including bet type, point where applicable, and contract bet versus odds bet, consistent with AutoFill's existing distinctions.
- Update LastBet only after the bet has been successfully accepted on the table.
- When pre-filling a new bet amount, use this priority:
  1. Matching AutoFill amount, if configured.
  2. Matching LastBet amount, if available.
  3. Zero otherwise.
- Consider implementing LastBet as a separate class, with responsibilities similar to AutoFill.

