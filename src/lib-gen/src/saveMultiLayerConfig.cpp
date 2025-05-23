//----------------------------------------------------------------
//
// File: MultiLayerConfig.cpp
//
//----------------------------------------------------------------

#include <gen/MultiLayerConfig.h>

using namespace Gen;

const std::string MultiLayerConfig::LayerDefaults = "defaults";
const std::string MultiLayerConfig::LayerCfgFile  = "cfgfile";
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
    return it != values_.end() ? std::optional<std::string>{it->second} : std::nullopt;
}

//----------------------------------------------------------------

const std::unordered_map<std::string, std::string>&
ConfigLayer::getAll() const
{
    return values_;
}

//----------------------------------------------------------------

void
MultiLayerConfig::addLayer(const std::string& name)
{
    layers_.emplace_back(name, std::make_shared<ConfigLayer>());
}

//----------------------------------------------------------------

void
MultiLayerConfig::set(const std::string& layerName,
                      const std::string& key,
                      const std::string& value)
{
    for (auto& [name, layer] : layers_)
    {
        if (name == layerName)
        {
            layer->set(key, value);
            return;
        }
    }
    throw std::runtime_error("Layer not found: " + layerName);
}

//----------------------------------------------------------------

std::optional<std::string>
MultiLayerConfig::get(const std::string& key) const
{
    for (const auto& [name, layer] : layers_)
    {
        if (auto val = layer->get(key); val.has_value())
        {
            return val;
        }
    }
    return std::nullopt;
}

//----------------------------------------------------------------

std::string
MultiLayerConfig::getOr(const std::string& key,
                        const std::string& defaultValue) const
{
    auto val = get(key);
    return val.has_value() ? val.value() : defaultValue;
}

//----------------------------------------------------------------

int
MultiLayerConfig::getIntOr(const std::string& key,
                           int defaultValue) const
{
    auto val = get(key);
    return val.has_value() ? std::stoi(val.value()) : defaultValue;
}

//----------------------------------------------------------------

std::unordered_map<std::string, std::string>
MultiLayerConfig::exportResolved() const
{
    std::unordered_map<std::string, std::string> result;
    for (const auto& [_, layer] : layers_)
    {
        for (const auto& [key, value] : layer->getAll())
        {
            result.try_emplace(key, value);
        }
    }
    return result;
}

//----------------------------------------------------------------

#if 0

int main() {
    MultiLayerConfig cfg;

    ConfigLayer defaults;
    defaults.set("port", "8080");
    defaults.set("debug", "false");

    ConfigLayer env;
    env.set("port", "8500");

    ConfigLayer cli;

    cfg.addLayer("default", defaults);
    cfg.addLayer("env", env);
    cfg.addLayer("cli", cli);

    cfg.set("cli", "port", "9000");  // overrides all previous
    cfg.set("cli", "loglevel", "verbose");

    cfg.listLayers();

    std::cout << "Port: " << cfg.getInt("port").value_or(-1) << "\n";
    std::cout << "Debug: " << cfg.getString("debug").value_or("N/A") << "\n";
    std::cout << "Log Level: " << cfg.getString("loglevel").value_or("default") << "\n";
}

#endif
