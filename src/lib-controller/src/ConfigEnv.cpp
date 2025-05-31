//----------------------------------------------------------------
//
// File: ConfigEnv.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigEnv.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;

/*-----------------------------------------------------------*//**

Process environment variables.

@param[in,out] pCfg
    The multi-layer configuration structure to populate.
*/
void
ConfigEnv::processEnv(const Gen::MultiLayerConfig& multiConfig,
                      Gen::ConfigLayer& cfg)
{
    (void) multiConfig;
    (void) cfg;
}

//----------------------------------------------------------------
