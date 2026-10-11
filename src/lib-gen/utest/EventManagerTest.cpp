//----------------------------------------------------------------
//
// File: EventManagerTest.cpp
//
//----------------------------------------------------------------

#include "gen/EventManager.h"

#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

#include <doctest/doctest.h>

using namespace Gen;

namespace {

struct DiceRolled
{
    int value;
};

struct BetPlaced
{
    int amount;
};

struct EmptyEvent
{
};

} // namespace

//----------------------------------------------------------------

TEST_CASE("EventManager")
{
    SUBCASE("Published event is delivered to a subscriber")
    {
        EventManager manager;

        std::mutex mutex;
        std::condition_variable condition;
        bool received = false;
        int receivedValue = 0;

        manager.subscribe<DiceRolled>(
            [&](const DiceRolled& event)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    receivedValue = event.value;
                    received = true;
                }

                condition.notify_one();
            });

        manager.publish(DiceRolled{7});

        {
            std::unique_lock<std::mutex> lock(mutex);

            const bool completed =
                condition.wait_for(
                    lock,
                    std::chrono::seconds(2),
                    [&]
                    {
                        return received;
                    });

            CHECK(completed);
        }

        CHECK(received);
        CHECK(receivedValue == 7);
    }

    SUBCASE("Multiple subscribers receive the same event")
    {
        EventManager manager;

        std::mutex mutex;
        std::condition_variable condition;
        int callbackCount = 0;

        auto callback = [&]
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                ++callbackCount;
            }

            condition.notify_one();
        };

        manager.subscribe<DiceRolled>(
            [&](const DiceRolled&)
            {
                callback();
            });

        manager.subscribe<DiceRolled>(
            [&](const DiceRolled&)
            {
                callback();
            });

        manager.publish(DiceRolled{8});

        {
            std::unique_lock<std::mutex> lock(mutex);

            const bool completed =
                condition.wait_for(
                    lock,
                    std::chrono::seconds(2),
                    [&]
                    {
                        return callbackCount == 2;
                    });

            CHECK(completed);
        }

        CHECK(callbackCount == 2);
    }

    SUBCASE("Events with no subscribers are ignored")
    {
        EventManager manager;

        // No subscriber exists for EmptyEvent. This should not throw
        // or block indefinitely.
        CHECK_NOTHROW(manager.publish(EmptyEvent{}));
    }

    SUBCASE("Subscribers only receive events of the matching type")
    {
        EventManager manager;

        std::mutex mutex;
        std::condition_variable condition;
        bool diceReceived = false;
        bool betReceived = false;

        manager.subscribe<DiceRolled>(
            [&](const DiceRolled&)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    diceReceived = true;
                }

                condition.notify_one();
            });

        manager.subscribe<BetPlaced>(
            [&](const BetPlaced&)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    betReceived = true;
                }

                condition.notify_one();
            });

        manager.publish(DiceRolled{5});

        {
            std::unique_lock<std::mutex> lock(mutex);

            const bool completed =
                condition.wait_for(
                    lock,
                    std::chrono::seconds(2),
                    [&]
                    {
                        return diceReceived;
                    });

            CHECK(completed);
        }

        CHECK(diceReceived);
        CHECK_FALSE(betReceived);
    }

    SUBCASE("Events are delivered in publish order")
    {
        EventManager manager;

        std::mutex mutex;
        std::condition_variable condition;
        std::vector<int> receivedValues;

        manager.subscribe<DiceRolled>(
            [&](const DiceRolled& event)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    receivedValues.push_back(event.value);
                }

                condition.notify_one();
            });

        manager.publish(DiceRolled{1});
        manager.publish(DiceRolled{2});
        manager.publish(DiceRolled{3});

        {
            std::unique_lock<std::mutex> lock(mutex);

            const bool completed =
                condition.wait_for(
                    lock,
                    std::chrono::seconds(2),
                    [&]
                    {
                        return receivedValues.size() == 3;
                    });

            CHECK(completed);
        }

        REQUIRE(receivedValues.size() == 3);
        CHECK(receivedValues[0] == 1);
        CHECK(receivedValues[1] == 2);
        CHECK(receivedValues[2] == 3);
    }

    SUBCASE("An empty handler is rejected")
    {
        EventManager manager;

        std::function<void(const DiceRolled&)> emptyHandler;

        CHECK_THROWS_AS(
            manager.subscribe<DiceRolled>(emptyHandler),
            std::invalid_argument);
    }

    SUBCASE("A handler exception does not terminate the event thread")
    {
        EventManager manager;

        std::mutex mutex;
        std::condition_variable condition;
        bool secondHandlerCalled = false;

        manager.subscribe<DiceRolled>(
            [](const DiceRolled&)
            {
                throw std::runtime_error("Handler failure");
            });

        manager.subscribe<DiceRolled>(
            [&](const DiceRolled&)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    secondHandlerCalled = true;
                }

                condition.notify_one();
            });

        manager.publish(DiceRolled{4});

        {
            std::unique_lock<std::mutex> lock(mutex);

            const bool completed =
                condition.wait_for(
                    lock,
                    std::chrono::seconds(2),
                    [&]
                    {
                        return secondHandlerCalled;
                    });

            CHECK(completed);
        }

        CHECK(secondHandlerCalled);
    }

    SUBCASE("The event is copied when published")
    {
        EventManager manager;

        std::mutex mutex;
        std::condition_variable condition;
        std::string receivedText;

        manager.subscribe<BetPlaced>(
            [&](const BetPlaced& event)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    receivedText = std::to_string(event.amount);
                }

                condition.notify_one();
            });

        BetPlaced event{100};
        manager.publish(event);

        // Changing the original after publishing should not affect the
        // event already stored in the queue.
        event.amount = 999;

        {
            std::unique_lock<std::mutex> lock(mutex);

            const bool completed =
                condition.wait_for(
                    lock,
                    std::chrono::seconds(2),
                    [&]
                    {
                        return !receivedText.empty();
                    });

            CHECK(completed);
        }

        CHECK(receivedText == "100");
    }
}

//----------------------------------------------------------------
