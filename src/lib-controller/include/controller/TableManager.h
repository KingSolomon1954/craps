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
    using TableDescriptions = std::vector<TableManifest::TableInfo>;

    /// @name Lifecycle
    /// @{
    TableManager();
   ~TableManager();
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode switchCrapsTable(
        const Craps::TableId& toTableId, Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    const TableDescriptions& getTableChoices() const;
    /// @}

private:
    Craps::CrapsTable* pCurrentCrapsTable_ = nullptr;
    TableManifest manifest_;

    Craps::CrapsTable* loadCrapsTable(const std::string& fileName);
    Craps::CrapsTable* loadStartingCrapsTable();

    static size_t retrieveMaxSessions();
    static size_t retrieveMaxRecentRolls();
    static std::filesystem::path formTablePath(const Craps::TableId& tableId);
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
