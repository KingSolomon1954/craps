//----------------------------------------------------------------
//
// File: PlayerManager.cpp
//
//----------------------------------------------------------------

#include "craps/PlayerManager.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include "craps/DecisionRecord.h"

using namespace Craps;

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
    auto player = std::make_shared<Player>(name, 1000); // Default balance
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
        auto player = std::make_shared<Player>();
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
PlayerManager::disburseWin(const DecisionRecord& dr) const
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
PlayerManager::disburseLose(const DecisionRecord& dr) const
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
PlayerManager::disburseKeep(const DecisionRecord& dr) const
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
                               const Gen::Uuid& playerId) const
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
