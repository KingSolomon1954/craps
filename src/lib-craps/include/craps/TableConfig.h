//----------------------------------------------------------------
//
// File: TableConfig.h
//
//----------------------------------------------------------------

#pragma once

namespace Craps {

struct TableConfig
{
    size_t maxSessions = 0;
    size_t maxRecentRolls = 0;
    // Directory location of craps table YAML file.
};

/*-----------------------------------------------------------*//**

@struct TableConfig

@brief Configuration parameters for CrapsTable at construction time

*/

} // namespace Craps

// std::ostream& operator<< (std::ostream& out, const Craps::TableConfig& tc);

//----------------------------------------------------------------
