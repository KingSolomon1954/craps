//----------------------------------------------------------------
//
// File: ConfigFiles.h
//
//----------------------------------------------------------------

#pragma once

namespace Gen {
    class MultiLayerConfig;  // fwd
}
    
namespace Ctrl {

class ConfigFiles
{
public:
    /// @name Lifecycle
    /// @{
    static void processFiles(Gen::MultiLayerConfig* pCfg);
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
