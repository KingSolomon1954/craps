//----------------------------------------------------------------
//
// File: TableManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <controller/TableDescription.h>
#include <craps/CrapsTable.h>

namespace Ctrl {

class TableManager
{
public:
    TableManager();
    using TableId = std::string;
    using TableDescriptions = std::vector<TableDescription>;

    static TableDescriptions loadTableChoices();
    static Craps::CrapsTable* loadCrapsTable(const TableId& tableId);
    static Craps::CrapsTable* loadStartingCrapsTable();
    
private:
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
