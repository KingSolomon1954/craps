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
    UserInputLine,
    UserInputChar,
    Timer,
    Signal
};

struct GameEvent
{
    using GameEventPtr = std::shared_ptr<GameEvent>;
    
    virtual ~GameEvent() = default;
    virtual EventType type() const = 0;
};

struct UserInputLineEvent : public GameEvent
{
    std::string input;
    EventType type() const override
    {
        return EventType::UserInputLine;
    }
};

struct UserInputCharEvent : public GameEvent
{
    char input;
    EventType type() const override
    {
        return EventType::UserInputChar;
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

struct SignalEvent : public GameEvent
{
    int signal;
    EventType type() const override
    {
        return EventType::Signal;
    }
};

} // namespace Ctrl

//----------------------------------------------------------------
