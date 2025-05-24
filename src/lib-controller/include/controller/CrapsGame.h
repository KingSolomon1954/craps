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
    class MultiLayerConfig;  // fwd
}

namespace Craps {
    class CrapsTable; // fwd
}

namespace Ctrl {

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
    Gen::MultiLayerConfig* initCfg(int argc, char* argv[]);
    Ctrl::EventManager*    initEventManager();
    Ctrl::TableManager*    initTableManager();
    Ctrl::PlayerManager*   initPlayerManager();
    Craps::CrapsTable*     initCrapsTable();
    Gen::MultiLayerConfig* initMultiLayerConfig(int argc, char* argv[]);
    void                   populateLayerDefaults(Gen::MultiLayerConfig* pCfg);
    void                   populateLayerFiles(Gen::MultiLayerConfig* pCfg);
    void                   populateLayerEnv(Gen::MultiLayerConfig* pCfg);
    void                   populateLayerCmdLine(int argc, char* argv[], Gen::MultiLayerConfig* pCfg);
    void                   dumpConfig(Gen::MultiLayerConfig* pCfg);
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
