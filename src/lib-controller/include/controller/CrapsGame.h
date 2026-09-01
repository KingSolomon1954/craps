//----------------------------------------------------------------
//
// File: CrapsGame.h
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
    
class CrapsGame
{
public:
    /// @name Lifecycle
    /// @{
    CrapsGame(int argc, char* argv[]);
   ~CrapsGame() = default;
    void terminateApp();
    /// @}

    /// @name Modifiers
    /// @{
    static CrapsGame* instance();
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    static CrapsGame*     instancePtr_;
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
    void                  shutdownView();
    void                  enableFileLogging();
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
