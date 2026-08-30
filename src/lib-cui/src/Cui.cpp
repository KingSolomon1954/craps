//----------------------------------------------------------------
//
// File: Cui.cpp
//
//----------------------------------------------------------------

#include <cui/Cui.h>
#include <cui/ConsoleManager.h>
#include <cui/ScreenCrapsTable.h>

//----------------------------------------------------------------

void
Cui::init()
{
    auto mgr = ConsoleManager::instance();
    mgr.init(); // ncurses init, 
    mgr.setSurface(&ScreenCrapsTable::instance());
}

//----------------------------------------------------------------

void
Cui::run()
{
    ConsoleManager::instance().run();
}

//----------------------------------------------------------------
