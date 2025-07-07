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
#include <craps/Bank.h>

namespace Craps {

struct TableConfig
{
    Craps::CrapsTable::TableId tableId;
    std::string tableName;
    
//    TableConfig(const TableConfig&) = default;  // copy ctor explicit request
//    TableConfig& operator=(const TableConfig&) = default;
//    TableConfig& operator=(TableConfig&&) = default;
    static TableConfig loadTableConfigFromYamlFile(const std::string& filepath);
};

/*-----------------------------------------------------------*//**

@struct TableConfig

@brief In-memory configuration to construct a CrapsTable

*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::TableConfig& tc);

//----------------------------------------------------------------
