//----------------------------------------------------------------
//
// File: PlayerDescription.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <string>
#include <ostream>
#include <gen/Uuid.h>

namespace Ctrl {

struct PlayerDescription
{
    std::string name;
    std::string description;
    Gen::Uuid playerId;
    std::chrono::time_point<std::chrono::system_clock> lastUsed;
};

/*-----------------------------------------------------------*//**

@struct PlayerDescriptions

@brief A brief description of a Craps Player

*/

} // namespace Ctrl

std::ostream& operator<< (std::ostream& out, const Ctrl::PlayerDescription& td);

//----------------------------------------------------------------
