//----------------------------------------------------------------
//
// File: TableDescription.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <string>
#include <ostream>
#include <craps/CrapsTable.h>

namespace Ctrl {

struct TableDescription
{
    Craps::CrapsTable::TableId tableId;
    std::string tableName;
    std::string shortDescription;
    std::string fullDescription;
};

/*-----------------------------------------------------------*//**

@struct TableDescriptions

@brief description of a Craps Table

*/

} // namespace Ctrl

std::ostream& operator<< (std::ostream& out, const Ctrl::TableDescription& td);

//----------------------------------------------------------------
