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

    /// @name Lifecycle
    /// @{
    PlayerManager();
   ~PlayerManager();
    /// @}

    /// @name Modifiers
    /// @{
    void shutdown();
    Gen::ReturnCode switchPlayer(const Craps::PlayerId& toPlayerId,
                                 Gen::ErrorPass& ep);
    PlayerPrefs& userPrefs();
    PlayerPrefs* getPrefs(const Craps::PlayerId& playerId);
    /// @}

    /// @name Observers
    /// @{
    const PlayerPrefs& userPrefs()                    const;
    const PlayerManifest::PlayerList& getPlayerList() const;
    const Players& getPlayers()                       const;
    Craps::Player* getUserPlayer()                    const;
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

@par Threads

Requests arrive on the EventManager thread while the View thread reads
through CrapsReaders. The manager's own bookkeeping (who is loaded, who
is the user) is guarded by a mutex. getPlayers() hands out a reference
and so is not protected; do not call it while a switch can be running.

*/

} // namespace Ctrl

//----------------------------------------------------------------
