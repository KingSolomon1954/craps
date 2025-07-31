//----------------------------------------------------------------
//
// File: TableManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <controller/TableManifest.h>
#include <controller/TableDescription.h>
#include <craps/CrapsTable.h>
#include <gen/ErrorPass.h>

namespace Ctrl {

class TableManager
{
public:
    
    /// @name Lifecycle
    /// @{
    TableManager();
   ~TableManager();
    /// @}
    
    using TableDescriptions = std::vector<TableManifest::TableInfo>;
    
    // Table selection
    Gen::ReturnCode switchCrapsTable(
        const Craps::CrapsTable::TableId& toTableId, Gen::ErrorPass& ep);
    const TableDescriptions& getTableChoices() const;

    // Table instantiation
    static Craps::CrapsTable* loadCrapsTable(
        const Craps::CrapsTable::TableId& tableId);
    static Craps::CrapsTable* loadStartingCrapsTable();
    
private:
    Craps::CrapsTable* pCurrentCrapsTable_ = nullptr;
    TableManifest manifest_;

    static size_t retrieveMaxSessions();
    static size_t retrieveMaxRecentRolls();
    static std::filesystem::path formTablePath(
        const Craps::CrapsTable::TableId& tableId);
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
