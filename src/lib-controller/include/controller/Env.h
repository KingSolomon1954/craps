//----------------------------------------------------------------
//
// File: Env.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class MultiLayerConfig;  // fwd
}
    
namespace Ctrl {

class Env
{
public:
    /// @name Lifecycle
    /// @{
    static void processEnv(Gen::MultiLayerConfig* pCfg);
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
