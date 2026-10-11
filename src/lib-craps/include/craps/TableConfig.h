//----------------------------------------------------------------
//
// File: TableConfig.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>

namespace Craps {

struct TableConfig
{
    size_t maxSessions = 0;
    size_t maxRecentRolls = 0;
    std::filesystem::path tablePath;
};

/*-----------------------------------------------------------*//**

@struct TableConfig

@brief Configuration parameters for CrapsTable at construction time

The CrapsTable class requires this config structure as an argument at
construction time. It supplies various configuration parameters thereby
eliminating any dependencies on the lib-controller, in this case the
ConfigManager.

*/

} // namespace Craps

// std::ostream& operator<< (std::ostream& out, const Craps::TableConfig& tc);

//----------------------------------------------------------------
