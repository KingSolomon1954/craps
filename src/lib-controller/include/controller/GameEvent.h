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
    CmdMakeBet,
    CmdMakeBetAuto,
    CmdMakeOddsBet,
    CmdMakeOddsBetAuto,
    CmdRollDice,
    
    ViewErrorDialog,
    ViewMakeBetSuccess,
    ViewMakeBetAutoSuccess,
    ViewMakeBetAutoError,
    ViewMakeOddsBetSuccess,
    ViewRollDiceCountDown,
    ViewProgramExit
};

struct GameEvent
{
    using GameEventPtr = std::shared_ptr<GameEvent>;
    
    virtual ~GameEvent() = default;
    virtual EventType type() const = 0;
};

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

struct CmdRollDice : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::CmdRollDice;
    }
};

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
    // TODO
    char input;
    EventType type() const override
    {
        return EventType::ViewRollDiceCountDown;
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
