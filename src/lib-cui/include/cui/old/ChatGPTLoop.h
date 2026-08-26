// GameEvent.h
#pragma once
#include <string>
#include <chrono>
#include <memory>

enum class EventType
{
    UserInputLine,
    UserInputChar,
    Timer,
    Signal
};

struct GameEvent
{
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

using GameEventPtr = std::shared_ptr<GameEvent>;

// GameEventLoop.h
#pragma once
#include "GameEvent.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <boost/asio.hpp>
#include <functional>

class GameEventLoop
{
public:
    GameEventLoop();
    void run();
    void stop();
    void enqueue(GameEventPtr ev);

private:
    void processEvents();

    bool running_ = false;
    std::queue<GameEventPtr> eventQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCv_;
    boost::asio::io_context io_;
    boost::asio::steady_timer timer_;
};

// GameEventLoop.cpp
#include "GameEventLoop.h"
#include <iostream>

GameEventLoop::GameEventLoop()
    : timer_(io_, std::chrono::seconds(5))
{
}

void
GameEventLoop::enqueue(GameEventPtr ev)
{
    std::lock_guard lock(queueMutex_);
    eventQueue_.push(std::move(ev));
    queueCv_.notify_one();
}

void
GameEventLoop::stop()
{
    running_ = false;
    queueCv_.notify_all();
}

void GameEventLoop::run()
{
    running_ = true;

    timer_.async_wait(
        [this](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                enqueue(std::make_shared<TimerEvent>(
                            TimerEvent{std::chrono::steady_clock::now()}));
            }
        }
    );

    std::thread asioThread([this] { io_.run(); });

    processEvents();

    asioThread.join();
}

void
GameEventLoop::processEvents()
{
    while (running_)
    {
        std::unique_lock lock(queueMutex_);
        queueCv_.wait(lock, [this] { return !eventQueue_.empty() || !running_; });

        while (!eventQueue_.empty())
        {
            GameEventPtr ev = std::move(eventQueue_.front());
            eventQueue_.pop();
            lock.unlock();

            switch (ev->type())
            {
                case EventType::UserInputLine:
                {
                    auto e = static_cast<UserInputLineEvent*>(ev.get());
                    std::cout << "[InputLine] " << e->input << '\n';
                    if (e->input == "quit") stop();
                    break;
                }
                case EventType::UserInputChar:
                {
                    auto e = static_cast<UserInputCharEvent*>(ev.get());
                    std::cout << "[InputChar] " << e->input << '\n';
                    if (e->input == 'q') stop();
                    break;
                }
                case EventType::Timer:
                {
                    std::cout << "[Timer] 5s timer fired\n";
                    break;
                }
                case EventType::Signal:
                {
                    auto e = static_cast<SignalEvent*>(ev.get());
                    std::cout << "[Signal] Received signal: " << e->signal << '\n';
                    stop();
                    break;
                }
            }

            lock.lock();
        }
    }
}

// ConsoleView.h
#pragma once
#include "GameEventLoop.h"
#include <boost/asio.hpp>
#include <memory>
#include <variant>

enum class InputMode
{
    Char,
    Line
};

class ConsoleView
{
public:
    ConsoleView(GameEventLoop& loop, boost::asio::io_context& io);
    void start();
    void setInputMode(InputMode mode);

private:
    void asyncReadChar();
    void asyncReadLine();

    GameEventLoop& eventLoop_;
    boost::asio::posix::stream_descriptor input_;
    std::array<char, 1> buffer_;
    std::string lineBuffer_;
    InputMode inputMode_ = InputMode::Char;
    static constexpr unsigned maxLineLen_ = 1024;
};

// ConsoleView.cpp
#include "ConsoleView.h"
#include <iostream>
#include <unistd.h>

ConsoleView::ConsoleView(GameEventLoop& loop,
                         boost::asio::io_context& io)
    : eventLoop_(loop)
    , input_(io, ::dup(STDIN_FILENO))
{
    // Empty
}

ConsoleView::~ConsoleView()
{
    boost::system::error_code ec;
    input_.cancel(ec);
    input_.close(ec);
}    

void
ConsoleView::ConsoleView::start()
{
    setInputMode(InputMode::Char);
}

void
ConsoleView::setInputMode(InputMode mode)
{
    if (inputMode_ == mode) return;
    inputMode_ = mode;

    boost::system::error_code ec;
    assert(input_.is_open());
    input_.cancel(ec);
    if (ec)
    {
        std::cerr << "Error cancelling input: " << ec.message() << '\n';
    }

    if (inputMode_ == InputMode::Line)
    {
        lineBuffer_.clear();
    }

    if (inputMode_ == InputMode::Char)
    {
        asyncReadChar();
    }
    else
    {
        asyncReadLine();
    }
}

void
ConsoleView::asyncReadChar()
{
    input_.async_read_some(boost::asio::buffer(buffer_),
        [this](boost::system::error_code ec, std::size_t length)
        {
            if (ec == boost::asio::error::operation_aborted) return;
            if (ec == boost::asio::error::eof)
            {
                eventLoop_.enqueue(std::make_shared<EOFEvent>());
                return;
            }
            if (!ec && length > 0)
            {
                char c = buffer_[0];
                eventLoop_.enqueue(std::make_shared<UserInputCharEvent>(UserInputCharEvent{c}));
                asyncReadChar();
            }
        }
    );
}

void
ConsoleView::asyncReadLine()
{
    input_.async_read_some(boost::asio::buffer(buffer_),
        [this](boost::system::error_code ec, std::size_t length)
        {
            if (ec == boost::asio::error::operation_aborted) return;
            if (ec == boost::asio::error::eof)
            {
                eventLoop_.enqueue(std::make_shared<EOFEvent>());
                return;
            }
            if (!ec && length > 0)
            {
                char c = buffer_[0];
                if (c == '\r')
                {
                    asyncReadLine();
                    return;
                }
                if (c == '\n')
                {
                    eventLoop_.enqueue(std::make_shared<UserInputLineEvent>(UserInputLineEvent{lineBuffer_}));
                    lineBuffer_.clear();
                }
                else
                {
                    lineBuffer_ += c;
                }
                if (lineBuffer_.size() > maxLineLen_)
                { 
                    eventLoop_.enqueue(std::make_shared<ErrorEvent>("Line too long"));
                    lineBuffer_.clear();
                }
                asyncReadLine();
            }
        }
    );
}
