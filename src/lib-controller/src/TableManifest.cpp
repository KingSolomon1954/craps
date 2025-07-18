//----------------------------------------------------------------
//
// File: TableManifest.cpp
//
//----------------------------------------------------------------

#include <controller/TableManifest.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <gen/ErrorPass.h>

using namespace Ctrl;

//----------------------------------------------------------------

const std::vector<ManifestRecord>&
TableManifest::getManifest(Gen::ErrorPass& ep)
{
    std::vector<ManifestRecord> tmpRecords;
    // read file
    loadFile();
    // freshly populate vector
    // return to user
    return records_;
}

//----------------------------------------------------------------

void
CrapsTable::loadFile(const TableId& tableId)
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysTables).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / ("tables.yaml");

    try
    {
        checkPath(path);                      // throws
        std::ifstream fin(path);
        checkOpen(fin);                       // throws
        YAML::Node root = YAML::Load(fin);
        fromYAML(root["tables"]);             // throws
    }
    catch (const std::runtime_error& e)
    {
        std::string diag("CrapsTable::loadFile(): \"" + path.string() + "\"; ");
        throw std::runtime_error(diag + e.what());
    }
}

//-----------------------------------------------------------------

void
CrapsTable::fromYAML(const YAML::Node& node)
{
    // need for loop
    for (i     )
    {
        vector[i].fromYAML();
    }
}

//-----------------------------------------------------------------

