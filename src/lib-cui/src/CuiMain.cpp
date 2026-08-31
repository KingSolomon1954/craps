//----------------------------------------------------------------
//
// File: CuiMain.cpp
//
//----------------------------------------------------------------

#include <cui/CuiMain.h>
#include <cui/ConsoleManager.h>
#include <cui/screens/ScreenCrapsTable.h>

using namespace Cui;

//----------------------------------------------------------------

void
CuiMain::init()
{
    auto& mgr = ConsoleManager::instance();
    mgr.init(); // ncurses init, 
    mgr.setSurface(&ScreenCrapsTable::instance());
}

//----------------------------------------------------------------
