//----------------------------------------------------------------
//
// File: TimerManagerTest.cpp
//
//----------------------------------------------------------------

#include "gen/TimerManager.h"

#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <doctest/doctest.h>

using namespace Gen;

//----------------------------------------------------------------

#include <condition_variable>

TEST_CASE("TimerManager")
{
    SUBCASE("One-shot timer fires after specified delay")
    {
        TimerManager manager;
        std::mutex mtx;
        std::condition_variable cv;
        bool fired = false;

        auto id = manager.createTimer(
            [&]
            {
                std::lock_guard<std::mutex> lock(mtx);
                fired = true;
                cv.notify_one();
            },
            std::chrono::milliseconds(100)
        );

        {
            std::unique_lock<std::mutex> lock(mtx);
            // Wait up to 2 seconds for timer to fire
            cv.wait_for(lock, std::chrono::seconds(2), [&] { return fired; });
        }
        CHECK(fired == true);
    }

    SUBCASE("Unarmed timer does not fire")
    {
        TimerManager manager;
        std::atomic<bool> fired = false;

        auto id = manager.createTimer(
            [&]
            {
                fired = true;
            }
        ); // Not armed

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        CHECK(fired == false);
    }

    SUBCASE("Manually armed timer fires correctly")
    {
        TimerManager manager;
        std::atomic<bool> fired = false;

        auto id = manager.createTimer([&]
        {
            fired = true;
        });

        manager.armTimer(id, std::chrono::milliseconds(100));

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        CHECK(fired == true);
    }

    SUBCASE("Repeat timer fires multiple times")
    {
        TimerManager manager;
        std::atomic<int> count = 0;

        auto id = manager.createTimer([&]
        {
            ++count;
        }, std::chrono::milliseconds(100), true); // armed immediately, repeat

        std::this_thread::sleep_for(std::chrono::milliseconds(350));
        CHECK(count >= 3);
    }

    SUBCASE("Cancel prevents timer callback")
    {
        TimerManager manager;
        std::atomic<bool> fired = false;

        auto id = manager.createTimer([&]
        {
            fired = true;
        }, std::chrono::milliseconds(100));

        manager.cancelTimer(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        CHECK(fired == false);
    }

    SUBCASE("Re-arming a timer after initial creation")
    {
        TimerManager manager;
        std::atomic<bool> fired = false;

        // Create an unarmed timer
        auto id = manager.createTimer([&]
        {
            fired = true;
        });

        // Wait a bit to confirm nothing fires yet
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        CHECK(fired == false);

        // Now arm the timer
        manager.armTimer(id, std::chrono::milliseconds(100));

        // Wait for it to fire
        for (int i = 0; i < 10 && !fired; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        CHECK(fired == true);
    }
}

//----------------------------------------------------------------
