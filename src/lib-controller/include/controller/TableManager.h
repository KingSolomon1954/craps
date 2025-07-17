//----------------------------------------------------------------
//
// File: TableManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <controller/TableDescription.h>
#include <craps/CrapsTable.h>
#include <gen/ErrorPass.h>

namespace Ctrl {

class TableManager
{
public:
    TableManager();
   ~TableManager();
    using TableId = std::string;
    using TableDescriptions = std::vector<TableDescription>;

    Gen::ReturnCode switchCrapsTable(const TableId& toTableId, Gen::ErrorPass& ep);
    
    static TableDescriptions  loadTableChoices();
    static Craps::CrapsTable* loadCrapsTable(const TableId& tableId);
    static Craps::CrapsTable* loadStartingCrapsTable();
    
private:
    Craps::CrapsTable* pCurrentCrapsTable_ = nullptr;
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
