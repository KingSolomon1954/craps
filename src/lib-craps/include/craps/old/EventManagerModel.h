//----------------------------------------------------------------
//
// File: EventManagerModel.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/EventManager.h>

namespace Craps {

class EventManagerModel
{
public:
    static Gen::EventManager& instance();

private:
    EventManagerModel() = delete;
};

} // namespace Craps

//----------------------------------------------------------------
