//----------------------------------------------------------------
//
// File: GameEvents.cpp
//
//----------------------------------------------------------------

#include <controller/GameEvents.h>

using namespace Ctrl;

static std::atomic<EventId> nextEventId{0};
static std::atomic<CorrelationId> nextCorrelationId{0};

//----------------------------------------------------------------

Ctrl::EventId
Ctrl::getNextEventId()
{
    return nextEventId.fetch_add(1, std::memory_order_relaxed);
}

//----------------------------------------------------------------

Ctrl::CorrelationId
Ctrl::getNextCorrelationId()
{
    return nextCorrelationId.fetch_add(1, std::memory_order_relaxed);
}

//----------------------------------------------------------------
