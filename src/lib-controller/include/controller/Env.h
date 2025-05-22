//----------------------------------------------------------------
//
// File: Env.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class MultiLayerCfg;  // fwd
}
    
namespace Ctrl {

class Env
{
public:
    /// @name Lifecycle
    /// @{
    static void processEnv(Gen::MultiLayerCfg* pCfg);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class Env

@brief Process environment vars at startup

Supply class description.

*/

} // namespace Ctrl

//----------------------------------------------------------------
