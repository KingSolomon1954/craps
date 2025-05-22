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

Ctrl::TableManager::TableId
ConsoleView::promptUserToSelectTable(
    const Ctrl::TableManager::TableDescriptions& tds)
{
    std::cout << "Available Tables:\n";
    for (size_t i = 0; i < tds.size(); ++i)
    {
        std::cout << i << ") " << tds[i].name << "\n";
    }
    std::cout << "Select table index: ";
    int choice = 0;
    std::cin >> choice;
    return tds[choice].tableId;
}

//----------------------------------------------------------------

std::vector<Ctrl::PlayerManager::PlayerId>
ConsoleView::promptUserToSelectPlayers(
    const Ctrl::PlayerManager::PlayerDescriptions& pds)
{
    std::cout << "Available Players:\n";
    for (size_t i = 0; i < pds.size(); ++i)
    {
        std::cout << i << ") " << pds[i].name << "\n";
    }

    std::cout << "Enter comma-separated indices (e.g. 0,2): ";
    std::string input;
    std::cin >> input;

    std::vector<Ctrl::PlayerManager::PlayerId> selected;
    size_t pos = 0;
    while ((pos = input.find(',')) != std::string::npos)
    {
        int idx = std::stoi(input.substr(0, pos));
        if (idx >= 0 && idx < pds.size())
            selected.push_back(pds[idx].playerId);
        input.erase(0, pos + 1);
    }
    if (!input.empty())
    {
        int idx = std::stoi(input);
        if (idx >= 0 && idx < pds.size())
            selected.push_back(pds[idx].playerId);
    }
    return selected;
}

//----------------------------------------------------------------
