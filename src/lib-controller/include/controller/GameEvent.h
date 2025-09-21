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
    PlayerMakeBet,
    AutomationMakeBet,
    PlayerMakeOddsBet,
    PlayerRollDice,
    
    ViewErrorDialog,
    ViewMakeBetSuccess,
    ViewAutomationMakeBetSuccess,
    ViewAutomationMakeBetError,
    ViewMakeOddsBetSuccess,
    ViewRollDiceCountDown,
    
    SignalProgramExit
};

struct GameEvent
{
    using GameEventPtr = std::shared_ptr<GameEvent>;
    
    virtual ~GameEvent() = default;
    virtual EventType type() const = 0;
};

struct PlayerMakeBet : public GameEvent
{
    uint64_t        correlationId;
    Craps::PlayerId playerId;
    BetName         betName;
    Gen::Money      contractAmount;
    size_t          pivot;
    
    EventType type() const override
    {
        return EventType::PlayerMakeBet;
    }
};

struct AutomationMakeBet : public GameEvent
{
    uint64_t        correlationId;
    Craps::PlayerId playerId;
    BetName         betName;
    Gen::Money      contractAmount;
    size_t          pivot;
    
    EventType type() const override
    {
        return EventType::AutomationMakeBet;
    }
};

struct PlayerRollDice : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::PlayerRollDice;
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

struct ViewAutomationMakeBetError : public GameEvent
{
    EventType orgEventType;
    uint64_t correlationId;
    Craps::PlayerId playerId;
    std::string diag;
    EventType type() const override
    {
        return EventType::ViewAutomationMakeBetError;
    }
};

struct ViewMakeBetSuccess : public GameEvent
{
    EventType orgEventType;
    uint64_t correlationId;

    EventType type() const override
    {
        return EventType::ViewMakeBetSuccess;
    }
};

struct ViewAutomationMakeBetSuccess : public GameEvent
{
    EventType orgEventType;
    uint64_t correlationId;
    std::string diag;

    EventType type() const override
    {
        return EventType::ViewAutomationMakeBetSuccess;
    }
};

struct ViewMakeOddsBetSuccess : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::ViewMakeOddsBetSuccess;
    }
};

struct ViewRollDiceCountDown : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::ViewRollDiceCountDown;
    }
};

struct SignalProgramExit : public GameEvent
{
    int signal;
    EventType type() const override
    {
        return EventType::SignalProgramExit;
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
