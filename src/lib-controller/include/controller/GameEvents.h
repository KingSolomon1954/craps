//----------------------------------------------------------------
//
// File: GameEvents.h
//
//----------------------------------------------------------------

#pragma once
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <gen/ErrorPass.h>
#include <atomic>
#include <string>
#include <chrono>
#include <memory>

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
    ReqRemoveBet,              // not yet implemented
    ReqChangeBet,              // not yet implemented
    ReqAutoFill,
    ReqQuickBet,

    ReqTerminate,

    // Responses
    RspMakeBet,
    RspOddsBet,
    RspRemoveBet,              // not yet implemented
    RspChangeBet,              // not yet implemented

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
    UslBetMade,
    UslBetResolved,
    UslBetRemoved,             // not yet implemented
    UslBetChanged,             // not yet implemented
    UslTableBalanceChanged,
    UslPlayerBalanceChanged,

    UslAutoFillChanged,
    UslQuickBetChanged,
    UslPlayerJoinedTable,
    UslPlayerLeftTable,
    UslCountdownRollDice       // not yet implemented
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

struct UslTableBalanceChanged : public GameEvent
{
    Gen::Money balance;          // how much left in bank
    int        netBalance;       // profit/loss this session

    UslTableBalanceChanged()
        : GameEvent{EventSource::Model, EventType::UslTableBalanceChanged}
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

struct UslBetResolved : public GameEvent
{
    Craps::PlayerId playerId;                      ///< Player ID
    BetName         betName    = BetName::Invalid; ///< Bet Name
    Craps::BetId    betId      = 0;                ///< Bet ID
    Gen::Money      amountWin  = 0;                ///< Amount won
    Gen::Money      amountLose = 0;                ///< Amount lost

    UslBetResolved()
        : GameEvent{EventSource::Model, EventType::UslBetResolved}
    {}
};

//----------------------------------------------------------------

struct UslAutoFillChanged : public GameEvent
{
    Craps::PlayerId playerId;                        ///< Player ID  
    BetName         betName   = BetName::Invalid;    ///< Bet Name   
    unsigned        pivot     = 0;                   ///< Bet ID     
    bool            isOddsBet = false;               ///< Amount is for odds
    Gen::Money      amount    = 0;                   ///< Amount
            
    UslAutoFillChanged()
        : GameEvent{EventSource::Model, EventType::UslAutoFillChanged}
    {}
};

//----------------------------------------------------------------

struct UslQuickBetChanged : public GameEvent
{
    Craps::PlayerId playerId;                        ///< Player ID  
    BetName         betName   = BetName::Invalid;    ///< Bet Name   
    unsigned        pivot     = 0;                   ///< Bet ID     
    bool            isOddsBet = false;               ///< Amount is for odds
    Gen::Money      amount    = 0;                   ///< Amount
            
    UslQuickBetChanged()
        : GameEvent{EventSource::Model, EventType::UslQuickBetChanged}
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

struct RspMakeBet : public GameEvent
{
    bool            accepted = false;
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    Gen::ErrorPass  error;

    RspMakeBet()
        : GameEvent{EventSource::Controller, EventType::RspMakeBet}
    {}
};

//----------------------------------------------------------------

struct ReqOddsBet : public GameEvent
{
    Craps::PlayerId playerId;
    Craps::BetId    betId = 0;
    Gen::Money      oddsAmount = 0;

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

struct ReqRollDice : public GameEvent
{
    ReqRollDice()
        : GameEvent{EventSource::View, EventType::ReqRollDice}
    {}
};

//----------------------------------------------------------------

struct ReqAutoFill : public GameEvent              // No response to this
{
    BetName         betName   = BetName::Invalid;  ///< Bet Name   
    unsigned        pivot     = 0;                 ///< Bet ID     
    bool            isOddsBet = false;             ///< Amount is for odds
    Gen::Money      amount    = 0;                 ///< Amount

    ReqAutoFill()
        : GameEvent{EventSource::View, EventType::ReqAutoFill}
    {}
};

//----------------------------------------------------------------

struct ReqQuickBet : public GameEvent              // No response to this
{
    BetName         betName   = BetName::Invalid;  ///< Bet Name   
    unsigned        pivot     = 0;                 ///< Bet ID     
    bool            isOddsBet = false;             ///< Amount is for odds
    Gen::Money      amount    = 0;                 ///< Amount

    ReqQuickBet()
        : GameEvent{EventSource::View, EventType::ReqQuickBet}
    {}
};

//----------------------------------------------------------------

struct ReqTerminate : public GameEvent
{
    ReqTerminate()
        : GameEvent{EventSource::View, EventType::ReqTerminate}
    {}
};

//----------------------------------------------------------------

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












#if 0

enum class EventType
{
    CmdPlayerJoinTable,
    CmdPlayerLeaveTable,
    CmdMakeBet,
    CmdMakeBetAuto,
    CmdBetSetContractAmount,
    CmdBetSetOddsAmount,
    CmdBetSetOffComeOutRoll,
    CmdBetSetOnComeOutRoll,
    CmdBetSetHardwayOff,
    CmdBetSetHardwayOn,
    CmdRollDice,
    CmdSetAutoFill,
    CmdDeleteAutoFill,
    CmdSetQuickBet,
    CmdApplyQuickBet,
    CmdDeleteQuickBetByLookup,
    CmdDeleteQuickBetByIndex,

    ViewSuccess,
    ViewErrorDialog,
    ViewMakeBetSuccess,
    ViewMakeBetAutoSuccess,
    ViewMakeBetAutoError,
    ViewMakeOddsBetSuccess,
    ViewRollDiceCountDown,
    ViewRollDiceAnimation,
    ViewProgramExit
};

//----------------------------------------------------------------
//
// Cmd Events
//
//----------------------------------------------------------------

struct CmdMakeBet : public GameEvent
{
    uint64_t        correlationId;
    Craps::PlayerId playerId;
    BetName         betName;
    Gen::Money      contractAmount;
    size_t          pivot;

    EventType type() const override
    {
        return EventType::CmdMakeBet;
    }
};

struct CmdMakeBetAuto : public GameEvent
{
    uint64_t        correlationId;
    Craps::PlayerId playerId;
    BetName         betName;
    Gen::Money      contractAmount;
    size_t          pivot;

    EventType type() const override
    {
        return EventType::CmdMakeBetAuto;
    }
};

struct CmdBetSetContractAmount : public GameEvent
{
    uint64_t     correlationId;
    Craps::BetId betId;
    Gen::Money   contractAmount;

    EventType type() const override
    {
        return EventType::CmdBetSetContractAmount;
    }
};

struct CmdBetSetOddsAmount : public GameEvent
{
    uint64_t        correlationId;
    Craps::BetId    betId;
    Gen::Money      oddsAmount;

    EventType type() const override
    {
        return EventType::CmdBetSetOddsAmount;
    }
};

struct CmdBetSetOffComeOutRoll : public GameEvent
{
    uint64_t        correlationId;
    Craps::BetId    betId;

    EventType type() const override
    {
        return EventType::CmdBetSetOffComeOutRoll;
    }
};

struct CmdBetSetOnComeOutRoll : public GameEvent
{
    uint64_t        correlationId;
    Craps::BetId    betId;

    EventType type() const override
    {
        return EventType::CmdBetSetOnComeOutRoll;
    }
};

struct CmdBetSetHardwayOff : public GameEvent
{
    uint64_t        correlationId;
    Craps::BetId    betId;

    EventType type() const override
    {
        return EventType::CmdBetSetHardwayOff;
    }
};

struct CmdBetSetHardwayOn : public GameEvent
{
    uint64_t        correlationId;
    Craps::BetId    betId;

    EventType type() const override
    {
        return EventType::CmdBetSetHardwayOn;
    }
};

struct CmdRollDice : public GameEvent
{
    uint64_t       correlationId;
    Craps::TableId tableId;

    EventType type() const override
    {
        return EventType::CmdRollDice;
    }
};

struct CmdPlayerJoinTable : public GameEvent
{
    uint64_t        correlationId;
    Craps::PlayerId playerId;
    Craps::TableId  tableId;
    EventType type() const override
    {
        return EventType::CmdPlayerJoinTable;
    }
};

struct CmdPlayerLeaveTable : public GameEvent
{
    uint64_t        correlationId;
    Craps::PlayerId playerId;
    EventType type() const override
    {
        return EventType::CmdPlayerLeaveTable;
    }
};

struct CmdSetAutoFill : public GameEvent
{
    uint64_t    correlationId;
    BetName     betName;
    size_t      pivot;
    bool        oddsBet;
    Gen::Money  amount;

    EventType type() const override
    {
        return EventType::CmdSetAutoFill;
    }
};

struct CmdDeleteAutoFill : public GameEvent
{
    uint64_t    correlationId;
    BetName     betName;
    size_t      pivot;
    bool        oddsBet;
    Gen::Money  amount;

    EventType type() const override
    {
        return EventType::CmdDeleteAutoFill;
    }
};

struct CmdSetQuickBet : public GameEvent
{
    uint64_t    correlationId;
    BetName     betName;
    size_t      pivot;
    bool        oddsBet;
    Gen::Money  amount;

    EventType type() const override
    {
        return EventType::CmdSetQuickBet;
    }
};

struct CmdApplyQuickBet : public GameEvent
{
    uint64_t    correlationId;
    size_t      index;

    EventType type() const override
    {
        return EventType::CmdApplyQuickBet;
    }
};

struct CmdDeleteQuickBetByLookup : public GameEvent
{
    uint64_t    correlationId;
    BetName     betName;
    size_t      pivot;
    bool        oddsBet;
    Gen::Money  amount;

    EventType type() const override
    {
        return EventType::CmdDeleteQuickBetByLookup;
    }
};

struct CmdDeleteQuickBetByIndex : public GameEvent
{
    uint64_t    correlationId;
    size_t      index;

    EventType type() const override
    {
        return EventType::CmdDeleteQuickBetByIndex;
    }
};











//----------------------------------------------------------------
//
// View Events
//
//----------------------------------------------------------------

struct ViewErrorDialog : public GameEvent
{
    EventType orgEventType;
    uint64_t correlationId;
    std::string diag;

    EventType type() const override
    {
        return EventType::ViewErrorDialog;
    }
};

struct ViewSuccess : public GameEvent // Generic success for most events
{
    uint64_t correlationId;
    Craps::BetId betId;
    Craps::PlayerId playerId;

    EventType type() const override
    {
        return EventType::ViewSuccess;
    }
};

struct ViewMakeBetSuccess : public GameEvent
{
    uint64_t correlationId;
    Craps::BetId betId;

    EventType type() const override
    {
        return EventType::ViewMakeBetSuccess;
    }
};

struct ViewMakeOddsBetSuccess : public GameEvent
{
    uint64_t correlationId;
    Craps::BetId betId;

    EventType type() const override
    {
        return EventType::ViewMakeOddsBetSuccess;
    }
};

struct ViewMakeBetAutoSuccess : public GameEvent
{
    uint64_t correlationId;
    Craps::BetId betId;

    EventType type() const override
    {
        return EventType::ViewMakeBetAutoSuccess;
    }
};

struct ViewMakeBetAutoError : public GameEvent
{
    EventType orgEventType;
    uint64_t correlationId;
    Craps::PlayerId playerId;
    std::string diag;
    EventType type() const override
    {
        return EventType::ViewMakeBetAutoError;
    }
};

struct ViewRollDiceCountDown : public GameEvent
{
    uint64_t       correlationId;
    Craps::TableId tableId;
    int            numSeconds;

    EventType type() const override
    {
        return EventType::ViewRollDiceCountDown;
    }
};

struct ViewRollDiceAnimation : public GameEvent
{
    uint64_t       correlationId;

    EventType type() const override
    {
        return EventType::ViewRollDiceAnimation;
    }
};

struct ViewProgramExit : public GameEvent
{
    // TODO
    int signal;
    EventType type() const override
    {
        return EventType::ViewProgramExit;
    }
};


struct TimerEvent : public GameEvent
{
    std::chrono::steady_clock::time_point time;
    EventType type() const override
    {
        return EventType::Timer;
    }
};
#endif
