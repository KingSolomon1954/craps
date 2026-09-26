//----------------------------------------------------------------
//
// File: GameEvents.h
//
//----------------------------------------------------------------

#pragma once
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <atomic>
#include <string>
#include <chrono>
#include <memory>

namespace Ctrl {

enum class EventSource
{
    Unset,
    Model,
    View,
    Controller
};

enum class EventType
{
    Unset,
    
    // Requests
    ReqRollDice,               // not yet implemented
    ReqMakeBet,                // not yet implemented
    ReqRemoveBet,              // not yet implemented
    ReqChangeBet,              // not yet implemented

    // Responses
    RspMakeBet,                // not yet implemented
    RspRemoveBet,              // not yet implemented
    RspChangeBet,              // not yet implemented

    // Unsolicited
    UslBettingClosed,
    UslBettingOpened,
    UslDiceThrowStart,
    UslDiceNewValue,
    UslResolveBetsStart,
    UslResolveBetsEnd,
    UslPointEstablished,
    UslSevenOut,
    UslPassLineWinner,
    UslNewShooter,

    UslBetMade,                // not yet implemented
    UslBetRemoved,             // not yet implemented Player decided to remove a bet
    UslBetChanged,             // not yet implemented Player decided to change a bet
    UslTableBalanceChanged,    // not yet implemented
    UslPlayerBalanceChanged,   // not yet implemented
    
    UslPlayerJoinedTable,
    UslPlayerLeftTable,
    UslCountdownRollDice       // not yet implemented
};


//    UslTableResults,
//    UslTableNumBetsOnTableChanged,
//    UslPlayerResults,
//    UslPlayerNumBetsOnTableChanged,
    
//----------------------------------------------------------------

using EventId       = std::uint64_t;
using CorrelationId = std::uint64_t;

EventId getNextEventId();
CorrelationId getNextCorrelationId();
    
//----------------------------------------------------------------

struct GameEvent
{
    EventSource   source        = EventSource::Unset;
    EventType     type          = EventType::Unset;
    EventId       id            = 0;
    CorrelationId correlationId = 0;

    GameEvent(EventSource source, EventType type)
        : source(source),
          type(type),
          id(getNextEventId()),
          correlationId(getNextCorrelationId())
    {
        // All events get a value for unique ID and a correlationId. In
        // the case of response events (or other related events), the
        // caller is responsible for overwriting the correlationId with
        // the source event's correlationId.
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

struct ReqRollDice : public GameEvent
{
    ReqRollDice()
        : GameEvent{EventSource::View, EventType::ReqRollDice}
    {}
};

//----------------------------------------------------------------

    




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
    
