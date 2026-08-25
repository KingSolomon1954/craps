//----------------------------------------------------------------
//
// File: EventManagerModel.cpp
//
//----------------------------------------------------------------

#include <craps/EventManagerModel.h>

using namespace Craps;

//----------------------------------------------------------------

Gen::EventManager&
EventManagerModel::instance()
{
    static Gen::EventManager mgr;
    return mgr;
}

//----------------------------------------------------------------

