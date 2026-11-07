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

namespace {

// Top-level sections of a player file
constexpr const char* KeyPlayer = "Player";
constexpr const char* KeyPrefs  = "Prefs";

using En = Gen::ErrorPass::ErrorName;
using Et = Gen::ErrorPass::ErrorType;
using Es = Gen::ErrorPass::Severity;

} // namespace

/*-----------------------------------------------------------*//**

Constructor

*/
PlayerManager::PlayerManager()
{
    manifest_.loadFromFile();  // Load players manifest
    loadStartingPlayers();     // Throws
    chooseStartingUser();      // Throws
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
        addLoaded(readPlayer(f));
    }
}

//----------------------------------------------------------------
//
// The user player at startup comes from configuration. It must be one
// of the players just loaded.
//
void
PlayerManager::chooseStartingUser()
{
    std::string fileName = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyTableUserIsPlayer).value();

    pUser_ = findEntryById(manifest_.getPlayerId(fileName));
    if (pUser_ == nullptr)
    {
        throw std::runtime_error(
            "PlayerManager::chooseStartingUser(): the user player \"" +
            fileName + "\" is not one of the players loaded at startup.");
    }
}

//----------------------------------------------------------------
//
// Reads a player's file. Adds nothing to the manager; see addLoaded().
//
PlayerManager::Loaded
PlayerManager::readPlayer(const std::string& fileName) const
{
    // Confirm fileName appears in players manifest
    auto playerId = manifest_.getPlayerId(fileName);
    if (playerId.empty())
    {
        std::string diag = "PlayerManager::readPlayer(): unable to load "
            "player file; lookup failed; file name:\"" + fileName +
            "\" is not found in manifest file:\"players.yaml\".";
        throw std::runtime_error(diag);
    }

    Loaded loaded;
    loaded.player = std::make_unique<Craps::Player>(playerId);
    loaded.entry  = std::make_unique<Entry>(loaded.player.get(),
                                            formPlayerPath(fileName));
    Entry& entry = *loaded.entry;

    entry.file.load(KeyPlayer);                                // throws
    try
    {
        loaded.player->fromYAML(entry.file.getSection(KeyPlayer)); // throws
    }
    catch (const std::runtime_error& e)
    {
        throw std::runtime_error("PlayerManager::readPlayer(): \"" +
            entry.file.getPath().string() + "\"; " + e.what());
    }
    entry.prefs.fromYAML(entry.file.getSection(KeyPrefs));     // forgiving

    return loaded;
}

//----------------------------------------------------------------
//
// Hands a loaded player over to the manager. Does not throw as long as
// reserveOneMore() was called first, which keeps a failure from leaving
// a player half added.
//
void
PlayerManager::addLoaded(Loaded&& loaded)
{
    reserveOneMore();
    players_.push_back(loaded.player.release());
    entries_.push_back(std::move(loaded.entry));
}

//----------------------------------------------------------------

void
PlayerManager::reserveOneMore()
{
    players_.reserve(players_.size() + 1);
    entries_.reserve(entries_.size() + 1);
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
    Gbl::pTable->setShooter(*pUser_->player);
}

//----------------------------------------------------------------
//
// Returns the preferences of whoever is the user player right now.
//
PlayerPrefs&
PlayerManager::userPrefs()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pUser_ == nullptr)
    {
        throw std::logic_error("PlayerManager::userPrefs(): "
                               "there is no user player");
    }
    return pUser_->prefs;
}

//----------------------------------------------------------------

const PlayerPrefs&
PlayerManager::userPrefs() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pUser_ == nullptr)
    {
        throw std::logic_error("PlayerManager::userPrefs(): "
                               "there is no user player");
    }
    return pUser_->prefs;
}

//----------------------------------------------------------------
//
// Returns nullptr if there is no such player.
//
PlayerPrefs*
PlayerManager::getPrefs(const Craps::PlayerId& playerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    Entry* e = findEntryById(playerId);
    return (e != nullptr) ? &e->prefs : nullptr;
}

//----------------------------------------------------------------
//
// Caller holds the mutex.
//
PlayerManager::Entry*
PlayerManager::findEntryById(const Craps::PlayerId& playerId) const
{
    for (auto& e : entries_)
    {
        if (e->player->getPlayerId() == playerId)
        {
            return e.get();
        }
    }
    return nullptr;
}

//----------------------------------------------------------------
//
// Called once at quit, after TableManager::shutdown() has shut the
// players down. Saves every player.
//
void
PlayerManager::shutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    saveAll();
}

//----------------------------------------------------------------
//
// Writes every player's file from a fresh snapshot of the player and
// its preferences. One player failing does not stop the others.
// Failures are logged. Caller holds the mutex.
//
void
PlayerManager::saveAll() const
{
    for (auto& e : entries_)
    {
        try
        {
            saveEntry(*e);                                    // throws
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR(std::string("PlayerManager::saveAll(): ") + ex.what());
        }
    }
}

//----------------------------------------------------------------

void
PlayerManager::saveEntry(Entry& entry) const
{
    entry.file.setSection(KeyPlayer, entry.player->toYAML());
    entry.file.setSection(KeyPrefs,  entry.prefs.toYAML());
    entry.file.save();                                        // throws
}

/*-----------------------------------------------------------*//**

Makes toPlayerId the user player.

See the class description for what happens. If this fails, nothing has
changed from the user's point of view: the current user player is still
the user and still at the table. Never throws.

@param[in] toPlayerId
    the player who becomes the user player
@param[in,out] ep
    if error, ep has the reason
@return
    Success if switched, otherwise Fail and ep has the reason
*/
Gen::ReturnCode
PlayerManager::switchPlayer(const Craps::PlayerId& toPlayerId,
                            Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::switchPlayer(): ");

    std::lock_guard<std::mutex> lock(mutex_);
    Entry* from = pUser_;
    try
    {
        return doSwitchPlayer(toPlayerId, ep);
    }
    catch (const std::exception& e)
    {
        EP_SET(ep, En::CaughtException, Et::ProcessingError,
               Es::Indeterminate, fname + e.what());
    }
    catch (...)
    {
        EP_SET(ep, En::CaughtException, Et::ProcessingError,
               Es::Indeterminate, fname + "unknown exception");
    }

    // Something unexpected got in the way. Make sure the user player is
    // still at the table. Normally already true: the table is only
    // touched after everything that can fail has been done.
    try
    {
        if (from != nullptr && Gbl::pTable != nullptr &&
            !Gbl::pTable->havePlayer(from->player))
        {
            (void) from->player->joinTable(*Gbl::pTable);
        }
    }
    catch (...)
    {
        LOG_ERROR(fname + "unable to put the user player back at the table");
    }
    pUser_ = from;
    LOG_ERROR(ep.fullDesc());
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------
//
// The work of switchPlayer(). Caller holds the mutex and catches
// whatever this throws.
//
// Everything that can fail comes first and changes nothing. Only then
// does the table swap the players, and only after that is anything
// committed.
//
Gen::ReturnCode
PlayerManager::doSwitchPlayer(const Craps::PlayerId& toPlayerId,
                              Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::switchPlayer(): ");

    // ---- Check, change nothing ----------------------------------

    Entry* from = pUser_;
    if (from == nullptr || Gbl::pTable == nullptr)
    {
        EP_SET(ep, En::IllegalStateError, Et::ProcessingError,
               Es::Indeterminate, fname + "there is no user player "
               "or no table.");
        return Gen::ReturnCode::Fail;
    }
    if (from->player->getPlayerId() == toPlayerId)
    {
        EP_SET(ep, En::InvalidArgumentError, Et::ProcessingError,
               Es::Indeterminate, fname + from->player->getName() +
               " is already the user player.");
        return Gen::ReturnCode::Fail;
    }

    Entry* to = findEntryById(toPlayerId);
    Loaded loaded;                 // Owns a player read just now, if any
    if (to == nullptr)
    {
        const std::string fileName = manifest_.getFileName(toPlayerId);
        if (fileName.empty())
        {
            EP_SET(ep, En::NotFoundError, Et::ProcessingError,
                   Es::Indeterminate, fname + "no such player: \"" +
                   toPlayerId + "\".");
            return Gen::ReturnCode::Fail;
        }
        reserveOneMore();                  // So adding him later cannot fail
        loaded = readPlayer(fileName);     // Throws
        to = loaded.entry.get();
    }
    else if (Gbl::pTable->havePlayer(to->player))
    {
        EP_SET(ep, En::ExistsError, Et::ProcessingError,
               Es::Indeterminate, fname + to->player->getName() +
               " is already at the table.");
        return Gen::ReturnCode::Fail;
    }

    // ---- The swap -----------------------------------------------
    // Leaving player's session ends, bets come off, new player takes
    // the seat. On failure the table is unchanged.

    auto result = Gbl::pTable->replacePlayer(*from->player, *to->player);
    if (!result)
    {
        ep = std::move(result).error();
        ep.prepend(fname);
        return Gen::ReturnCode::Fail;
    }

    // ---- Commit -------------------------------------------------

    if (loaded.entry)
    {
        addLoaded(std::move(loaded));
    }
    pUser_ = to;

    // Write the player who left. His session has ended and the stats
    // are merged. If this fails the switch still stands.
    try
    {
        saveEntry(*from);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(fname + "unable to save " + from->player->getName() +
                  " after switching away; " + e.what());
    }

    LOG_INFO(fname + "user player is now " + to->player->getName() +
             " (was " + from->player->getName() + ")");
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Craps::Player*
PlayerManager::getPlayer(const Craps::PlayerId& playerId,
                         Gen::ErrorPass& ep) const
{
    std::lock_guard<std::mutex> lock(mutex_);
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

const PlayerManager::Players&
PlayerManager::getPlayers() const
{
    return players_;
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
    std::lock_guard<std::mutex> lock(mutex_);
    return (pUser_ != nullptr) ? pUser_->player : nullptr;
}

//----------------------------------------------------------------
