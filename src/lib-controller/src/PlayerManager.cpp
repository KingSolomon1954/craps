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
#include <gen/FileUtils.h>
#include <algorithm>
#include <cctype>
#include <functional>
#include <system_error>
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

// What a new player starts with. Matches the packaged "unnamed" player.
constexpr unsigned NewPlayerBank          = 10000;
constexpr unsigned NewPlayerRefillAt      = 5000;
constexpr unsigned NewPlayerRefillAmount  = 10000;

constexpr const char* TrashFolder = "trash";

// Runs op, turning any exception into a failed ReturnCode with ep set.
Gen::ReturnCode
guarded(const std::string& fname, Gen::ErrorPass& ep,
        const std::function<Gen::ReturnCode()>& op)
{
    try
    {
        return op();
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
    LOG_ERROR(ep.fullDesc());
    return Gen::ReturnCode::Fail;
}

Gen::ReturnCode
fail(Gen::ErrorPass& ep, En name, const std::string& msg)
{
    EP_SET(ep, name, Et::ProcessingError, Es::Indeterminate, msg);
    return Gen::ReturnCode::Fail;
}

std::string
trim(const std::string& s)
{
    size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) b++;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) e--;
    return s.substr(b, e - b);
}

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

    // The manifest decides the name. The file may still hold an older
    // one if the game ended before it could be saved after a rename.
    if (const auto* desc = manifest_.find(playerId))
    {
        loaded.player->rename(desc->playerName);
    }

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

    const auto* toDesc = manifest_.find(toPlayerId);
    if (toDesc == nullptr)
    {
        EP_SET(ep, En::NotFoundError, Et::ProcessingError,
               Es::Indeterminate, fname + "no such player: \"" +
               toPlayerId + "\".");
        return Gen::ReturnCode::Fail;
    }
    if (toDesc->kind != PlayerKind::User)
    {
        EP_SET(ep, En::InvalidArgumentError, Et::ProcessingError,
               Es::Indeterminate, fname + toDesc->playerName +
               " is a buddy player and cannot become the user player.");
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
//
// File name (no directory) of the user player, "" if none.
//
std::string
PlayerManager::getUserFileName() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return (pUser_ != nullptr)
        ? manifest_.getFileName(pUser_->player->getPlayerId())
        : std::string();
}

//----------------------------------------------------------------
//
// File names of the players seated at the table right now, the user
// player first, so the user is never lost to a limit on how many are
// remembered.
//
std::vector<std::string>
PlayerManager::getTablePlayerFileNames() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    if (Gbl::pTable == nullptr)
    {
        return names;
    }

    auto add = [&](const Entry& e)
    {
        std::string f = manifest_.getFileName(e.player->getPlayerId());
        if (!f.empty() && Gbl::pTable->havePlayer(e.player))
        {
            names.push_back(std::move(f));
        }
    };
    if (pUser_ != nullptr)
    {
        add(*pUser_);
    }
    for (const auto& e : entries_)
    {
        if (e.get() != pUser_)
        {
            add(*e);
        }
    }
    return names;
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
    EP_SET(ep, Gen::ErrorPass::ErrorName::NotFoundError,
           Gen::ErrorPass::ErrorType::ProcessingError,
           Gen::ErrorPass::Severity::Indeterminate,
           "PlayerManager::getPLayer(): no such playerId; \"" + playerId + "\"");
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
        Ctrl::ConfigManager::KeyDirsUsrPlayers).value();

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

/*-----------------------------------------------------------*//**

Tells whether name is acceptable for a player, apart from being unique.

A name is 1 to MaxNameLength printable ASCII characters. Call with the
name already trimmed.

@param[in] name
    the proposed name
@param[out] whyNot
    if not acceptable, a sentence the person can read
*/
bool
PlayerManager::checkName(const std::string& name, std::string& whyNot)
{
    if (name.empty())
    {
        whyNot = "A player needs a name.";
        return false;
    }
    if (name.size() > MaxNameLength)
    {
        whyNot = "A name can be at most " + std::to_string(MaxNameLength) +
                 " characters.";
        return false;
    }
    for (char c : name)
    {
        const unsigned char u = static_cast<unsigned char>(c);
        if (u < 0x20 || u > 0x7e)
        {
            whyNot = "A name can only have letters, digits, spaces and "
                     "ordinary punctuation.";
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------
//
// A short description may be empty. Otherwise it is at most
// MaxDescriptionLength printable ASCII characters. Call with the text
// already trimmed.
//
bool
PlayerManager::checkDescription(const std::string& description,
                                std::string& whyNot)
{
    if (description.size() > MaxDescriptionLength)
    {
        whyNot = "A description can be at most " +
                 std::to_string(MaxDescriptionLength) + " characters.";
        return false;
    }
    for (char c : description)
    {
        const unsigned char u = static_cast<unsigned char>(c);
        if (u < 0x20 || u > 0x7e)
        {
            whyNot = "A description can only have letters, digits, spaces "
                     "and ordinary punctuation.";
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------
//
// Name is valid and no other player has it. Caller holds the mutex.
//
bool
PlayerManager::nameOk(const std::string& name,
                      const Craps::PlayerId& exceptId,
                      Gen::ErrorPass& ep) const
{
    std::string why;
    if (!checkName(name, why))
    {
        fail(ep, En::InvalidArgumentError, why);
        return false;
    }
    const auto* other = manifest_.findByName(name);
    if (other != nullptr && other->playerId != exceptId)
    {
        fail(ep, En::ExistsError,
             "A player named " + other->playerName + " already exists.");
        return false;
    }
    return true;
}

//----------------------------------------------------------------
//
// Everything the View shows when listing players. In manifest order.
//
std::vector<PlayerManager::PlayerInfo>
PlayerManager::listPlayers() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PlayerInfo> list;
    for (const auto& d : manifest_.getPlayerList())
    {
        PlayerInfo info;
        info.playerId = d.playerId;
        info.name     = d.playerName;
        info.fileName = d.fileName;
        info.description = d.shortDescription;
        info.kind     = d.kind;
        info.isUser   = (pUser_ != nullptr &&
                         pUser_->player->getPlayerId() == d.playerId);
        const Entry* e = findEntryById(d.playerId);
        info.atTable  = (e != nullptr && Gbl::pTable != nullptr &&
                         Gbl::pTable->havePlayer(e->player));
        list.push_back(std::move(info));
    }
    return list;
}

//----------------------------------------------------------------
//
// Returns the entry for playerId, reading the player's file first if
// the player is not loaded yet. Caller holds the mutex. Throws.
//
PlayerManager::Entry*
PlayerManager::ensureLoaded(const Craps::PlayerId& playerId)
{
    if (Entry* e = findEntryById(playerId))
    {
        return e;
    }
    const std::string fileName = manifest_.getFileName(playerId);
    if (fileName.empty())
    {
        throw std::runtime_error("no such player: \"" + playerId + "\"");
    }
    reserveOneMore();
    addLoaded(readPlayer(fileName));          // throws
    return entries_.back().get();
}

//----------------------------------------------------------------
//
// A file name for a new player: made from the name, not used by any
// player and not already on disk. Caller holds the mutex.
//
std::string
PlayerManager::makeFileName(const std::string& name) const
{
    // nameToFileName() adds the ".yaml"; it is added back below, after
    // any number that makes the name unique.
    std::string base = Gen::FileUtils::nameToFileName(name);
    const std::string ext(".yaml");
    if (base.size() >= ext.size() &&
        base.compare(base.size() - ext.size(), ext.size(), ext) == 0)
    {
        base.resize(base.size() - ext.size());
    }
    if (base.empty())
    {
        base = "player";
    }

    std::error_code ec;
    for (int n = 1; ; n++)
    {
        const std::string candidate =
            base + (n == 1 ? "" : "-" + std::to_string(n)) + ".yaml";
        if (!manifest_.fileNameInUse(candidate) &&
            !std::filesystem::exists(formPlayerPath(candidate), ec))
        {
            return candidate;
        }
    }
}

/*-----------------------------------------------------------*//**

Gives a player a new name. See the class description.

@param[in] playerId
    the player to rename
@param[in] newName
    the new name; leading and trailing blanks are ignored
@param[in,out] ep
    if error, ep has the reason
@return
    Success or Fail
*/
Gen::ReturnCode
PlayerManager::renamePlayer(const Craps::PlayerId& playerId,
                            const std::string& newName,
                            Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::renamePlayer(): ");
    std::lock_guard<std::mutex> lock(mutex_);
    return guarded(fname, ep, [&]() -> Gen::ReturnCode
    {
        const auto* desc = manifest_.find(playerId);
        if (desc == nullptr)
        {
            return fail(ep, En::NotFoundError,
                        fname + "no such player: \"" + playerId + "\".");
        }
        if (desc->kind != PlayerKind::User)
        {
            return fail(ep, En::InvalidArgumentError,
                        desc->playerName + " is a buddy player and cannot "
                        "be renamed.");
        }
        const std::string name = trim(newName);
        if (!nameOk(name, playerId, ep))
        {
            return Gen::ReturnCode::Fail;
        }

        const std::string oldName = desc->playerName;
        manifest_.rename(playerId, name);
        try
        {
            manifest_.saveToFile();                           // throws
        }
        catch (...)
        {
            manifest_.rename(playerId, oldName);
            throw;
        }

        // A loaded player is saved with his new name at the next switch
        // or at quit. Until then the manifest is what holds it.
        if (Entry* e = findEntryById(playerId))
        {
            e->player->rename(name);
        }
        LOG_INFO(fname + "\"" + oldName + "\" is now \"" + name + "\"");
        return Gen::ReturnCode::Success;
    });
}

/*-----------------------------------------------------------*//**

Makes a new user player. See the class description.

@param[in] name
    the new player's name; leading and trailing blanks are ignored
@param[out] newPlayerId
    the new player's id, if successful
@param[in,out] ep
    if error, ep has the reason
@return
    Success or Fail
*/
Gen::ReturnCode
PlayerManager::createPlayer(const std::string& name,
                            const std::string& description,
                            Craps::PlayerId& newPlayerId,
                            Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::createPlayer(): ");
    std::lock_guard<std::mutex> lock(mutex_);
    return guarded(fname, ep, [&]() -> Gen::ReturnCode
    {
        const std::string trimmed = trim(name);
        if (!nameOk(trimmed, Craps::PlayerId(), ep))
        {
            return Gen::ReturnCode::Fail;
        }
        const std::string about = trim(description);
        std::string why;
        if (!checkDescription(about, why))
        {
            return fail(ep, En::InvalidArgumentError, why);
        }

        // Start from a default player, then set the starting bank.
        std::unique_ptr<Craps::Player> fresh(
            Craps::Player::createPlayer(trimmed));
        const Craps::PlayerId id = fresh->getPlayerId();
        YAML::Node node = fresh->toYAML();
        node["Bank"]["originalStartBalance"] = NewPlayerBank;
        node["Bank"]["sessionStartBalance"]  = NewPlayerBank;
        node["Bank"]["refillThreshold"]      = NewPlayerRefillAt;
        node["Bank"]["refillAmount"]         = NewPlayerRefillAmount;

        const std::string fileName = makeFileName(trimmed);
        reserveOneMore();                       // So adding him cannot fail
        Loaded loaded;
        loaded.player.reset(Craps::Player::fromString(YAML::Dump(node), id));
        loaded.entry = std::make_unique<Entry>(loaded.player.get(),
                                               formPlayerPath(fileName));
        saveEntry(*loaded.entry);               // throws; writes the file

        PlayerManifest::PlayerDescription desc;
        desc.playerId         = id;
        desc.kind             = PlayerKind::User;
        desc.playerName       = trimmed;
        desc.shortDescription = about.empty() ? "Created by you" : about;
        desc.fullDescription  = about.empty() ? "A player you created." : about;
        desc.fileName         = fileName;
        manifest_.add(desc);
        try
        {
            manifest_.saveToFile();             // throws
        }
        catch (...)
        {
            manifest_.remove(id);
            std::error_code ignore;
            std::filesystem::remove(loaded.entry->file.getPath(), ignore);
            throw;
        }

        addLoaded(std::move(loaded));
        newPlayerId = id;
        LOG_INFO(fname + "created \"" + trimmed + "\" in " + fileName);
        return Gen::ReturnCode::Success;
    });
}

/*-----------------------------------------------------------*//**

Deletes a user player. See the class description. The player's file is
moved to the "trash" folder, so it can be recovered by hand.

@param[in] playerId
    the player to delete
@param[in,out] ep
    if error, ep has the reason
@return
    Success or Fail
*/
Gen::ReturnCode
PlayerManager::deletePlayer(const Craps::PlayerId& playerId,
                            Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::deletePlayer(): ");
    std::lock_guard<std::mutex> lock(mutex_);
    return guarded(fname, ep, [&]() -> Gen::ReturnCode
    {
        namespace fs = std::filesystem;

        const auto* found = manifest_.find(playerId);
        if (found == nullptr)
        {
            return fail(ep, En::NotFoundError,
                        fname + "no such player: \"" + playerId + "\".");
        }
        const PlayerManifest::PlayerDescription desc = *found;   // copy

        if (desc.kind != PlayerKind::User)
        {
            return fail(ep, En::InvalidArgumentError,
                        desc.playerName + " is a buddy player and cannot "
                        "be deleted.");
        }
        if (pUser_ != nullptr && pUser_->player->getPlayerId() == playerId)
        {
            return fail(ep, En::IllegalStateError,
                        desc.playerName + " is the player you are playing "
                        "as. Switch to another player first.");
        }
        Entry* e = findEntryById(playerId);
        if (e != nullptr && Gbl::pTable != nullptr &&
            Gbl::pTable->havePlayer(e->player))
        {
            return fail(ep, En::IllegalStateError,
                        desc.playerName + " is at the table.");
        }

        // Move the file to the trash first; nothing else has changed yet.
        const fs::path from = formPlayerPath(desc.fileName);
        fs::path trashed;
        std::error_code ec;
        if (fs::exists(from, ec))
        {
            const fs::path trashDir = from.parent_path() / TrashFolder;
            fs::create_directories(trashDir, ec);
            if (ec)
            {
                throw std::runtime_error("unable to create \"" +
                    trashDir.string() + "\": " + ec.message());
            }
            trashed = trashDir / desc.fileName;
            for (int n = 2; fs::exists(trashed, ec); n++)
            {
                trashed = trashDir /
                    (from.stem().string() + "-" + std::to_string(n) +
                     from.extension().string());
            }
            fs::rename(from, trashed, ec);
            if (ec)
            {
                throw std::runtime_error("unable to move \"" +
                    from.string() + "\" to the trash: " + ec.message());
            }
        }

        manifest_.remove(playerId);
        try
        {
            manifest_.saveToFile();                           // throws
        }
        catch (...)
        {
            manifest_.add(desc);                              // undo
            if (!trashed.empty())
            {
                std::error_code ignore;
                fs::rename(trashed, from, ignore);
            }
            throw;
        }

        if (e != nullptr)
        {
            Craps::Player* player = e->player;
            players_.erase(std::find(players_.begin(), players_.end(), player));
            entries_.erase(std::find_if(entries_.begin(), entries_.end(),
                [&](const std::unique_ptr<Entry>& u) { return u.get() == e; }));
            delete player;
        }
        LOG_INFO(fname + "deleted \"" + desc.playerName + "\"; file moved to " +
                 (trashed.empty() ? std::string("(none; file was missing)")
                                  : trashed.string()));
        return Gen::ReturnCode::Success;
    });
}

/*-----------------------------------------------------------*//**

A buddy player sits down at the table.

@param[in] playerId
    the buddy player
@param[in,out] ep
    if error, ep has the reason. For example, the table is full.
@return
    Success or Fail
*/
Gen::ReturnCode
PlayerManager::buddyJoinsTable(const Craps::PlayerId& playerId,
                               Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::buddyJoinsTable(): ");
    std::lock_guard<std::mutex> lock(mutex_);
    return guarded(fname, ep, [&]() -> Gen::ReturnCode
    {
        const auto* desc = manifest_.find(playerId);
        if (desc == nullptr)
        {
            return fail(ep, En::NotFoundError,
                        fname + "no such player: \"" + playerId + "\".");
        }
        if (desc->kind != PlayerKind::Buddy)
        {
            return fail(ep, En::InvalidArgumentError,
                        desc->playerName + " is not a buddy player.");
        }
        if (Gbl::pTable == nullptr)
        {
            return fail(ep, En::IllegalStateError, fname + "no table.");
        }
        Entry* e = ensureLoaded(playerId);                    // throws
        if (Gbl::pTable->havePlayer(e->player))
        {
            return fail(ep, En::ExistsError,
                        e->player->getName() + " is already at the table.");
        }
        auto result = e->player->joinTable(*Gbl::pTable);
        if (!result)
        {
            ep = std::move(result).error();
            ep.prepend(fname);
            return Gen::ReturnCode::Fail;
        }
        LOG_INFO(fname + e->player->getName() + " joined the table");
        return Gen::ReturnCode::Success;
    });
}

/*-----------------------------------------------------------*//**

A buddy player leaves the table. His bets come off the table and are
returned to him, and his session is saved.

@param[in] playerId
    the buddy player
@param[in,out] ep
    if error, ep has the reason
@return
    Success or Fail
*/
Gen::ReturnCode
PlayerManager::buddyLeavesTable(const Craps::PlayerId& playerId,
                                Gen::ErrorPass& ep)
{
    const std::string fname("PlayerManager::buddyLeavesTable(): ");
    std::lock_guard<std::mutex> lock(mutex_);
    return guarded(fname, ep, [&]() -> Gen::ReturnCode
    {
        const auto* desc = manifest_.find(playerId);
        if (desc == nullptr)
        {
            return fail(ep, En::NotFoundError,
                        fname + "no such player: \"" + playerId + "\".");
        }
        if (desc->kind != PlayerKind::Buddy)
        {
            return fail(ep, En::InvalidArgumentError,
                        desc->playerName + " is not a buddy player.");
        }
        Entry* e = findEntryById(playerId);
        if (e == nullptr || Gbl::pTable == nullptr ||
            !Gbl::pTable->havePlayer(e->player))
        {
            return fail(ep, En::NotFoundError,
                        desc->playerName + " is not at the table.");
        }

        e->player->endSession();     // Leaves the table, returns bets
        try
        {
            saveEntry(*e);
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR(fname + "unable to save " + e->player->getName() +
                      " after leaving; " + ex.what());
        }
        LOG_INFO(fname + e->player->getName() + " left the table");
        return Gen::ReturnCode::Success;
    });
}

//----------------------------------------------------------------
