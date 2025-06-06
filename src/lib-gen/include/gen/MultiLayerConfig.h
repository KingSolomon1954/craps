//----------------------------------------------------------------
//
// File: MultiLayerConfig.h
//
//----------------------------------------------------------------

#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

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
    static const std::string LayerFiles;
    static const std::string LayerEnv;
    static const std::string LayerCmdLine;

    void addLayer(const std::string& name, const ConfigLayer& layer);
    ConfigLayer& getLayer(const std::string& layerName);
    void set(const std::string& layerName,
             const std::string& key,
             const std::string& value);
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<int>         getInt   (const std::string& key) const;
    std::optional<bool>        getBool  (const std::string& key) const;
    void listLayers() const;
    std::unordered_map<std::string, std::string> exportResolved() const;

private:
    mutable std::mutex mutex_;
    std::vector<std::pair<std::string, std::shared_ptr<ConfigLayer>>> layers_;
    std::unordered_map<std::string, size_t> nameToIndex_;
};

//----------------------------------------------------------------

}  // namespace Gen
