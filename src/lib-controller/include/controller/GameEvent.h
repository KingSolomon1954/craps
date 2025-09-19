//----------------------------------------------------------------
//
// File: GameEvent.h
//
//----------------------------------------------------------------

#pragma once
#include <string>
#include <chrono>
#include <memory>

namespace Ctrl {

enum class EventType
{
    UserMakeBet,
    UserMakeOddsBet,
    UserRollDice,
    
    ViewErrorDialog,
    ViewMakeBetSuccess,
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

struct UserMakeBet : public GameEvent
{
    std::string input;
    EventType type() const override
    {
        return EventType::UserMakeBet;
    }
};

struct UserRollDice : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::UserRollDice;
    }
};

struct ViewErrorDialog : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::ViewErrorDialog;
    }
};

struct ViewMakeBetSuccess : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::ViewMakeBetSuccess;
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
