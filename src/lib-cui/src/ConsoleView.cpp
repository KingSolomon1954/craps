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

Ctrl::TableManager::TableId
ConsoleView::promptUserToSelectTable(const Ctrl::TableManager::TableDescriptions& tds)
{
    // TODO
    return 0;
}

//----------------------------------------------------------------

std::vector<Ctrl::PlayerManager::PlayerId>
ConsoleView::promptUserToSelectPlayers(
        const Ctrl::PlayerManager::PlayerDescriptions& pds)
{
    // TODO
    (void) pds;
    return {
        {"abcd"},
        {"efgh"},
        {"ijkl"},
    };
}

//----------------------------------------------------------------
