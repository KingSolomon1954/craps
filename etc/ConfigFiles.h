#pragma once

namespace Gen {
    class ConfigLayer;  // fwd
    class MultiLayerConfig;  // fwd
}

namespace Ctrl {

class ConfigFiles
{
public:
    static void processFiles(const Gen::MultiLayerConfig& multiConfig, Gen::ConfigLayer& cfg);
    static void saveGameState(const Gen::ConfigLayer& cfg, const std::string& state, const std::string& filename);
    static std::string loadGameState(const Gen::ConfigLayer& cfg, const std::string& filename);
    static void saveHighScore(const Gen::ConfigLayer& cfg, const std::string& player, int score);
};

} // namespace Ctrl