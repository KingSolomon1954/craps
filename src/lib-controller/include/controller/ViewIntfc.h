//----------------------------------------------------------------
//
// File: ViewIntfc.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <string>
#include <controller/TableManager.h>
#include <controller/PlayerManager.h>
#include <craps/CrapsTypes.h>

namespace Ctrl {

class ViewIntfc
{
public:
    /// @name Lifecycle
    /// @{
    virtual void run() = 0;
    /// @}

    /// @name Modifiers
    /// @{
    virtual void displayMessage(const std::string& msg) = 0;
    virtual void displayAboutCraps() = 0;
    virtual Craps::TableId promptUserToSelectTable(const TableManager::TableDescriptions& tds) = 0;
    virtual std::vector<Craps::PlayerId> promptUserToSelectPlayers(const PlayerManager::PlayerDescriptions& pds) = 0;
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ViewIntfc

@brief Base interface for controller to issue commands to view(s)

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
