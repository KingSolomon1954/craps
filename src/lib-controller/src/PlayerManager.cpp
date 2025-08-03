//----------------------------------------------------------------
//
// File: PlayerManager.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerManager.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <controller/Globals.h>
#include <craps/DecisionRecord.h>
#include <craps/CrapsTable.h>
#include <gen/ErrorPass.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
PlayerManager::PlayerManager()
{
    // Empty.
}

/*-----------------------------------------------------------*//**

Create a Player

*/
Craps::PlayerPtr
PlayerManager::createPlayer(const std::string& name)
{
    // TODO
    Craps::PlayerConfig config;
    
    auto player = std::make_shared<Craps::Player>(
        "1", config, *Gbl::pEventMgr);
    players_[player->getPlayerId()] = player;
    return player;
}

/*-----------------------------------------------------------*//**

Get Player by UUID

*/
Craps::PlayerPtr
PlayerManager::getPlayer(const Craps::PlayerId& playerId) const
{
    auto it = players_.find(playerId);
    if (it != players_.end()) return it->second;
    return nullptr;
}

/*-----------------------------------------------------------*//**

Loads player descriptions from file.

*/
PlayerManager::PlayerDescriptions
PlayerManager::loadPlayerChoices()
{
    // TODO: read directory and build up player choices
    return {
        { "John", "The Bronx better",  "abcd", std::chrono::system_clock::now()},
        { "Jane", "The Chatty roller", "efgh", std::chrono::system_clock::now()},
        { "Dave", "The quiet gambler", "ijkl", std::chrono::system_clock::now()},
    };
}

//----------------------------------------------------------------

void
PlayerManager::loadStartingPlayers()
{
    assert(Gbl::pTable);
    
    // TODO read from mult layer config
    // Form list of the last player ids
    std::vector<Craps::PlayerId> ids;
    
    // Add each player to table
    Gen::ErrorPass ep;
    for (auto pid : ids)  // Players join table
    {
        // TODO: check error return
        // Gbl::pTable->addPlayer(pid, ep);
    }
}

//----------------------------------------------------------------

Craps::Player
PlayerManager::loadPlayer(const Craps::PlayerId& playerId)
{
    // TODO: read from file and create player

    Craps::PlayerConfig config;
    return Craps::Player("1", config, *Gbl::pEventMgr);
}

/*-----------------------------------------------------------*//**

Load all players from file

*/
bool
PlayerManager::loadPlayers()
{
#if 0    
    /// TODO switch to a YAML index, or read directory freshly each time
    std::ifstream in("players/index.json");
    if (!in) return false;
    json index;
    in >> index;

    for (const auto& entry : index["players"])
    {
        auto player = std::make_shared<Craps::Player>();
        if (player->loadFromFile("players/" + entry["uuid"].get<Gen::Uuid>() + ".json"))
        {
            players_[player->getUuid()] = player;
        }
    }
#endif    
    return true;
}

/*-----------------------------------------------------------*//**

Save all players to file

*/
bool
PlayerManager::savePlayers()
{
#if 0    
    json index;
    for (const auto& [uuid, player] : players_)
    {
        player->saveToFile("players/" + uuid + ".json");
        index["players"].push_back({
            {"uuid", uuid},
            {"name", player->getName()}
        });
    }

    std::ofstream out("players/index.json");
    out << index.dump(2);
#endif    
    return true;
}

//----------------------------------------------------------------
