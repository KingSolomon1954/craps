
## TODO

### Write script to copy files/assets to install locations

### Refund on quit - unresolved bets back to player's bank
- show a message like: “You left the table with active bets.
- Your chips have been returned to your bank.”
- When switching tables:
- Add a warning like: “You have active bets. Are you sure
- you want to quit and abandon them?”

### Save Config to yaml file

### Need unit test for initial bank money

### Need unit test for undo bet capability

### Need unit test ConfigManagerTest

### Minimum bets are wrong.
- Allow $1 bets for proposition bets, hardways,
- fix min field bet

### Add world bet

### Add Roll History Format A,B,C,D to multi-configuration
- Modify Roll History to grab Format from multi-configuration

### LocationManager needs smarter options for window placement

### Need More MenuBetting implementation

### Implement "Unknown" player start up logic

### Implement the many MenuConfiguration options

### Need to inhibit various actions during animation roll

### Track contract-only amounts separately in BetStats

BetStats records contract and odds amounts under the same bet-type
key. The displayed expectation therefore uses the selected bet profile's
house edge against the combined recorded amount. That is an estimate,
not an exact expected loss, because the statistics don't retain the
contract-only amount separately. A future model-level refinement could
track that amount independently.

### Implement QuickBet::applyQuickBet for new CUI design

### Implement MenuQuickBet

### Implement doQuickBet in MenuBetting

### Implement UndoLastBet in MenuBetting

### Implement RemoveBets in MenuBetting

### Implement doBetFlags in MenuBetting

### Implement WindowPlayerArea - show bets on screen

### Implement WindowMessageArea

### Implement ReqMakeBet, to include UndoManager

### Fix Place bets being removed from table after a win

I'd introduce an explicit distinction in the decision/resolution model
between:
- Outcome resolved: the roll produced a win or loss.
- Wager remains active: the original bet continues after that outcome.

Bottom line: evalPlace() marks the win as a final decision,
Player::processWin() removes the bet from the player, and
CrapsTable::trimTableBets() removes it from the table. All three parts
need to agree on the intended lifecycle of a Place bet.

(Done) Early illegal bet test - jump to error immediatly
(Done) Place, Buy, Lay  bet odds hints
(Done) Add LastFill feature
(Done) Add payout hint to DialogBetAmount
(Done) Fix window width of MenuOddsBet
(Done) Fix multiple odds bets for same bet and number MenuOddsBet
(Done) Setting an Odds bet shows as two bets on the table and player brief
(Done) Fixed MenuOddsBet - saw two "Bet::Invalid", CarrierBet init problem?
(Done) Debug & fix WindowPlayerBrief
(Done) Added new events to populate WindowPlayerBrief
(Done) Debug & fix WindowHouseBrief
(Done) New events needed to populate WindowHouseBrief
(Done) Complete the first round-trip bet made by the CUI
(Done) Add DialogConfirm when quitting game tied to operations manager\
(Done) OperationTracker needs correlationId matching\
(Done) OperationTracker (manages interactive lifecycle)\
(Done) Implemented ReqAutoFill and ReqQuickBet\
(Done) Added new event UslQuickBetChanged. Published it\
(Done) Added new event UslAutoFillChanged. Published it\
(Done) Made LineInput processing resuable. New class LineInput\
(Done) Fix DialogAmount line input processing, show cursor\
(Done) Elminated configure() in menus, using CarrierBet instead\
(Done) In CUI, implemented DialogAmount\
(Done) Bring DialogMessage and DialogError into alignment\
(Done) In CUI, implemented DialogError\
(Done) Fix GitHub Actions. Docs are once again built and deployed\
(Done) Update design doc\
(Done) Integrated DialogMessage, added colorized introducers\
(Done) Beefed up ErrorPass and propagate throughout, needs more attention\
(Done) Introduced zeus::expected library\
(Done) Fix invisible MenuControl when child menus are cancelled\
(Done) Added skeleton MenuConfig\
(Done) Fix key handling ScreenCrapsTable (currently forced to ctrl-c to exit)\
(Done) UndoManager implements instance(), remove from Globals\
(Done) Added QuickBets back into compilation\
(Done) Only CrapsTypes define using PlayersId, BetIds, and RecentRolls\
(Done) Fix shared structure between CrapsRaders and AutoFill\
(Done) Implement PlayerAreaAllPlayers::onBetResolved()\
(Done) Refactor PlayerAreaAllPlayers use a table\
(Done) Implement PlayerAreaOnePlayer::onBetResolved()\
(Done) Implement PlayerAreaOnePlayer\
(Done) In CuiThread, separate out the processing into a dispatcher\
(Done) In PlayerBrief area, display player name in his matchingcolor\
(Done) Add shooter colorized name to animation window\
(Done) Fix advanceShooter processing and initalization\
(Done) Implemented onPlayerJoinTable/LeftTable in PlayerAreaAllPlayers\
(Done) PlayerAreaAllPlayers needs implementation for onBetMade() updates\
(Done) Implement Advance shooter event, WindowPlayerBrief updated\
(Done) Re-factor these events: UslBetMade, UslTableResults, UslPlayerResults\
(Done) PlayerAreaAllPlayers implemented\
(Done) Divide WindowPlayerArea into several files\
(Done) Need to display table rules in Title bar\
(Done) Created WindowTitleBar, integrated and works\
(Done) Created WindowRollHistory, integrated and works\
(Done) MenuBetting processes "r" and rolls dice\
(Done) Implement first round-trip events for roll dice event\
(Done) Initial implementation of GameEvents.h\
(Done) Implement Controller::EventHandler skeleton\
(Done) Redesign CUI, lib-controller, lib-craps\
(Done) Fix failing unit-test, lastRollStats\
(Done) Switch to lib-gen EventManager. Eliminate 2 different event managers.\
(Done) First cut geometry management/control over menu placement\
(Done) In CUI, write ConfirmDialog class\
(Done) In CUI, need DialogBase\
(Done) Hook up ScreenCrapsTable::onAttach(MenuBetting)\
(Done) Fix ScreenCrapsTable initial corrupted drawing\
(Done) Add/fix explicit shutdown in GameMain\
(Done) Logger initialization, turn off console logging just before view\
(Done) Fix logger writing to file\
(Done) Logger supports writing to file and console same time\
(Done) Add startup terminal resizing to CUI logic\
(Done) Organize lib-cui into subfolders\
(Done) Add shortCircuit() to MenuOddsBet to skip menu if only one odds bet\
(Done) LayoutCrapsScreen draw all Panel borders and fix-up on behalf of Panels\
(Done) LayoutCrapsScreen made into a static class\
(Done) Fix up NavBar to be a Panel\
(Done) Use single layout class that all Panels reference\
(Done) PaneBase class for Windows attached to full screens\
(Done) EventManager available as a lib-gen class\
(Done) Implement WindowPlayerArea inital drawing and switch one/all player\
(Done) Implement first cut at WindowNavBar \
(Done) Surface base class to implement releaseNcursesResources()\
(Done) ConsoleManager provides pParent surface to form operation change\
(Done) Implemented first dynamic menu MenuOddsBet\
(Done) Implemented first static menu MenuPlaceBet\
(Done) Implement CarrierBet, to pass down menu chain\
(Done) Surface base class to own pWin_\
(Done) Implement ScreenBase, MenuBase, Surface\
(Done) ConsoleManager needs to collect unique surface pointers.\
(Done) ConsoleManager needs a prepareForShutdown() over the collection.\
(Done) ConsoleView -> ConsoleManager\
(Done) ScreenTableCraps to use its own pWin_, not stdscr\
(Done) Implement initial ScreenCrapsTable draw window\
(Done) Change API for controller events (emitters, handlers, readers)\
(Done) Switch to unique_ptr for event loop ovnership\
(Done) ConsoleView delegates to ViewEventEmitter\
(Done) ConsoleView inherits from ViewInterface\
(Done) Implement controller events infrastructure, emitters, handlers, reader\
(Done) Modify lifetime model for menus, use instance() accessor\
(Done) Design menu hierarchy\
(Done) Undo capability, UndoManager class, UndoBet classes\
(Done) Build up CrapsInterface for UI interaction\
(Done) Populate CrapsInterface, organized around players, bets, table\
(Done) Write unit test MultiLayerConfigTest\
(Done) load Player choices by reading dir\
(Done) Load config.yaml with arrays into multi-layer config\
(Done) Switch to use file names to load, instead of TableId or PlayerId\
(Done) Implement PlayerManifest\
(Done) Fix Player startup/init\
(Done) Load n Players on startup/init\
(Done) Load Player from yaml file\
(Done) Save Player to yaml file\
(Done) stats PlayerStats\
(Done) Player::leaveTable(), refund on leave\
(Done) Create initial unit-test for Player\
(Done) Eliminate lib-controller from lib-craps\
(Done) Move EventManager inside of CrapsTable.cpp - went a different way\
(Done) Add accessor for BankStats from CrapsTable\
(Done) Eliminate PlayerManager from CrapsTable.cpp\
(Done) Eliminate ConfigManager from CrapsTable.cpp\
(Done) CrapsTable unit test relies on a working Player - maybe mock Player\
(Done) Need unit test for bank refills\
(Done) Fix sessionStartBalance to continue session to session\
(Done) Redesign bankStats to fall under Bank class\
(Done) maxFieldBet_ and friends need to read/write YAML()\
(Done) Not needed - make a static CrapsBet function that creates a shared_ptr\
(Done) Move testRollDice() under -DUNIT_TEST flag\
(Done) Update CrapBet unit-test, for any bet and setOdds exceeding table limit\
(Done) Need to add DontPlace bets to the game - abandoned\
(Done) Buy bets and Lay bets have wrong payouts - need updating\
(Done) Money calculation uses multiples and pays 1:1 odds on leftover\
(Done) Allow odds bets less than contract bet - pay even money\
(Done) Design change, CrapsTable accepts CrapsBets. eliminate CrapsBetIntfc\
(Done) change changeOddsAmount() -> setOddsAmount()\
(Done) change changeContractAmount() -> setContractAmount()\
(Done) Rules for CrapsTables (10x odds, limits, automation)\
(Done) Load Table from yaml file (CrapsTable.cpp)\
(Done) Save Table from yaml file (CrapsTable.cpp)\
(Done) tables.yaml manifest\
(Done) eliminate TableId or TableName, not both\
(Done) single CrapsTable yaml file, includes stats\
(Done) stats CrapsTable returns Session summaries\
(Done) unit test for SessionHistory class\
(Done) Cap SessionHistory to a max number. Roll out oldest.\
(Done) change TableStats::lastSessionDate --> sessionDate (better yet eliminate)\
(Done) CrapsTable creates Session summaries\
(Done) Create session history class\
(Done) TableStats saves/loads bank/money\
(Done) TableStats add lastSessionDate, lastSessionDuration, maxSessionDuration\
(Done) Timepoint gets duration with days conversions, needed by YAML files\
(Done) Able to read/load from config.yaml\
(Done) Added Debug macros, beefed up logging\
(Done) Added logging class\
(Done) TableStats reads, modifies, writes alltime stats\
(Done) Starting tableId comes from config file\
(Done) TableStats serializes, deserializes\
(Done) TableStats implemented TableStats::merge(sessionStats)\
(Done) TableStats unit test add tests for betting stats\
(Done) TableStats signature recordWin takes a reference not a pointer\
(Done) CrapsTable returns TableStats\
(Done) TableStats unit test add tests for dice rolls\
(Done) TableStats needs fencing around recordWin/Lose/Keep rolls\
(Done) TableStats implement "n" back dice roll history\
(Done) TableStats needs refactoring of recordWin/Lose/Keep rolls\
(Done) stats TableStats switch to recordWin(), recordLose(), recordKeep()\
(Done) stats design\
(Done) stats implementation\
(Done) Update Shooter stats \
(Done) TimerManager compiles\
(Done) Unit test for TimerManager\
(Done) Eliminate GameControllerIntfc in favor of Events and EventLoop\
(Done) Implement minimal game loop\
(Done) Add getBool() to MultiLayerManager
