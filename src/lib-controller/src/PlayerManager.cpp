//----------------------------------------------------------------
//
// File: PlayerManager.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerManager.h>
#include <cassert>
#include <fstream>
#include <iostream>
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
PlayerManager::PlayerPtr
PlayerManager::createPlayer(const std::string& name)
{
    auto player = std::make_shared<Craps::Player>(name, 1000); // Default balance
    playersAll_[player->getUuid()] = player;
    return player;
}

/*-----------------------------------------------------------*//**

Get Player by UUID

*/
PlayerManager::PlayerPtr
PlayerManager::getPlayer(const Gen::Uuid& uuid) const
{
    auto it = playersAll_.find(uuid);
    if (it != playersAll_.end()) return it->second;
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
    std::vector<PlayerManager::PlayerId> ids;
    
    // Add each player to table
    Gen::ErrorPass ep;
    for (auto pid : ids)  // Players join table
    {
        // TODO: check error return
        Gbl::pTable->addPlayer(pid, ep);
    }
}

//----------------------------------------------------------------

Craps::Player
PlayerManager::loadPlayer(const PlayerId& playerId)
{
    // TODO: read from file and create player
    return Craps::Player();
}

/*-----------------------------------------------------------*//**

Load all players from file

*/
bool
PlayerManager::loadPlayers()
{
    std::ifstream in("players/index.json");
    if (!in) return false;
    json index;
    in >> index;

    for (const auto& entry : index["players"])
    {
        auto player = std::make_shared<Craps::Player>();
        if (player->loadFromFile("players/" + entry["uuid"].get<Gen::Uuid>() + ".json"))
        {
            playersAll_[player->getUuid()] = player;
        }
    }
    return true;
}

/*-----------------------------------------------------------*//**

Save all players to file

*/
bool
PlayerManager::savePlayers()
{
    json index;
    for (const auto& [uuid, player] : playersAll_)
    {
        player->saveToFile("players/" + uuid + ".json");
        index["players"].push_back({
            {"uuid", uuid},
            {"name", player->getName()}
        });
    }

    std::ofstream out("players/index.json");
    out << index.dump(2);
    return true;
}

/*-----------------------------------------------------------*//**

Disburse WIN decision record to player.

Called by CrapsTable to dish out a winning bet to a Player.

*/
void
PlayerManager::disburseWin(const Craps::DecisionRecord& dr) const
{
    auto pPlayer = getPlayer(dr.playerId);
    if (pPlayer == nullptr)
    {
        diagBadPlayerId("disburseWin() ", dr.playerId);
        assert(false);
        return;
    }
    pPlayer->processWin(dr);
}

/*-----------------------------------------------------------*//**

Disburse LOSE decision record to player.

Called by CrapsTable to dish out a losing bet to a Player.

*/
void
PlayerManager::disburseLose(const Craps::DecisionRecord& dr) const
{
    auto pPlayer = getPlayer(dr.playerId);
    if (pPlayer == nullptr)
    {
        diagBadPlayerId("disburseLose() ", dr.playerId);
        assert(false);
        return;
    }
    pPlayer->processLose(dr);
}

/*-----------------------------------------------------------*//**

Disburse KEEP decision record to player.

Called by CrapsTable to inform a Player about their bet that
have no decision on the current roll of dice.

*/
void
PlayerManager::disburseKeep(const Craps::DecisionRecord& dr) const
{
    auto pPlayer = getPlayer(dr.playerId);
    if (pPlayer == nullptr)
    {
        diagBadPlayerId("disburseKeep() ", dr.playerId);
        assert(false);
        return;
    }
    pPlayer->processKeep(dr);
}

//----------------------------------------------------------------

void
PlayerManager::diagBadPlayerId(const std::string& funcName,
                               const PlayerId& playerId) const
{
    std::string diag =
        "Internal Error: Unable to process decision record. "
        "PlayerManager::" + funcName + "cant match "
        "decision record playerId against any playerId in "
        "playersAll list. Bad uuid:" + playerId;
    // TODO: error manager
    std::cerr << diag << std::endl;
}

//----------------------------------------------------------------
