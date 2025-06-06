//----------------------------------------------------------------
//
// File: MultiLayerConfig.cpp
//
//----------------------------------------------------------------

#include <gen/MultiLayerConfig.h>
#include <algorithm>

using namespace Gen;

const std::string MultiLayerConfig::LayerDefaults = "defaults";
const std::string MultiLayerConfig::LayerFiles    = "files";
const std::string MultiLayerConfig::LayerEnv      = "env";
const std::string MultiLayerConfig::LayerCmdLine  = "cmdline";

//----------------------------------------------------------------

void
ConfigLayer::set(const std::string& key, const std::string& value)
{
    values_[key] = value;
}

//----------------------------------------------------------------

std::optional<std::string>
ConfigLayer::get(const std::string& key) const
{
    auto it = values_.find(key);
    return it != values_.end() ? std::make_optional(it->second) : std::nullopt;
}

//----------------------------------------------------------------

const std::unordered_map<std::string, std::string>&
ConfigLayer::getAll() const
{
    return values_;
}

//----------------------------------------------------------------

void
MultiLayerConfig::addLayer(const std::string& name, const ConfigLayer& layer)
{
    auto ptr = std::make_shared<ConfigLayer>(layer);
    layers_.emplace_back(name, ptr);
    nameToIndex_[name] = layers_.size() - 1;
}

//----------------------------------------------------------------

ConfigLayer&
MultiLayerConfig::getLayer(const std::string& layerName)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = nameToIndex_.find(layerName);
    if (it == nameToIndex_.end()) {
        throw std::runtime_error("Layer not found: " + layerName);
    }

    const auto& layerPair = layers_.at(it->second);
    return *(layerPair.second); // Dereference shared_ptr to get reference
}

//----------------------------------------------------------------

void
MultiLayerConfig::set(const std::string& layerName,
                      const std::string& key,
                      const std::string& value)
{
    auto it = nameToIndex_.find(layerName);
    if (it != nameToIndex_.end())
    {
        layers_[it->second].second->set(key, value);
    }
    else
    {
        std::cerr << "Layer \"" << layerName << "\" not found.\n";
    }
}

//----------------------------------------------------------------

std::optional<std::string>
MultiLayerConfig::getString(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it)
    {
        if (auto val = it->second->get(key); val.has_value())
        {
            return val;
        }
    }
    return std::nullopt;
}

//----------------------------------------------------------------

std::optional<int>
MultiLayerConfig::getInt(const std::string& key) const
{
    if (auto val = getString(key); val.has_value())
    {
        try {
            return std::stoi(*val);
        } catch (...) {}
    }
    return std::nullopt;
}

//----------------------------------------------------------------

std::optional<bool>
MultiLayerConfig::getBool(const std::string& key) const
{
    if (auto val = getString(key); val.has_value())
    {
        std::string lower;
        lower.resize(val->size());
        std::transform(val->begin(), val->end(), lower.begin(), ::tolower);

        if (lower == "true" || lower == "1" ||
            lower == "yes"  || lower == "on")
        {
            return true;
        }
        else if (lower == "false" || lower == "0" ||
                 lower == "no"    || lower == "off")
        {
            return false;
        }
    }
    return std::nullopt;
}

//----------------------------------------------------------------

void
MultiLayerConfig::listLayers() const
{
    std::cout << "Layers (lowest to highest priority):\n";
    for (const auto& [name, _] : layers_)
    {
        std::cout << "- " << name << '\n';
    }
}

//----------------------------------------------------------------

std::unordered_map<std::string, std::string>
MultiLayerConfig::exportResolved() const
{
    std::unordered_map<std::string, std::string> result;

    std::lock_guard<std::mutex> lock(mutex_);  // Make sure to protect access if used concurrently

    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it)
    {
        const auto& layer = it->second;
        for (const auto& [key, value] : layer->getAll())
        {
            result.try_emplace(key, value);  // Lower layers won't overwrite higher-priority values
        }
    }

    return result;
}

//----------------------------------------------------------------
