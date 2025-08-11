//----------------------------------------------------------------
//
// File: PlayerManager.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerManager.h>
#include <cassert>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
PlayerManager::PlayerManager()
{
    loadStartingPlayers();  // throws
    addPlayersToTable();    // throws
}

//----------------------------------------------------------------

PlayerManager::~PlayerManager()
{
    // empty
}

//----------------------------------------------------------------

Craps::Player*
PlayerManager::loadPlayer(const Craps::PlayerId& playerId)
{
    Craps::PlayerConfig config;
    config.playerPath = PlayerManager::formPlayerPath(playerId);

    return Craps::Player::fromFile(playerId, config, *Gbl::pEventMgr);
}

//----------------------------------------------------------------

void
PlayerManager::addPlayersToTable()
{
    Gen::ErrorPass ep;
    bool booboo = false;
    
    for (auto p : players_)
    {
        auto rc = p->joinTable(Gbl::pTable, ep);
        if (rc == Gen::ReturnCode::Fail) booboo = true;
    }
    if (booboo)
    {
        // TODO beef this up
        throw std::runtime_error(ep.diag);
    }
}

//----------------------------------------------------------------

void
PlayerManager::loadStartingPlayers()
{
    // TODO
    std::vector<Craps::PlayerId> playerVec;
    
    Craps::PlayerId pid1 =
        Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer1).value();
    playerVec.push_back(pid1);

    // TODO
    for (auto pid : playerVec)
    {
        players_.push_back(loadPlayer(pid));
    }
}

//----------------------------------------------------------------

const PlayerManager::Players& 
PlayerManager::getPlayers() const
{
    return players_;
}

//----------------------------------------------------------------

const PlayerManager::PlayerDescriptions&
PlayerManager::getPlayerChoices() const
{
    // TODO
    manifest_.loadFromFile();  // Load players.yaml manifest
    return manifest_.getPlayers();
}

//----------------------------------------------------------------

std::filesystem::path
PlayerManager::formPlayerPath(const Craps::PlayerId& playerId)
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysPlayers).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / (playerId + ".yaml");
    return path;
}
    
//----------------------------------------------------------------








#if 0
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

Craps::Player
PlayerManager::loadPlayer(const Craps::PlayerId& playerId)
{
    // TODO: read from file and create player

    Craps::PlayerConfig config;
    return Craps::Player("1", config, *Gbl::pEventMgr);
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

#endif

