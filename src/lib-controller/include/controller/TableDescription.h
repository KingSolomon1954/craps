//----------------------------------------------------------------
//
// File: TableDescription.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <string>
#include <ostream>

namespace Ctrl {

struct TableDescription
{
    std::string name;
    std::string description;
    unsigned tableId;
    std::chrono::time_point<std::chrono::system_clock> lastUsed;
};

/*-----------------------------------------------------------*//**

@struct TableDescriptions

@brief A brief description of a Craps Table

*/

} // namespace Ctrl

std::ostream& operator<< (std::ostream& out, const Ctrl::TableDescription& td);

//----------------------------------------------------------------
