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
    manifest_.loadFromFile();  // Load players manifest
    loadStartingPlayers();     // Throws
    addPlayersToTable();       // Throws
}

//----------------------------------------------------------------

PlayerManager::~PlayerManager()
{
    for (auto p : players_)
    {
        delete p;
    }
}

//----------------------------------------------------------------

void
PlayerManager::loadStartingPlayers()
{
    std::vector<std::string> playerFiles;
    
    Craps::PlayerId playerFile1 = Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer1).value();
    Craps::PlayerId playerFile2 = Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer2).value();
    Craps::PlayerId playerFile3 = Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer3).value();
    Craps::PlayerId playerFile4 = Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer4).value();
    Craps::PlayerId playerFile5 = Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer5).value();
    Craps::PlayerId playerFile6 = Gbl::pConfigMgr->getString(ConfigManager::KeyTablePlayer6).value();
    if (!playerFile1.empty()) playerFiles.push_back(playerFile1);
    if (!playerFile2.empty()) playerFiles.push_back(playerFile2);
    if (!playerFile3.empty()) playerFiles.push_back(playerFile3);
    if (!playerFile4.empty()) playerFiles.push_back(playerFile4);
    if (!playerFile5.empty()) playerFiles.push_back(playerFile5);
    if (!playerFile6.empty()) playerFiles.push_back(playerFile6);

    for (auto& f : playerFiles)
    {
        players_.push_back(loadPlayer(f));
    }
}

//----------------------------------------------------------------

Craps::Player*
PlayerManager::loadPlayer(const std::string& fileName)
{
    // Confirm fileName appears in players manifest
    auto playerId = manifest_.getPlayerId(fileName);
    if (playerId.empty())
    {
        throw std::runtime_error("TODO: fill me out");
    }
    
    Craps::PlayerConfig config;
    config.playerPath = PlayerManager::formPlayerPath(fileName);
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
        ep.prepend("PlayerManager::addPlayersToTable(): Unable to "
                   "add player to table; ");
        throw std::runtime_error(ep.diag);
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
    return manifest_.getPlayers();
}

//----------------------------------------------------------------

std::filesystem::path
PlayerManager::formPlayerPath(const std::string& fileName)
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysPlayers).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / fileName;
    return path;
}
    
/*-----------------------------------------------------------*//**

Returns the player that is the user.

This is the person operating the game as player "x".
*/
Craps::Player*
PlayerManager::getUserPlayer() const
{
    std::string playerFileName = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyTableUserIsPlayer).value();

    auto playerId = manifest_.getPlayerId(playerFileName);
    for (auto p : players_)
    {
        if (p->getPlayerId() == playerId)
        {
            return p;
        }
    }
    return nullptr;
}

//----------------------------------------------------------------
