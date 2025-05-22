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
    using TableId = unsigned;
    using TableDescriptions = std::vector<TableDescription>;

    static TableDescriptions loadTableChoices();
    static Craps::CrapsTable loadCrapsTable(TableId tableId);
    
private:
};

/*-----------------------------------------------------------*//**

@class TableManager

@brief Management across multiple Craps Tables.

*/

}  // namespace Ctrl

//----------------------------------------------------------------
