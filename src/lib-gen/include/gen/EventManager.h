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

@example

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

@endexample
    
*/

} // namespace Gen

//----------------------------------------------------------------
