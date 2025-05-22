//----------------------------------------------------------------
//
// File: CrapsGame.h
//
//----------------------------------------------------------------

#pragma once
#include <string>

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
    Ctrl::PlayerManager*   initPlayerManager();
    Craps::CrapsTable*     initCrapsTable();
    void dumpConfig(Gen::MultiLayerConfig* pCfg);
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
