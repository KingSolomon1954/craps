//----------------------------------------------------------------
//
// File: CuiSender.cpp
//
//----------------------------------------------------------------

#include <cui/CuiSender.h>
#include <craps/CrapsTypes.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

CuiSender::CuiSender()
{
    // TODO
}

//----------------------------------------------------------------

CuiSender&
CuiSender::instance()
{
    static CuiSender cs;
    return cs;
}

//----------------------------------------------------------------
