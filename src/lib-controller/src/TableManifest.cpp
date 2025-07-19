//----------------------------------------------------------------
//
// File: TableManifest.cpp
//
//----------------------------------------------------------------

#include <controller/TableManifest.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <gen/ErrorPass.h>
#include <gen/FileUtils.h>
#include <fstream>

using namespace Ctrl;

//----------------------------------------------------------------

const std::vector<TableManifest::TableInfo>&
TableManifest::getTables() const
{
    return tables_;
}

//----------------------------------------------------------------

void
TableManifest::loadFromFile()
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysTables).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / ("tables.yaml");

    try
    {
        std::ifstream fin = Gen::FileUtils::openOrThrow(path);
        YAML::Node root = YAML::Load(fin);
        fromYAML(root["tables"]);
    }
    catch (const std::runtime_error& e)
    {
        std::string diag("TableManifest::loadFromFile(): \"" + path.string() + "\"; ");
        throw std::runtime_error(diag + e.what());
    }
}

//-----------------------------------------------------------------

void
TableManifest::fromYAML(const YAML::Node& node)
{
    tables_.clear();

    if (!node || !node.IsSequence())
    {
        throw std::runtime_error("TableManifest::fromYAML(): expected a sequence node");
    }

    for (const auto& tableNode : node)
    {
        TableInfo info;
        info.fromYAML(tableNode);
        tables_.emplace_back(std::move(info));
    }
}

//-----------------------------------------------------------------

void
TableManifest::TableInfo::fromYAML(const YAML::Node& node)
{
    tableId          = node["tableId"].as<Craps::CrapsTable::TableId>();
    tableName        = node["tableName"].as<std::string>();
    shortDescription = node["shortDescription"].as<std::string>();
    fullDescription  = node["fullDescription"].as<std::string>();
    fileName         = node["fileName"].as<std::string>();
}

//-----------------------------------------------------------------

YAML::Node
TableManifest::TableInfo::toYAML() const
{
    YAML::Node node;
    node["tableId"]          = tableId;
    node["tableName"]        = tableName;
    node["shortDescription"] = shortDescription;
    node["fullDescription"]  = fullDescription;
    node["fileName"]         = fileName;
    return node;
}

//-----------------------------------------------------------------
