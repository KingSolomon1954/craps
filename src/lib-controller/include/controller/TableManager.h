//----------------------------------------------------------------
//
// File: TableManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <filesystem>
#include <vector>
#include <controller/TableManifest.h>
#include <controller/TableDescription.h>
#include <controller/YamlFile.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

namespace Craps {
    class CrapsTable;  // fwd
}

namespace Ctrl {

class TableManager
{
public:
    using Tables = std::vector<Craps::CrapsTable*>;

    /// @name Lifecycle
    /// @{
    TableManager();
   ~TableManager();
    /// @}

    /// @name Modifiers
    /// @{
    void shutdown();

    /// What a switch did, for the answer to the View.
    struct SwitchResult
    {
        Craps::TableId fromTableId;
        Craps::TableId toTableId;
        std::string    toTableName;
        unsigned       buddiesFollowed = 0;   ///< Now seated at the new table
        unsigned       buddiesLeft     = 0;   ///< Left the game
        unsigned       betsReturned    = 0;   ///< Came down, money returned
    };
    Gen::ReturnCode switchCrapsTable(
        const Craps::TableId& toTableId, bool buddiesFollow,
        SwitchResult& result, Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    const TableManifest::TableList& getTableList() const;
    std::string getCurrentFileName() const;
    Craps::CrapsTable* getTable(const Craps::TableId& tableId,
                                Gen::ErrorPass& ep) const;
    /// @}

private:
    Craps::CrapsTable* pCurrentCrapsTable_ = nullptr;
    std::unique_ptr<YamlFile> pCurrentFile_;   // file of the current table
    TableManifest manifest_;
    Tables tables_;
    /// Tables the game has left. Kept, not deleted, until the game ends,
    /// because the View may still be reading one as the switch happens.
    /// They hold no players and no bets.
    std::vector<std::unique_ptr<Craps::CrapsTable>> retired_;

    Craps::CrapsTable* loadCrapsTable(const std::string& fileName,
                                      std::unique_ptr<YamlFile>& file);
    Craps::CrapsTable* loadStartingCrapsTable(std::unique_ptr<YamlFile>& file);
    void saveCurrentTable() const;

    static size_t retrieveMaxSessions();
    /// How many recent rolls a table remembers. Not a user setting.
    /// Matches what the View's Deep Roll History can show.
    static constexpr size_t MaxRecentRolls = 120;
    static std::filesystem::path formTablePath(const Craps::TableId& tableId);
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

@par Table files

Craps::CrapsTable knows nothing about files. It only converts itself to
and from YAML. TableManager owns the rest, the same way PlayerManager
does for players:

@li loadCrapsTable() reads the table's file once, keeps it as a YamlFile
and builds the CrapsTable from its "Table" section.
@li shutdown() shuts the current table down, which in turn shuts down
all of its players and merges the session into their stats and the
table's. It then takes a fresh snapshot of the table and writes its
file. Failures are logged with LOG_ERROR. Call it once at quit, and
before PlayerManager::shutdown(), which saves the players afterward.
@li switchCrapsTable() moves the user player, and the buddies who follow,
to another table. See below.

@par Switching tables

switchCrapsTable() loads the new table first, so a table file that cannot
be read leaves everything as it was. After that it does not fail:

@li Betting must be open. Every seated player ends the session at the old
table: bets come down, money returns to the player, stats merge.
@li The old table is closed, which records its session, and saved.
@li The new table becomes the active one (Gbl::pTable) and the user player,
then each buddy who follows, joins it, in the seating order they had. The
user is the shooter. Buddies who do not follow are saved and are out of
the game until someone seats them.
@li The old table object is kept in retired_ rather than deleted. Another
thread may be reading the table as the switch happens, and a retired
table is a closed, empty one, which is safe to read.
@li Undo history is cleared, and AutoOdds forgets the bets it had handled.
@li The players' files are saved.

The caller, EventHandler, answers the View and sends the events that tell
it the table changed.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
