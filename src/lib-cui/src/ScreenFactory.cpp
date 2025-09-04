//----------------------------------------------------------------
//
// File: ScreenFactory.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenFactory.h>
#include <cui/ScreenCrapsTable.h>

using namespace Cui;

//----------------------------------------------------------------

Screen* 
ScreenFactory::createSrceen(ConsoleView::ScreenId id, ConsoleView& v)
{
    Screen* s;
    
    switch(id)
    {
    case ConsoleView::ScreenId::ScreenCrapsTable:  s = newScreenCrapsTable(v);  break;
//  case ConsoleView::ScreenPlayerStats: s = newScreenPlayerStats(v); break;
    }
    return s;
}

//----------------------------------------------------------------

Screen*
ScreenFactory::newScreenCrapsTable(ConsoleView& view)
{
    return new ScreenCrapsTable(view);
}

//----------------------------------------------------------------

