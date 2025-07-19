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
    
    Gen::ReturnCode switchCrapsTable(
        const Craps::CrapsTable::TableId& toTableId, Gen::ErrorPass& ep);
    
    const TableDescriptions& getTableChoices() const;
    
    static Craps::CrapsTable* loadCrapsTable(
        const Craps::CrapsTable::TableId& tableId);
    
    static Craps::CrapsTable* loadStartingCrapsTable();
    
private:
    Craps::CrapsTable* pCurrentCrapsTable_ = nullptr;
    TableManifest manifest_;
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
