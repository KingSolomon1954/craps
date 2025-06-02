//----------------------------------------------------------------
//
// File: CrapsGame.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class BuildInfo;  // fwd
}

namespace Craps {
    class CrapsTable; // fwd
}

namespace Ctrl {

class ConfigManager;  // fwd
class EventLoop;      // fwd
class EventManager;   // fwd
class GameController; // fwd
class PlayerManager;  // fwd
class TableManager;   // fwd
class ViewIntfc;      // fwd
    
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
    Craps::CrapsTable*     initCrapsTable();
    Ctrl::PlayerManager*   initPlayerManager();
    Ctrl::ViewIntfc*       initView();
    Ctrl::GameController*  initGameController();
    Ctrl::EventLoop*       initEventLoop();
    
    Ctrl::ViewIntfc* getView();
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
