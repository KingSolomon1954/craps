//----------------------------------------------------------------
//
// File: MultiLayerConfig.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>

namespace Gen {

class ConfigLayer
{
public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAll() const;

private:
    std::unordered_map<std::string, std::string> values_;
};

class MultiLayerConfig
{
public:
    static const std::string LayerDefaults;
    static const std::string LayerCfgFile;
    static const std::string LayerEnv;
    static const std::string LayerCmdLine;
    
    void addLayer(const std::string& name);
    void set(const std::string& layerName, const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    std::string getOr(const std::string& key, const std::string& defaultValue) const;
    int getIntOr(const std::string& key, int defaultValue) const;

    std::unordered_map<std::string, std::string> exportResolved() const;

private:
    std::vector<std::pair<std::string, std::shared_ptr<ConfigLayer>>> layers_;
};

//----------------------------------------------------------------

}  // namespace Gen
