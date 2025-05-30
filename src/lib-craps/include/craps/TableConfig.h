//----------------------------------------------------------------
//
// File: TableConfig.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
#include <string>
#include <controller/Globals.h>
#include <craps/CrapsTable.h>

namespace Craps {

struct TableConfig
{
    Craps::CrapsTable::TableId tableId;
    std::string tableName;
    Gbl::Money houseBank;
};

/*-----------------------------------------------------------*//**

@struct TableConfig

@brief In-memory configuration to construct a CrapsTable

*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::TableConfig& tc);

//----------------------------------------------------------------
