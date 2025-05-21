//----------------------------------------------------------------
//
// File: Consoleview.cpp
//
//----------------------------------------------------------------

#include <cui/ConsoleView.h>
#include <cassert>
#include <iostream>
#include <controller/GameControllerIntfc.h>

using namespace Cui;

//----------------------------------------------------------------

void
ConsoleView::setGameController(Ctrl::GameControllerIntfc* pCtrlIntfc)

{
    pCtrlIntfc_ = pCtrlIntfc;
}

//----------------------------------------------------------------

void
ConsoleView::run()
{
    std::cout << "Need to implement run()\n";
}

//----------------------------------------------------------------

void
ConsoleView::displayMessage()
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::displayAboutCraps()
{
    // TODO
}

//----------------------------------------------------------------
