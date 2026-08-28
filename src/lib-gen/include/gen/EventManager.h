//----------------------------------------------------------------
//
// File: EventManager.h
//
//----------------------------------------------------------------

#pragma once

#include <any>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Gen {

class EventManager
{
public:
    EventManager()
        : worker_(&EventManager::processEvents, this)
    {
    }

    ~EventManager()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }

        condition_.notify_one();

        if (worker_.joinable())
        {
            worker_.join();
        }
    }

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    EventManager(EventManager&&) = delete;
    EventManager& operator=(EventManager&&) = delete;

    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler)
    {
        if (!handler)
        {
            throw std::invalid_argument("Cannot subscribe an empty handler");
        }

        AnyHandler wrapper =
            [handler = std::move(handler)](const std::any& event)
            {
                handler(std::any_cast<const EventType&>(event));
            };

        {
            std::lock_guard<std::mutex> lock(mutex_);
            subscribers_[std::type_index(typeid(EventType))]
                .push_back(std::move(wrapper));
        }
    }

    template<typename EventType>
    void publish(const EventType& event)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stopping_) {
                throw std::runtime_error(
                    "Cannot publish to a stopping EventManager");
            }

            // Store a copy of the event in the queue.
            events_.emplace(event);
        }

        condition_.notify_one();
    }

private:
    using AnyHandler = std::function<void(const std::any&)>;

    using SubscriberMap =
        std::unordered_map<
            std::type_index,
            std::vector<AnyHandler>
        >;

    void processEvents()
    {
        for (;;)
        {
            std::any event;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                condition_.wait(
                    lock,
                    [this]
                    {
                        return stopping_ || !events_.empty();
                    });

                // Process all queued events before exiting. This gives
                // already-published events a chance to be delivered.
                if (stopping_ && events_.empty())
                {
                    return;
                }

                event = std::move(events_.front());
                events_.pop();
            }

            dispatch(event);
        }
    }

    void dispatch(const std::any& event)
    {
        std::vector<AnyHandler> handlers;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            const auto it =
                subscribers_.find(std::type_index(event.type()));

            if (it == subscribers_.end())
            {
                return;
            }

            // Copy the handlers while holding the lock, then invoke them
            // without the lock. This prevents callbacks from blocking or
            // deadlocking subscribe(), publish(), or destruction.
            handlers = it->second;
        }

        for (const auto& handler : handlers)
        {
            try
            {
                handler(event);
            }
            catch (...)
            {
                // A handler exception must not terminate the worker thread.
                //
                // Replace this with logging or an application-specific error
                // policy if required.
            }
        }
    }

private:
    SubscriberMap subscribers_;
    std::queue<std::any> events_;

    mutable std::mutex mutex_;
    std::condition_variable condition_;

    bool stopping_ = false;
    std::thread worker_;
};

/*-----------------------------------------------------------*//**

@class EventManager

@brief Provides an asynchronous thread safe Pub/Sub Event Bus

Important behaviors:

@li publish() copies the event into a thread-safe queue and 
    returns without running handlers.
@li A private worker thread removes events and dispatches them.
@li The subscriber map and event queue are protected by the same mutex.
@li Handlers are copied while locked but invoked after unlocking.
@li Events already queued are dispatched before the destructor finishes.
@li This version has no unsubscribe mechanism; subscriptions remain
    active for the lifetime of the manager.
@li Handler callbacks execute serially on the single worker thread.
    If one callback blocks, later events wait.

1. Example: Unnamed subscriber callback function

@@code
struct DiceRolled
{
    int value;
};

Craps::EventManager events;

events.subscribe<DiceRolled>(
    [](const DiceRolled& event)
    {
        // This runs on EventManager's worker thread.
        std::cout << "Rolled: " << event.value << '\n';
    });

events.publish(DiceRolled{6});
@endcode

2. Example: Named subscriber callback function

There are times you want to subscribe and dispatch to a named
function within the source file. 

Define a named function whose parameter matches the event type
Then subscribe it using its address

@code
void handleDiceRolled(const DiceRolled& event)
{
    std::cout << "Rolled: " << event.value << '\n';
}

events.subscribe<DiceRolled>(&handleDiceRolled);
@endcode

3. Example: Using std::function()

A function pointer converts to the std::function<void(const
DiceRolled&)> parameter automatically. So You can also write the
equivalent with an explicit std::function: cpp

@code
events.subscribe<DiceRolled>(
    std::function<void(const DiceRolled&)>{handleDiceRolled});
@endcode


4. Example: For a function declared in another source file

Place its declaration in a header:

@code
// DiceHandlers.h
#pragma once

void handleDiceRolled(const DiceRolled& event);
@endcode

Define it in a .cpp file:

@code
// DiceHandlers.cpp
#include "DiceHandlers.h"
#include <iostream>

void handleDiceRolled(const DiceRolled& event)
{
    std::cout << "Rolled: " << event.value << '\n';
}
@endcode

Then subscribe normally:

@code
#include "DiceHandlers.h"

Gen::EventManager events;

events.subscribe<DiceRolled>(&handleDiceRolled);
@endcode

5. Example: For a static member function:

@code
class DiceHandlers
{
public:
    static void onDiceRolled(const DiceRolled& event);
};

void DiceHandlers::onDiceRolled(const DiceRolled& event)
{
    std::cout << "Rolled: " << event.value << '\n';
}
@endcode

Subscribe to it like this:

@code
events.subscribe<DiceRolled>(&DiceHandlers::onDiceRolled);
@endcode

5. Example: For a non-static member function

Use a lambda that captures the object:

@code
class Game
{
public:
    void onDiceRolled(const DiceRolled& event)
    {
        std::cout << "Game received: " << event.value << '\n';
    }
};

Game game;

events.subscribe<DiceRolled>(
    [&game](const DiceRolled& event)
    {
        game.onDiceRolled(event);
    });
@endcode

Because EventManager stores the handler until it is destroyed, game
must remain alive for as long as the subscription can be invoked. A
safer option is to capture a std::weak_ptr when the receiving object is
managed by std::shared_ptr:

@code
class Game
{
public:
    void onDiceRolled(const DiceRolled& event)
    {
        std::cout << "Game received: " << event.value << '\n';
    }
};

auto game = std::make_shared<Game>();
std::weak_ptr<Game> weakGame = game;

events.subscribe<DiceRolled>(
    [weakGame](const DiceRolled& event)
    {
        if (auto game = weakGame.lock())
        {
            game->onDiceRolled(event);
        }
    });
@endcode

*/

} // namespace Gen

//----------------------------------------------------------------
