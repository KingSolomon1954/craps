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
    class CrapsTable;   // fwd
    class EventManager; // fwd
}

namespace Ctrl {

class ConfigManager;  // fwd
class EventLoop;      // fwd
class GameController; // fwd
class PlayerManager;  // fwd
class TableManager;   // fwd
class ViewIntfc;      // fwd
class UndoManager;    // fwd
    
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
    Craps::EventManager*   initEventManager();
    Ctrl::TableManager*    initTableManager();
    Ctrl::PlayerManager*   initPlayerManager();
    Ctrl::UndoManager*     initUndoManager();
    Ctrl::ViewIntfc*       initView();
    Ctrl::GameController*  initGameController();
    Ctrl::EventLoop*       initEventLoop();
    
    Ctrl::ViewIntfc* getView();
    void enableFileLogging();
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
