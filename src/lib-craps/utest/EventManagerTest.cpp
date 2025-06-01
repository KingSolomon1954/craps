//----------------------------------------------------------------
//
// File: EventManagerTest.cpp
//
//----------------------------------------------------------------

#include "EventManager.h"
#include <iostream>
#include <doctest/doctest.h>
#include "Events.h"

using namespace App;

//----------------------------------------------------------------

TEST_CASE("EventManager")
{
    class SubscriberFuncs
    {
    public:
        void registerFuncs(EventManager& em)
        {
            em.subscribe<BettingClosed>(
                [this](const BettingClosed&)
                {
                    // std::cout << "Betting is now closed.\n";
                    this->count_++;
                }
            );
            em.subscribe<BettingOpened>(
                [this](const BettingOpened&)
                {
                    // std::cout << "Betting is now open.\n";
                    this->count_++;
                }
            );
            em.subscribe<PointEstablished>(
                [this](const PointEstablished& evt)
                {
                    // std::cout << "Point is " << evt.point << std::endl;
                    CHECK(evt.point == 4);
                    this->count_++;
                }
            );
        }
    public:
        unsigned count_ = 0;
    };

    EventManager em;
    SubscriberFuncs subs;
    subs.registerFuncs(em);
    em.publish(BettingClosed{});
    em.publish(BettingOpened{});
    em.publish(PointEstablished{4});
    CHECK(subs.count_ == 3);
}

//----------------------------------------------------------------
