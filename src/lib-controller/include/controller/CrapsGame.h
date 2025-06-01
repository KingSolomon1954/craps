//----------------------------------------------------------------
//
// File: CrapsGame.h
//
//----------------------------------------------------------------

#pragma once
#include <memory>
#include <string>
#include <controller/ViewIntfc.h>

namespace Gen {
    class BuildInfo;         // fwd
}

namespace Craps {
    class CrapsTable; // fwd
}

namespace Ctrl {

class ConfigManager;  // fwd
class EventManager;   // fwd
class PlayerManager;  // fwd
class TableManager;   // fwd
    
class CrapsGame
{
public:
    /// @name Lifecycle
    /// @{
    CrapsGame(int argc, char* argv[]);
   ~CrapsGame() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    Gen::BuildInfo*        initBuildInfo();
    Ctrl::ConfigManager*   initConfigManager(int argc, char* argv[]);
    Ctrl::EventManager*    initEventManager();
    Ctrl::TableManager*    initTableManager();
    Ctrl::PlayerManager*   initPlayerManager();
    Craps::CrapsTable*     initCrapsTable();
    std::shared_ptr<ViewIntfc> getView();
    
};

/*-----------------------------------------------------------*//**

@class CrapsGame

@brief Craps Game Top Level class.

Responsibiities:

* initialize Configuration
* initializes Globals

*/

} // namespace Ctrl

//----------------------------------------------------------------
