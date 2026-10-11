//----------------------------------------------------------------
//
// File: Consoleview.cpp
//
//----------------------------------------------------------------

#include <cui/ConsoleView.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <cassert>
#include <iostream>
#include <vector>

using namespace Cui;

//----------------------------------------------------------------

void
ConsoleView::run()
{
    std::cout << "Need to implement run()\n";
    std::cout << "Press any key to exit\n";
    
    int choice = 0;
    std::cin >> choice;
}

//----------------------------------------------------------------

void
ConsoleView::displayMessage(const std::string& msg)
{
    std::cout << msg << "\n";
}

//----------------------------------------------------------------

void
ConsoleView::displayAboutCraps()
{
    // TODO
}

//----------------------------------------------------------------

Craps::TableId
ConsoleView::promptUserToSelectTable()
{
    std::cout << "Available Tables:\n";
    auto tableList = Gbl::pTableMgr->getTableList();
    for (size_t i = 0; i < tableList.size(); ++i)
    {
        std::cout << i << ") " << tableList[i].tableName << "\n";
    }
    std::cout << "Select table index: ";
    int choice = 0;
    std::cin >> choice;
    return tableList[choice].tableId;
}

//----------------------------------------------------------------

std::vector<Craps::PlayerId>
ConsoleView::promptUserToSelectPlayers()
{
    std::cout << "Available Players:\n";
    const auto& players = Gbl::pPlayerMgr->getPlayerList();
    for (size_t i = 0; i < players.size(); ++i)
    {
        std::cout << i << ") " << players[i].playerName << "\n";
    }

    std::cout << "Enter comma-separated indices (e.g. 0,2): ";
    std::string input;
    std::cin >> input;

    std::vector<Craps::PlayerId> selected;
    size_t pos = 0;
    while ((pos = input.find(',')) != std::string::npos)
    {
        int idx = std::stoi(input.substr(0, pos));
        if (idx >= 0 && idx < players.size())
            selected.push_back(players[idx].playerId);
        input.erase(0, pos + 1);
    }
    if (!input.empty())
    {
        int idx = std::stoi(input);
        if (idx >= 0 && idx < players.size())
            selected.push_back(players[idx].playerId);
    }
    return selected;
}

//----------------------------------------------------------------
