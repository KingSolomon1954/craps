//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <filesystem>
#include <mutex>
#include <utility>
#include <vector>
#include <controller/PlayerManifest.h>
#include <controller/PlayerPrefs.h>
#include <controller/YamlFile.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

namespace Craps {
    class Player;  // fwd
}

namespace Ctrl {

class PlayerManager
{
public:
    using Players = std::vector<Craps::Player*>;

    /// One known player, as the View needs to show it.
    struct PlayerInfo
    {
        Craps::PlayerId playerId;
        std::string     name;
        std::string     fileName;
        std::string     description;       ///< Short description
        PlayerKind      kind    = PlayerKind::User;
        bool            isUser  = false;   ///< The current user player
        bool            atTable = false;
    };

    static constexpr std::size_t MaxNameLength        = 16;
    static constexpr std::size_t MaxDescriptionLength = 40;

    /// @name Lifecycle
    /// @{
    PlayerManager();
   ~PlayerManager();
    /// @}

    /// @name Modifiers
    /// @{
    void shutdown();
    void savePlayers();    // Writes every loaded player's file
    Gen::ReturnCode switchPlayer(const Craps::PlayerId& toPlayerId,
                                 Gen::ErrorPass& ep);
    PlayerPrefs& userPrefs();
    PlayerPrefs* getPrefs(const Craps::PlayerId& playerId);
    Gen::ReturnCode renamePlayer(const Craps::PlayerId& playerId,
                                 const std::string& newName,
                                 Gen::ErrorPass& ep);
    Gen::ReturnCode createPlayer(const std::string& name,
                                 const std::string& description,
                                 Craps::PlayerId& newPlayerId,
                                 Gen::ErrorPass& ep);
    Gen::ReturnCode deletePlayer(const Craps::PlayerId& playerId,
                                 Gen::ErrorPass& ep);
    Gen::ReturnCode buddyJoinsTable(const Craps::PlayerId& playerId,
                                    Gen::ErrorPass& ep);
    Gen::ReturnCode buddyLeavesTable(const Craps::PlayerId& playerId,
                                     Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    const PlayerPrefs& userPrefs()                    const;
    const PlayerManifest::PlayerList& getPlayerList() const;
    std::vector<PlayerInfo> listPlayers()             const;
    static bool checkName(const std::string& name, std::string& whyNot);
    static bool checkDescription(const std::string& description,
                                 std::string& whyNot);
    const Players& getPlayers()                       const;
    Craps::Player* getUserPlayer()                    const;
    std::string getUserFileName()                     const;
    std::vector<std::string> getTablePlayerFileNames() const;
    Craps::Player* getPlayer(
        const Craps::PlayerId& playerId,
        Gen::ErrorPass& ep)                           const;
    /// @}

private:
    // Everything kept for one loaded player. The Player itself is owned
    // by players_, not by the Entry.
    struct Entry
    {
        Entry(Craps::Player* p, std::filesystem::path path)
            : player(p), file(std::move(path)) {}

        Craps::Player* player;
        YamlFile       file;
        PlayerPrefs    prefs;
    };

    // A player read from its file but not yet handed to the manager.
    // Whatever is still owned here is deleted if the manager never
    // takes it.
    struct Loaded
    {
        std::unique_ptr<Craps::Player> player;
        std::unique_ptr<Entry>         entry;
    };

    mutable PlayerManifest manifest_;
    Players players_;
    std::vector<std::unique_ptr<Entry>> entries_;
    Entry* pUser_ = nullptr;             // the user player's entry
    mutable std::mutex mutex_;           // guards the four above

    Loaded readPlayer(const std::string& fileName) const;   // throws
    void   addLoaded(Loaded&& loaded);                      // no throw if reserved
    void   reserveOneMore();                                // throws
    Entry* findEntryById(const Craps::PlayerId& playerId) const;
    Gen::ReturnCode doSwitchPlayer(const Craps::PlayerId& toPlayerId,
                                   Gen::ErrorPass& ep);     // may throw
    Entry* ensureLoaded(const Craps::PlayerId& playerId);   // throws
    bool   nameOk(const std::string& name, const Craps::PlayerId& exceptId,
                  Gen::ErrorPass& ep) const;
    std::string makeFileName(const std::string& name) const;
    void   saveEntry(Entry& entry) const;                   // throws
    void   saveAll() const;
    void   loadStartingPlayers();  // throws
    void   chooseStartingUser();   // throws
    void   addPlayersToTable();    // throws
    static std::filesystem::path formPlayerPath(const std::string& fileName);
};

/*-----------------------------------------------------------*//**

@class PlayerManager

@brief Managers players

Manages players.

Maintains a central index file (e.g., players/index.json) listing all
known player IDs and names for fast access/UI.

PlayerManager Responsibilities:

@li Maintain list of player IDs

@li Own each loaded player's file (YamlFile) and controller-level
preferences (PlayerPrefs), and decide when they are read and written

@li Create new player

@li Know who the user player is, and switch it

@par Player files

Craps::Player knows nothing about files. It only converts itself to and
from YAML. PlayerManager owns the rest:

@li The starting players are read once at startup, each from the
"Player" section of its file and the "Prefs" section for PlayerPrefs.
@li shutdown() takes a fresh snapshot of every Player and every
PlayerPrefs and writes each file. Failures are logged with LOG_ERROR;
one player failing does not stop the others. Call it once at quit, and
only after TableManager::shutdown(). The table is what shuts the
players down, and Player::shutdown() is what merges the session into
the stats. Saving before that would write the stats without this
session. Player::shutdown() is not repeatable, so PlayerManager does
not call it again itself.
@li userPrefs() is the preferences of whoever is the user player right
now. getPrefs() is the same for any player, by id.

@par The user player

The user player is the player the person at the keyboard is playing as.
It is one of the starting players until switchPlayer() makes someone else
the user. switchPlayer() never leaves things half done: if it fails, the
current user player is still at the table, still the user, and can keep
playing. It catches everything, so it never throws.

switchPlayer(toPlayerId):

@li toPlayerId must be a known player (the manifest), must not be the
current user, and must not already be at the table. A player who is not
loaded yet is read from his file first. Any of these failing changes
nothing.
@li The table swaps the two players (CrapsTable::replacePlayer()). The
user player's session ends, his bets are taken off the table and
returned to him, and the new user takes his seat, and the dice if he had
them. The View is expected to confirm with the person first if there are
bets on the table.
@li Then the player who left is written to his file. If that write fails
it is logged; the switch still stands.

A player who switched out stays loaded. If he is switched in again later
he starts a new session.

@par Player kinds

Every player in the manifest is a user player or a buddy player. User
players are the ones the person can become (switchPlayer()), rename and
delete. Buddy players come with the game: they can sit at the table
(buddyJoinsTable(), buddyLeavesTable()) but never become the user player
and are never renamed or deleted. A new player is always a user player.

@par Managing players

All of these never throw and change nothing on failure.

@li renamePlayer(): names are trimmed, 1 to MaxNameLength printable
ASCII characters, and unique ignoring case. Only the name changes; the
player id and the file name stay the same, because config.yaml and
state.yaml refer to players by file name. The manifest is saved at once.
The manifest's name is the one that counts when a player is loaded.
@li createPlayer(): makes a user player with a fresh id, an optional short
description (up to MaxDescriptionLength printable ASCII characters), a file name made
from the name (unique), and the usual starting bank. The player file and
the manifest are written at once. Does not switch to the new player.
@li deletePlayer(): user players only; not the current user and not
anyone at the table. The file is moved to the "trash" folder inside the
players folder, not erased.
@li buddyJoinsTable() and buddyLeavesTable(): a leaving buddy's bets come
off the table and are returned to him, the same as for any player who
leaves.

@par Threads

Requests arrive on the EventManager thread while the View thread reads
through CrapsReaders. The manager's own bookkeeping (who is loaded, who
is the user) is guarded by a mutex. getPlayers() hands out a reference
and so is not protected; do not call it while a switch can be running.

*/

} // namespace Ctrl

//----------------------------------------------------------------
