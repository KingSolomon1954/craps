//----------------------------------------------------------------
//
// File: ScreenFactory.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Screen.h>
#include <cui/ConsoleView.h>

namespace Cui {

class ScreenFactory
{
public:
    static Screen* createSrceen(ConsoleView::ScreenId screenId, ConsoleView& view);

private:
    static Screen* newScreenCrapsTable(ConsoleView& view);
};

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------



