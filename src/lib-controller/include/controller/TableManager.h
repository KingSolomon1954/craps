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
    Gen::ReturnCode switchCrapsTable(
        const Craps::TableId& toTableId, Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    const TableManifest::TableList& getTableList() const;
    Craps::CrapsTable* getTable(const Craps::TableId& tableId,
                                Gen::ErrorPass& ep) const;
    /// @}

private:
    Craps::CrapsTable* pCurrentCrapsTable_ = nullptr;
    std::unique_ptr<YamlFile> pCurrentFile_;   // file of the current table
    TableManifest manifest_;
    Tables tables_;

    Craps::CrapsTable* loadCrapsTable(const std::string& fileName,
                                      std::unique_ptr<YamlFile>& file);
    Craps::CrapsTable* loadStartingCrapsTable(std::unique_ptr<YamlFile>& file);
    void saveCurrentTable() const;

    static size_t retrieveMaxSessions();
    static size_t retrieveMaxRecentRolls();
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
@li switchCrapsTable() saves the table it is leaving, after closing it.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
