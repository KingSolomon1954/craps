//----------------------------------------------------------------
//
// File: GameEvents.h
//
//----------------------------------------------------------------

#pragma once
#include <controller/AnimationSpeed.h>
#include <controller/BetFlags.h>
#include <controller/BetSpec.h>
#include <controller/RollHistoryFormat.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <craps/LastRollStats.h>
#include <gen/MoneyUtils.h>
#include <gen/ErrorPass.h>
#include <string>

namespace Ctrl {

enum class EventSource
{
    Unset,
    Model,                    ///< Craps engine originated event
    View,                     ///< View ( Cui / Gui ) originated event
    Controller                ///< Controller ( Ctrl ) originated event
};

enum class EventType
{
    Unset,                     ///< uninitialized, treat as illegal

    // Requests
    ReqRollDice,
    ReqMakeBet,
    ReqOddsBet,
    ReqRemoveBet,
    ReqUndoBet,
    ReqApplyQuickBets,
    ReqEditQuickBets,
    ReqEditAutoOdds,
    ReqSetBetFlags,
    ReqSetAnimationSpeed,
    ReqSetRollHistoryFormat,
    ReqSwitchPlayer,
    ReqManagePlayer,
    ReqTerminate,

    // Responses
    RspMakeBet,
    RspOddsBet,
    RspRemoveBet,
    RspUndoBet,
    RspApplyQuickBets,
    RspEditQuickBets,
    RspEditAutoOdds,
    RspSetBetFlags,
    RspChangeBet,              // not yet implemented
    RspSwitchPlayer,
    RspManagePlayer,

    // Unsolicited
    UslBettingClosed,
    UslDiceThrowStart,
    UslDiceNewValue,
    UslResolveBetsStart,
    UslResolveBetsEnd,
    UslPointEstablished,
    UslSevenOut,
    UslPassLineWinner,
    UslNewShooter,
    UslBettingOpened,
    UslAutoOddsApplied,
    UslCountdownRollDice,      // not yet implemented

    UslBetMade,
    UslBetResolved,
    UslBetEvaluationFailed,
    UslBetRemoved,
    UslBetChanged,
    UslUserPlayerSwitched,
    UslPlayerRenamed,
    UslRollHistoryFormatChanged,

    UslTableBetsChanged,
    UslTableSessionStatsChanged,
    UslTableLastRollStatsChanged,
    UslTableBalanceChanged,

    UslPlayerBetsChanged,
    UslPlayerSessionStatsChanged,
    UslPlayerLastRollStatsChanged,
    UslPlayerBalanceChanged,
    UslPlayerJoinedTable,
    UslPlayerLeftTable
};

//----------------------------------------------------------------

using EventId       = std::uint64_t;
using CorrelationId = std::uint64_t;

EventId       getNextEventId();
CorrelationId getNextCorrelationId();

/*-----------------------------------------------------------*//**

@brief Base class for all Events

There are three categories of Events:

@li Request Events (prefix Req) - requesting some action be taken
@li Response Events (prefix Rsp) - response to a request
@li Unsolicited Events (prefix Usl) - something of interest happened

There are three Sources of Events, aligned with the
Model/View/Controller design (See Ctrl::EventSource):

@li Model - The Craps engine
@li View - The CUI or GUI
@li Controller - The Controller

Unique ID (See Ctrl::EventId):

Every event gets a globally unique value for unique ID.

Correlation ID (See Ctrl::CorrelationId):

Every event also carries a correlationId allowing users to match up
Requests with Responses.  By default, for convenience, the correlationId
is set to the uniqueId. In the case of Response Events, the caller is
responsible for overwriting the correlationId with the source event's
correlationId.

*/
struct GameEvent
{
    EventSource   source        = EventSource::Unset;  ///< Came from where
    EventType     type          = EventType::Unset;    ///< Name of the Event
    EventId       id            = 0;                   ///< Unique ID
    CorrelationId correlationId = 0;                   ///< Correlation ID

    GameEvent(EventSource source, EventType type)
        : source(source),
          type(type),
          id(getNextEventId()),
          correlationId(getNextCorrelationId())
    {
    }
};

//----------------------------------------------------------------
//
// Unsolicited
//
//----------------------------------------------------------------

struct UslBettingOpened : public GameEvent
{
    UslBettingOpened()
        : GameEvent{EventSource::Model, EventType::UslBettingOpened}
    {}
};

//----------------------------------------------------------------

struct UslBettingClosed : public GameEvent
{
    UslBettingClosed()
        : GameEvent{EventSource::Model, EventType::UslBettingClosed}
    {}
};

//----------------------------------------------------------------

struct UslDiceThrowStart : public GameEvent
{
    unsigned rollCount = 0;
    UslDiceThrowStart()
        : GameEvent{EventSource::Model, EventType::UslDiceThrowStart}
    {}
};

//----------------------------------------------------------------

struct UslDiceNewValue : public GameEvent
{
    unsigned rollCount = 0;
    unsigned val = 0;
    unsigned d1 = 0;
    unsigned d2 = 0;

    UslDiceNewValue()
        : GameEvent{EventSource::Model, EventType::UslDiceNewValue}
    {}
};

//----------------------------------------------------------------

struct UslResolveBetsStart : public GameEvent
{
    UslResolveBetsStart()
        : GameEvent{EventSource::Model, EventType::UslResolveBetsStart}
    {}
};

//----------------------------------------------------------------

struct UslResolveBetsEnd : public GameEvent
{
    UslResolveBetsEnd()
        : GameEvent{EventSource::Model, EventType::UslResolveBetsEnd}
    {}
};

//----------------------------------------------------------------

struct UslPointEstablished : public GameEvent
{
    unsigned point = 0;
    UslPointEstablished()
        : GameEvent{EventSource::Model, EventType::UslPointEstablished}
    {}
};

//----------------------------------------------------------------

struct UslSevenOut : public GameEvent
{
    UslSevenOut()
        : GameEvent{EventSource::Model, EventType::UslSevenOut}
    {}
};

//----------------------------------------------------------------

struct UslPassLineWinner : public GameEvent
{
    UslPassLineWinner()
        : GameEvent{EventSource::Model, EventType::UslPassLineWinner}
    {}
};

//----------------------------------------------------------------

struct UslNewShooter : public GameEvent
{
    Craps::PlayerId playerId;

    UslNewShooter()
        : GameEvent{EventSource::Model, EventType::UslNewShooter}
    {}
};

//----------------------------------------------------------------

struct UslBetMade : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId;
    BetName         betName;
    Gen::Money      contractAmount = 0;
    Gen::Money      oddsAmount     = 0;
    unsigned        pivot          = 0;

    UslBetMade()
        : GameEvent{EventSource::Model, EventType::UslBetMade}
    {}
};

//----------------------------------------------------------------

struct UslBetEvaluationFailed : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId;
    BetName         betName = BetName::Invalid;
    std::string     description;

    UslBetEvaluationFailed()
        : GameEvent{EventSource::Model, EventType::UslBetEvaluationFailed}
    {}
};

//----------------------------------------------------------------

struct UslBetRemoved : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId;
    BetName         betName = BetName::Invalid;

    UslBetRemoved()
        : GameEvent{EventSource::Model, EventType::UslBetRemoved}
    {}
};

//----------------------------------------------------------------

struct UslBetChanged : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId;
    BetName         betName;
    Gen::Money      contractAmount = 0;
    Gen::Money      oddsAmount     = 0;
    unsigned        pivot          = 0;

    UslBetChanged()
        : GameEvent{EventSource::Model, EventType::UslBetChanged}
    {}
};

//----------------------------------------------------------------

struct UslBetResolved : public GameEvent
{
    Craps::PlayerId playerId;                          ///< Player ID
    BetName         betName        = BetName::Invalid; ///< Bet Name
    Craps::BetId    betId          = 0;                ///< Bet ID
    Gen::Money      amountWin      = 0;                ///< Amount won
    Gen::Money      amountLose     = 0;                ///< Amount lost
    Gen::Money      returnToPlayer = 0;                ///< Due to "Off"
    Gen::Money      commission     = 0;                ///< Deducted Buy/Lay

    UslBetResolved()
        : GameEvent{EventSource::Model, EventType::UslBetResolved}
    {}
};

//----------------------------------------------------------------

struct UslTableBetsChanged : public GameEvent
{
    unsigned   numBetsOnTable;
    Gen::Money amountOnTable;

    UslTableBetsChanged()
        : GameEvent{EventSource::Model, EventType::UslTableBetsChanged}
    {}
};

//----------------------------------------------------------------

struct UslTableSessionStatsChanged : public GameEvent
{
    unsigned numBetsWon;
    unsigned numBetsLost;

    UslTableSessionStatsChanged()
        : GameEvent{EventSource::Model, EventType::UslTableSessionStatsChanged}
    {}
};

//----------------------------------------------------------------

struct UslTableLastRollStatsChanged : public GameEvent
{
    Craps::LastRollStats stats;

    UslTableLastRollStatsChanged()
        : GameEvent{EventSource::Model, EventType::UslTableLastRollStatsChanged}
    {}
};

//----------------------------------------------------------------

struct UslTableBalanceChanged : public GameEvent
{
    Gen::Money balance;          // how much left in bank
    int        netBalance;       // profit/loss this session

    UslTableBalanceChanged()
        : GameEvent{EventSource::Model, EventType::UslTableBalanceChanged}
    {}
};

//----------------------------------------------------------------

struct UslPlayerBetsChanged : public GameEvent
{
    Craps::PlayerId playerId;
    unsigned        numBetsOnTable;
    Gen::Money      amountOnTable;

    UslPlayerBetsChanged()
        : GameEvent{EventSource::Model, EventType::UslPlayerBetsChanged}
    {}
};

//----------------------------------------------------------------

struct UslPlayerSessionStatsChanged : public GameEvent
{
    Craps::PlayerId playerId;
    unsigned numBetsWon;
    unsigned numBetsLost;

    UslPlayerSessionStatsChanged()
        : GameEvent{EventSource::Model, EventType::UslPlayerSessionStatsChanged}
    {}
};

//----------------------------------------------------------------

struct UslPlayerLastRollStatsChanged : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::LastRollStats stats;

    UslPlayerLastRollStatsChanged()
        : GameEvent{EventSource::Model, EventType::UslPlayerLastRollStatsChanged}
    {}
};

//----------------------------------------------------------------

struct UslPlayerBalanceChanged : public GameEvent
{
    Craps::PlayerId playerId;
    Gen::Money      balance;     // how much left in bank
    int             netBalance;  // profit/loss this session

    UslPlayerBalanceChanged()
        : GameEvent{EventSource::Model, EventType::UslPlayerBalanceChanged}
    {}
};

//----------------------------------------------------------------

struct UslPlayerJoinedTable : public GameEvent
{
    Craps::PlayerId playerId;

    UslPlayerJoinedTable()
        : GameEvent{EventSource::Model, EventType::UslPlayerJoinedTable}
    {}
};

//----------------------------------------------------------------

struct UslPlayerLeftTable : public GameEvent
{
    Craps::PlayerId playerId;

    UslPlayerLeftTable()
        : GameEvent{EventSource::Model, EventType::UslPlayerLeftTable}
    {}
};

//----------------------------------------------------------------
//
// Requests
//
//----------------------------------------------------------------

struct ReqMakeBet : public GameEvent
{
    Craps::PlayerId playerId;
    BetName         betName = BetName::Invalid;
    unsigned        pivot = 0;
    Gen::Money      contractAmount = 0;

    ReqMakeBet()
        : GameEvent{EventSource::View, EventType::ReqMakeBet}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqMakeBet. A player has one bet per spot, so asking for a
/// bet the player already has changes that bet's amount instead of
/// making a second one. Then changed is true, previousAmount is what it
/// was, and betId is the existing bet.
struct RspMakeBet : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    bool            changed = false;
    Gen::Money      previousAmount = 0;
    Gen::ErrorPass  error;

    RspMakeBet()
        : GameEvent{EventSource::Controller, EventType::RspMakeBet}
    {}
};

//----------------------------------------------------------------

/// The person wants to play as a different player.
struct ReqSwitchPlayer : public GameEvent
{
    Craps::PlayerId playerId;   ///< The player to become the user player

    ReqSwitchPlayer()
        : GameEvent{EventSource::View, EventType::ReqSwitchPlayer}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqSwitchPlayer. If not switched, the user player is
/// unchanged and still at the table, and error says why.
struct RspSwitchPlayer : public GameEvent
{
    bool            switched = false;
    Craps::PlayerId fromPlayerId;   ///< The user player when asked
    Craps::PlayerId toPlayerId;     ///< The player asked for
    Gen::ErrorPass  error;

    RspSwitchPlayer()
        : GameEvent{EventSource::Controller, EventType::RspSwitchPlayer}
    {}
};

//----------------------------------------------------------------

/// The user player changed. Sent after RspSwitchPlayer, only when the
/// switch happened. Everything the View shows about the user is stale.
struct UslUserPlayerSwitched : public GameEvent
{
    Craps::PlayerId fromPlayerId;
    Craps::PlayerId toPlayerId;

    UslUserPlayerSwitched()
        : GameEvent{EventSource::Controller, EventType::UslUserPlayerSwitched}
    {}
};

//----------------------------------------------------------------

/// What ReqManagePlayer asks for.
enum class PlayerAction
{
    Rename,        ///< playerId gets the name
    Create,        ///< a new user player with the name
    Delete,        ///< playerId, a user player, is deleted
    BuddyJoins,    ///< buddy playerId sits at the table
    BuddyLeaves    ///< buddy playerId leaves the table
};

/// The person wants to change the roster of players or who is at the
/// table. Answered by RspManagePlayer.
struct ReqManagePlayer : public GameEvent
{
    PlayerAction    action = PlayerAction::Rename;
    Craps::PlayerId playerId;   ///< Not used by Create
    std::string     name;       ///< Used by Rename and Create
    std::string     description;///< Used by Create; may be empty

    ReqManagePlayer()
        : GameEvent{EventSource::View, EventType::ReqManagePlayer}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqManagePlayer. If not ok, nothing changed and error says
/// why. For Create, playerId is the new player.
struct RspManagePlayer : public GameEvent
{
    PlayerAction    action = PlayerAction::Rename;
    bool            ok = false;
    Craps::PlayerId playerId;
    std::string     name;       ///< The player's name after the change
    Gen::ErrorPass  error;

    RspManagePlayer()
        : GameEvent{EventSource::Controller, EventType::RspManagePlayer}
    {}
};

//----------------------------------------------------------------

/// A player's name changed. Names the View has kept are stale.
struct UslPlayerRenamed : public GameEvent
{
    Craps::PlayerId playerId;
    std::string     newName;

    UslPlayerRenamed()
        : GameEvent{EventSource::Controller, EventType::UslPlayerRenamed}
    {}
};

//----------------------------------------------------------------

struct ReqOddsBet : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    Gen::Money      oddsAmount = 0;
    /// Count this change as part of the previous undo step, not a new one
    bool            undoJoinsPrevious = false;

    ReqOddsBet()
        : GameEvent{EventSource::View, EventType::ReqOddsBet}
    {}
};

//----------------------------------------------------------------

struct RspOddsBet : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    Gen::ErrorPass  error;

    RspOddsBet()
        : GameEvent{EventSource::Controller, EventType::RspOddsBet}
    {}
};

//----------------------------------------------------------------

/// Change how the dice roll animation plays, for the rest of this game.
/// The choice is remembered at quit. No response.
struct ReqSetAnimationSpeed : public GameEvent
{
    AnimationSpeed speed = AnimationSpeed::Normal;

    ReqSetAnimationSpeed()
        : GameEvent{EventSource::View, EventType::ReqSetAnimationSpeed}
    {}
};

//----------------------------------------------------------------

/// Change how the roll history is shown, for the rest of this game.
/// The choice is remembered at quit. The answer is
/// UslRollHistoryFormatChanged.
struct ReqSetRollHistoryFormat : public GameEvent
{
    RollHistoryFormat format = RollHistoryFormat::Spaced;

    ReqSetRollHistoryFormat()
        : GameEvent{EventSource::View, EventType::ReqSetRollHistoryFormat}
    {}
};

//----------------------------------------------------------------

/// The roll history format is now as given. The View redraws.
struct UslRollHistoryFormatChanged : public GameEvent
{
    RollHistoryFormat format = RollHistoryFormat::Spaced;

    UslRollHistoryFormatChanged()
        : GameEvent{EventSource::Controller, EventType::UslRollHistoryFormatChanged}
    {}
};

//----------------------------------------------------------------

/// Take a bet off the table. Funds (contract and odds) return to the player.
struct ReqRemoveBet : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    /// Count this removal as part of the previous undo step, not a new
    /// one. Set for every removal after the first in a batch.
    bool            undoJoinsPrevious = false;

    ReqRemoveBet()
        : GameEvent{EventSource::View, EventType::ReqRemoveBet}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqRemoveBet. When accepted, betName/pivot say what was
/// removed and returned is the money that went back to the player.
struct RspRemoveBet : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    BetName         betName = BetName::Invalid;
    unsigned        pivot = 0;
    Gen::Money      returned = 0;
    Gen::ErrorPass  error;

    RspRemoveBet()
        : GameEvent{EventSource::Controller, EventType::RspRemoveBet}
    {}
};

//----------------------------------------------------------------

/// Take back the player's most recent bet change since the last roll.
struct ReqUndoBet : public GameEvent
{
    Craps::PlayerId playerId;

    ReqUndoBet()
        : GameEvent{EventSource::View, EventType::ReqUndoBet}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqUndoBet. When accepted, description says what was done,
/// e.g. "Took down your $10 Place 6 bet". Otherwise error says why, such
/// as nothing to undo or betting being closed.
struct RspUndoBet : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    std::string     description;
    Gen::ErrorPass  error;

    RspUndoBet()
        : GameEvent{EventSource::Controller, EventType::RspUndoBet}
    {}
};

//----------------------------------------------------------------

/// Apply the named QuickBets entry for a player. Always answers with
/// RspApplyQuickBets. The bet and balance events follow as for any bet.
struct ReqApplyQuickBets : public GameEvent
{
    Craps::PlayerId playerId;
    std::string     name;           ///< Name of the entry to apply

    ReqApplyQuickBets()
        : GameEvent{EventSource::View, EventType::ReqApplyQuickBets}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqApplyQuickBets. accepted means the entry was applied, even
/// if some of its bets were skipped; see the counts. Otherwise error says
/// why nothing was tried, such as betting being closed or no such entry.
struct RspApplyQuickBets : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    std::string     name;
    unsigned        placed         = 0;
    unsigned        changed        = 0;
    unsigned        alreadyInPlace = 0;
    unsigned        skipped        = 0;
    std::string     headline;       ///< e.g. "3 placed, 1 skipped"
    std::string     skippedDetail;  ///< One line per skipped bet
    Gen::ErrorPass  error;

    RspApplyQuickBets()
        : GameEvent{EventSource::Controller, EventType::RspApplyQuickBets}
    {}
};

//----------------------------------------------------------------

/// Change the player's QuickBets. Always answers with RspEditQuickBets.
///
/// | Action    | Uses                                                    |
/// | --------- | ------------------------------------------------------- |
/// | Create    | name (the new entry), spec                              |
/// | AddBet    | name (existing entry), spec                             |
/// | Rename    | name (existing entry), newName                          |
/// | SetAmount | name, index (of the bet in the entry), amount           |
/// | RemoveBet | name, index                                             |
/// | Delete    | name                                                    |
struct ReqEditQuickBets : public GameEvent
{
    enum class Action { Create, AddBet, Rename, SetAmount, RemoveBet, Delete };

    Craps::PlayerId playerId;
    Action          action = Action::Create;
    std::string     name;           ///< New entry's name, or the entry to change
    BetSpec         spec;           ///< The bet to save (Create, AddBet)
    std::string     newName;        ///< Rename
    std::size_t     index  = 0;     ///< SetAmount, RemoveBet: which bet
    Gen::Money      amount = 0;     ///< SetAmount

    ReqEditQuickBets()
        : GameEvent{EventSource::View, EventType::ReqEditQuickBets}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqEditQuickBets. When accepted, message says what was done,
/// e.g. Added Place 6 $24 to "Place Hi". Otherwise error says why not.
/// Changes are kept in the player's prefs and saved with the players.
struct RspEditQuickBets : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    std::string     name;
    std::string     message;
    Gen::ErrorPass  error;

    RspEditQuickBets()
        : GameEvent{EventSource::Controller, EventType::RspEditQuickBets}
    {}
};

//----------------------------------------------------------------

/// Change the player's AutoOdds settings. Always answers with
/// RspEditAutoOdds. Nothing here touches the table.
///
/// | Action     | Uses                                                   |
/// | ---------- | ------------------------------------------------------ |
/// | SetEnabled | enabled                                                |
/// | Set        | betName, point (0 = all points), and multiple or       |
/// |            | amount. An amount is exact dollars; otherwise multiple |
/// |            | is times the contract. Both zero unsets.               |
/// | Preset345  | nothing; 3x on 4 and 10, 4x on 5 and 9, 5x on 6 and 8  |
/// | Clear      | nothing; clears every setting                          |
struct ReqEditAutoOdds : public GameEvent
{
    enum class Action { SetEnabled, Set, Preset345, Clear };

    Craps::PlayerId playerId;
    Action          action   = Action::SetEnabled;
    bool            enabled  = false;
    BetName         betName  = BetName::Invalid;
    unsigned        point    = 0;
    unsigned        multiple = 0;
    Gen::Money      amount   = 0;

    ReqEditAutoOdds()
        : GameEvent{EventSource::View, EventType::ReqEditAutoOdds}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqEditAutoOdds. When accepted, message says what was done,
/// e.g. "AutoOdds for PassLine 6: set to 5x". Otherwise error says why not.
/// Changes are kept in the player's prefs and saved with the players.
struct RspEditAutoOdds : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    std::string     message;
    Gen::ErrorPass  error;

    RspEditAutoOdds()
        : GameEvent{EventSource::Controller, EventType::RspEditAutoOdds}
    {}
};

//----------------------------------------------------------------

/// Sent after betting reopens when AutoOdds put odds on, or wanted to and
/// could not. The odds bets themselves are announced by the usual bet
/// events. headline is e.g. "AutoOdds: 2 placed", skippedDetail has one
/// line per odds not placed.
struct UslAutoOddsApplied : public GameEvent
{
    Craps::PlayerId playerId;
    unsigned        placed  = 0;
    unsigned        skipped = 0;
    std::string     headline;
    std::string     skippedDetail;

    UslAutoOddsApplied()
        : GameEvent{EventSource::Controller, EventType::UslAutoOddsApplied}
    {}
};

//----------------------------------------------------------------

/// Turn bets off or on. Always answers with RspSetBetFlags. Betting must
/// be open.
///
/// scope says which bets: Bet is the one named by betId, the others are
/// every bet of the player that can take a flag in that category. change
/// says what to do; see BetFlags::newState().
struct ReqSetBetFlags : public GameEvent
{
    Craps::PlayerId     playerId;
    BetFlags::Scope     scope  = BetFlags::Scope::Bet;
    Craps::BetId        betId  = 0;
    BetFlags::Change    change = BetFlags::Change::NotWorking;

    ReqSetBetFlags()
        : GameEvent{EventSource::View, EventType::ReqSetBetFlags}
    {}
};

//----------------------------------------------------------------

/// Answer to ReqSetBetFlags. When accepted, message says what was done,
/// e.g. "Odds bets set to Not-Working: 2 changed"; changed and unchanged
/// count the bets, unchanged being those that already had that state.
/// Otherwise error says why not. A change is one undo step.
struct RspSetBetFlags : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    std::string     message;
    unsigned        changed   = 0;
    unsigned        unchanged = 0;
    Gen::ErrorPass  error;

    RspSetBetFlags()
        : GameEvent{EventSource::Controller, EventType::RspSetBetFlags}
    {}
};

//----------------------------------------------------------------

struct ReqRollDice : public GameEvent
{
    ReqRollDice()
        : GameEvent{EventSource::View, EventType::ReqRollDice}
    {}
};

//----------------------------------------------------------------

struct ReqTerminate : public GameEvent
{
    ReqTerminate()
        : GameEvent{EventSource::View, EventType::ReqTerminate}
    {}
};

/*-----------------------------------------------------------*//**

@enum EventSource

@brief The source of an Event

The source aligns with the Model/View/Controller (MVC) design
where:

@li Model = the event came from the Craps Engine (lib-craps)
@li View = the event came from the CUI (lib-cui) or GUI (lib-gui)
@li Controller = the event came from the Controlloer (lib-controller)
*/

/*-----------------------------------------------------------*//**

@enum EventType

@brief The name of the Event

A unique name that describes the purpose of the Event.

*/

/*-----------------------------------------------------------*//**

@typedef EventId

@brief A unique ID

Every event gets a unique value for unique ID.

*/

/*-----------------------------------------------------------*//**

@typedef CorrelationId

@brief An ID to match Response Events to Request Events

Every event carries a correlationId allowing users to match up Requests
with Responses.  By default, and for convenience, a correlationId is set
to the uniqueId. In the case of Response Events, the caller is
responsible for overwriting the correlationId with the source event's
correlationId.

*/

} // namespace Ctrl

//----------------------------------------------------------------
