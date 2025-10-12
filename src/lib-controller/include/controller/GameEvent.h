//----------------------------------------------------------------
//
// File: GameEvent.h
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

struct GameEvent
{
    using GameEventPtr = std::unique_ptr<GameEvent>;
    
    virtual ~GameEvent() = default;
    virtual EventType type() const = 0;
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



    
#if 0    
struct TimerEvent : public GameEvent
{
    std::chrono::steady_clock::time_point time;
    EventType type() const override
    {
        return EventType::Timer;
    }
};
#endif
    
} // namespace Ctrl

//----------------------------------------------------------------
