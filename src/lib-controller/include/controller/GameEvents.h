//----------------------------------------------------------------
//
// File: GameEvents.h
//
//----------------------------------------------------------------

#pragma once
#include <string>
#include <chrono>
#include <memory>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>

namespace Ctrl {

using EventId = unsigned;
using CorrelationId = unsigned;
    
enum class EventSource
{
    Unset,
    Model,
    Controller,
    View
};

enum class EventType
{
    Unset,
    
    // Requests
    ReqMakeBet,
    ReqRemoveBet,
    ReqRollDice,

    // Responses
    RspMakeBetAccepted,
    RspMakeBetRejected,

    // Unsolicited
    UslBettingClosed,
    UslBettingOpened,
    UslDiceThrowStart,
    UslDiceThrowEnd,
    UslDiceRolled,
    UslResolveBetsStart,
    UslResolveBetsEnd,
    UslPointEstablished,
    UslSevenOut,
    UslPassLineWinner,
    UslNewShooter,
    UslPlayerJoinedTable,
    UslPlayerLeftTable,

    UslBetChanged,
    UslPlayerBalanceChanged,
    UslBuddyBetMade,
    UslCountdownRollDice,
    UslHouseLowOnFunds,
};

//----------------------------------------------------------------

struct GameEvent
{
    EventId       id            = 0;
    CorrelationId correlationId = 0;
    EventSource   source        = EventSource::Unset;
    EventType     type          = EventType::Unset;

    GameEvent(EventSource source, EventType type)
        : source(source), type(type)
    {
    }
};

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
    UslDiceThrowStart()
        : GameEvent{EventSource::Model, EventType::UslDiceThrowStart}
    {}
};

//----------------------------------------------------------------

struct UslDiceThrowEnd : public GameEvent
{
    UslDiceThrowEnd()
        : GameEvent{EventSource::Model, EventType::UslDiceThrowEnd}
    {}
};

//----------------------------------------------------------------

struct UslDiceRolled : public GameEvent
{
    unsigned roll = 0;
    unsigned d1 = 0;
    unsigned d2 = 0;
    
    UslDiceRolled()
        : GameEvent{EventSource::Model, EventType::UslDiceRolled}
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
    UslNewShooter()
        : GameEvent{EventSource::Model, EventType::UslNewShooter}
    {}
};

//----------------------------------------------------------------

struct UslPlayerJoinedTable : public GameEvent
{
    Gen::Uuid playerId;
    
    UslPlayerJoinedTable()
        : GameEvent{EventSource::Model, EventType::UslPlayerJoinedTable}
    {}
};

//----------------------------------------------------------------

struct UslPlayerLeftTable : public GameEvent
{
    Gen::Uuid playerId;
    
    UslPlayerLeftTable()
        : GameEvent{EventSource::Model, EventType::UslPlayerLeftTable}
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
    
