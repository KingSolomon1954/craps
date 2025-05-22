//----------------------------------------------------------------
//
// File: ConfigFiles.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class MultiLayerCfg;  // fwd
}
    
namespace Ctrl {

class ConfigFiles
{
public:
    /// @name Lifecycle
    /// @{
    static void processFiles(Gen::MultiLayerCfg* pCfg);
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ConfigFiles

@brief Process startup configuration files

Supply class description.

*/

} // namespace Ctrl

//----------------------------------------------------------------
