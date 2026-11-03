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
#include <controller/LastFill.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
#include <yaml-cpp/yaml.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Constructor

*/
PlayerManager::PlayerManager()
{
    manifest_.loadFromFile();  // Load players manifest
    loadStartingPlayers();     // Throws
    addPlayersToTable();       // Throws
    loadUserBetPrefs();        // User's LastFills, etc.
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
        std::string diag = "PlayerManager::loadPlayer(): unable to load "
            "player file; lookup failed; file name:\"" + fileName +
            "\" is not found in manifest file:\"players.yaml\".";
        throw std::runtime_error(diag);
    }

    Craps::PlayerConfig config;
    config.playerPath = PlayerManager::formPlayerPath(fileName);
    return Craps::Player::fromFile(playerId, config);
}

//----------------------------------------------------------------

void
PlayerManager::addPlayersToTable()
{
    Gen::ErrorPass ep;
    bool booboo = false;

    for (auto p : players_)
    {
        auto result = p->joinTable(*Gbl::pTable);
        if (!result)
        {
            ep = result.error();
            booboo = true;
        }
    }
    if (booboo)
    {
        ep.prepend("PlayerManager::addPlayersToTable(): Unable to "
                   "add player to table; ");
        throw std::runtime_error(ep.description());
    }

    // Tell the table which user starts off as shooter.
    Gbl::pTable->setShooter(*getUserPlayer());
}

//----------------------------------------------------------------
//
// Copies the controller's bet preferences into the player, in memory.
// The player writes them to its file in saveFile(), which happens at
// player shutdown. Keys in the section that we don't own are kept.
//
void
PlayerManager::saveBetPrefs(Craps::Player& player) const
{
    YAML::Node prefs = player.getBetPreferences();  // a copy
    if (!prefs.IsMap())
    {
        prefs = YAML::Node(YAML::NodeType::Map);    // missing or junk
    }
    prefs["LastFills"] = LastFill::instance().toYAML();
    player.setBetPreferences(prefs);
}

//----------------------------------------------------------------
//
// Replaces the controller's bet preferences with the player's.
//
void
PlayerManager::loadBetPrefs(const Craps::Player& player) const
{
    const YAML::Node prefs = player.getBetPreferences();
    if (prefs.IsMap())
    {
        LastFill::instance().fromYAML(prefs["LastFills"]);
    }
    else
    {
        LastFill::instance().clear();  // player has none
    }
}

//----------------------------------------------------------------

void
PlayerManager::saveUserBetPrefs() const
{
    if (Craps::Player* p = getUserPlayer())
    {
        saveBetPrefs(*p);
    }
}

//----------------------------------------------------------------

void
PlayerManager::loadUserBetPrefs() const
{
    if (Craps::Player* p = getUserPlayer())
    {
        loadBetPrefs(*p);
    }
}

//----------------------------------------------------------------

Craps::Player*
PlayerManager::getPlayer(const Craps::PlayerId& playerId,
                         Gen::ErrorPass& ep) const
{
    for (auto p : players_)
    {
        if (p->getPlayerId() == playerId)
        {
            return p;
        }
    }
    ep.setDescription("PlayerManager::getPLayer(): no such playerId; \"" +
                      playerId + "\"");
    return nullptr;
}

//----------------------------------------------------------------

const PlayerManifest::PlayerList&
PlayerManager::getPlayerList() const
{
    return manifest_.getPlayerList();
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
