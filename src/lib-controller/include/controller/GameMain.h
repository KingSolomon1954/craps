//----------------------------------------------------------------
//
// File: GameMain.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/SignalHandler.h>

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
class ViewInterface;  // fwd
class UndoManager;    // fwd
    
class GameMain
{
public:
    /// @name Lifecycle
    /// @{
    GameMain(int argc, char* argv[]);
   ~GameMain() = default;
    void terminateApp();
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    static GameMain* instance();
    /// @}

private:
    static GameMain*      instancePtr_;
    Gen::SignalHandler    signalHandler_;
    
    Gen::BuildInfo*       initBuildInfo();
    Ctrl::ConfigManager*  initConfigManager(int argc, char* argv[]);
    Craps::EventManager*  initEventManager();
    Ctrl::TableManager*   initTableManager();
    Ctrl::PlayerManager*  initPlayerManager();
    Ctrl::UndoManager*    initUndoManager();
    Ctrl::ViewInterface*  initView();
    Ctrl::GameController* initGameController();
    Ctrl::EventLoop*      initEventLoop();
    
    Ctrl::ViewInterface*  getView();
    void                  setupLogging();
    void                  disableConsoleLogging();
    void                  explicitShutdown();
    void                  shutdownView();
};

/*-----------------------------------------------------------*//**

@class GameMain

@brief Craps Game Top Level class.

Responsibiities:

@li owns Globals classes
@li manages lifetime of Globals
@li initialization and shutdown
@li main thread blocking and signal handling

*/

} // namespace Ctrl

//----------------------------------------------------------------
