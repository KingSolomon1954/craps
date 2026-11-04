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

    auto player = std::make_unique<Craps::Player>(playerId);
    auto entry  = std::make_unique<Entry>(player.get(),
                                          formPlayerPath(fileName));
    entry->file.load();                                        // throws
    try
    {
        player->fromYAML(entry->file.getPlayerNode());         // throws
    }
    catch (const std::runtime_error& e)
    {
        throw std::runtime_error("PlayerManager::loadPlayer(): \"" +
            entry->file.getPath().string() + "\"; " + e.what());
    }
    entry->prefs.fromYAML(entry->file.getPrefsNode());         // forgiving

    entries_.push_back(std::move(entry));
    return player.release();
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
// Returns the preferences of whoever is the user player right now.
//
PlayerPrefs&
PlayerManager::userPrefs()
{
    Entry* e = findEntry(getUserPlayer());
    if (e == nullptr)
    {
        throw std::logic_error("PlayerManager::userPrefs(): "
                               "there is no user player");
    }
    return e->prefs;
}

//----------------------------------------------------------------

const PlayerPrefs&
PlayerManager::userPrefs() const
{
    Entry* e = findEntry(getUserPlayer());
    if (e == nullptr)
    {
        throw std::logic_error("PlayerManager::userPrefs(): "
                               "there is no user player");
    }
    return e->prefs;
}

//----------------------------------------------------------------
//
// Returns nullptr if there is no such player.
//
PlayerPrefs*
PlayerManager::getPrefs(const Craps::PlayerId& playerId)
{
    for (auto& e : entries_)
    {
        if (e->player->getPlayerId() == playerId)
        {
            return &e->prefs;
        }
    }
    return nullptr;
}

//----------------------------------------------------------------

PlayerManager::Entry*
PlayerManager::findEntry(const Craps::Player* player) const
{
    for (auto& e : entries_)
    {
        if (e->player == player)
        {
            return e.get();
        }
    }
    return nullptr;
}

//----------------------------------------------------------------
//
// Writes every player's file from a fresh snapshot of the player and
// its preferences. One player failing does not stop the others. If any
// fail, returns Fail with a description of each.
//
Gen::ReturnCode
PlayerManager::saveAll(Gen::ErrorPass& ep) const
{
    std::string failures;

    for (auto& e : entries_)
    {
        try
        {
            e->file.setPlayerNode(e->player->toYAML());
            e->file.setPrefsNode (e->prefs.toYAML());
            e->file.save();                                   // throws
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR(std::string("PlayerManager::saveAll(): ") + ex.what());
            failures += std::string(failures.empty() ? "" : "; ") + ex.what();
        }
    }

    if (!failures.empty())
    {
        ep.setDescription("PlayerManager::saveAll(): " + failures);
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;
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
