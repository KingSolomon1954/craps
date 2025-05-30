//----------------------------------------------------------------
//
// File: TableConfig.cpp
//
//----------------------------------------------------------------

#include <craps/TableConfig.h>
// #include <yaml-cpp/yaml.h>

using namespace Craps;

TableConfig
TableConfig::loadTableConfigFromYamlFile(
    const std::string& filepath)
{
#if 0    
    YAML::Node root = YAML::LoadFile(filepath);
    TableConfig config;

    const auto& table = root["table"];
    config.name = table["name"].as<std::string>();
    config.maxPlayers = table["max_players"].as<int>();
    for (const auto& rule : table["rules"]) {
        config.rules.push_back(rule.as<std::string>());
    }
#endif
    // TODO    
    TableConfig config;
    return config;
}

//----------------------------------------------------------------

std::ostream&
operator<< (std::ostream& out, const TableConfig& tc)
{
    out <<
    "       tableId: " << tc.tableId   << std::endl <<
    "     tableName: " << tc.tableName << std::endl <<
    "     houseBank: " << tc.houseBank << std::endl;
    return out;
}

//----------------------------------------------------------------

