//----------------------------------------------------------------
//
// File: CuiReceiver.cpp
//
//----------------------------------------------------------------

#include <cui/CuiReceiver.h>
#include <craps/CrapsTypes.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

CuiReceiver::CuiReceiver()
{
    // TODO subscribe ...
}

//----------------------------------------------------------------

CuiReceiver&
CuiReceiver::instance()
{
    static CuiReceiver cr;
    return cr;
}

//----------------------------------------------------------------
