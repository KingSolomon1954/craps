//----------------------------------------------------------------
//
// File: PlayerConfig.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>

namespace Craps {

struct PlayerConfig
{
    std::filesystem::path playerPath;
};

/*-----------------------------------------------------------*//**

@struct PlayerConfig

@brief Configuration parameters for Players at construction time

The Player class requires this config structure as an argument at
construction time. It supplies various configuration parameters thereby
eliminating any dependencies on the lib-controller, in this case the
PlayerManager.

*/

} // namespace Craps

// std::ostream& operator<< (std::ostream& out, const Craps::TableConfig& tc);

//----------------------------------------------------------------
