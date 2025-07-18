//----------------------------------------------------------------
//
// File: MultiLayerConfigTest.cpp
//
//----------------------------------------------------------------

#include "gen/MultiLayerConfig.h"

#include <string>
#include <iostream>
#include <doctest/doctest.h>

using namespace Gen;

//----------------------------------------------------------------

TEST_CASE("MultiLayerConfig:layering")
{
    MultiLayerConfig cfg;

    Gen::ConfigLayer defaults;
    Gen::ConfigLayer files;
    Gen::ConfigLayer env;
    Gen::ConfigLayer cmdline;
    // Order matters. Lookup is reverse order
    cfg.addLayer(Gen::MultiLayerConfig::LayerDefaults, defaults);
    cfg.addLayer(Gen::MultiLayerConfig::LayerFiles, files);
    cfg.addLayer(Gen::MultiLayerConfig::LayerEnv,env);
    cfg.addLayer(Gen::MultiLayerConfig::LayerCmdLine,cmdline);
    
    const std::string& layer = Gen::MultiLayerConfig::LayerCmdLine;
    cfg.set(layer, "screenRender", "cmdline");

    CHECK(cfg.getString("screenRender").value_or("N/A") == "cmdline");
    
    // std::cout << "screenRender: " << cfg.getString("screenRender").value_or("N/A") << "\n";
    // auto finalConfig = cfg.exportResolved();
    // std::cout << "\nResolved Config:\n";
    // for (const auto& [key, value] : finalConfig)
    // {
    //     std::cout << key << " = " << value << '\n';
    // }
    // cfg.listLayers();
}

//----------------------------------------------------------------
