//----------------------------------------------------------------
//
// File: TableManager.cpp
//
//----------------------------------------------------------------

#include <filesystem>
#include <controller/TableManager.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <craps/CrapsTable.h>
#include <craps/EventManager.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Design note:
//
// Private pCurrentCrapsTable_ is used to manage the lifetime of the
// current global CrapsTable. This differs from how CrapsGame class
// manages lifetime of all the other globals. Unlike the other
// globals, which remain for the life of the program, the CrapsTable
// global needs the ability to be swapped out for a different one
// if/when user selects different table at runtime.
//
TableManager::TableManager()
{
    manifest_.loadFromFile();           // Load tables.yaml manifest
    pCurrentCrapsTable_ = loadStartingCrapsTable();
    Gbl::pTable = pCurrentCrapsTable_;  // Make it accessible via globals
    tables_.push_back(pCurrentCrapsTable_);
}

//----------------------------------------------------------------

TableManager::~TableManager()
{
    delete pCurrentCrapsTable_;
}

//----------------------------------------------------------------

const TableManager::TableDescriptions&
TableManager::getTableChoices() const
{
    return manifest_.getTables();
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::getTable(const Craps::TableId& tableId,
                       Gen::ErrorPass& ep) const
{
    for (auto t : tables_)
    {
        if (t->getTableId() == tableId)
        {
            return t;
        }
    }
    ep.diag = "TableManager::getTable(): no such tableId; \"" +
        tableId + "\"";
    return nullptr;
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadCrapsTable(const std::string& fileName)
{
    auto tableId = manifest_.getTableId(fileName);
    if (tableId.empty())
    {
        std::string diag = "TableManager::loadCrapsTable(): unable to load "
            "table file; lookup failed; file name:\"" + fileName +
            "\" is not found in manifest file:\"tables.yaml\".";
        throw std::runtime_error(diag);
    }

    Craps::TableConfig config;
    config.maxSessions    = TableManager::retrieveMaxSessions();
    config.maxRecentRolls = TableManager::retrieveMaxRecentRolls();
    config.tablePath      = TableManager::formTablePath(fileName);

    return Craps::CrapsTable::fromFile(tableId, config, *Gbl::pEventMgr);
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadStartingCrapsTable()
{
    std::string fileName =
        Gbl::pConfigMgr->getString(ConfigManager::KeyTableStart).value();
    return loadCrapsTable(fileName);
}

//----------------------------------------------------------------

Gen::ReturnCode
TableManager::switchCrapsTable(
    const Craps::TableId& toTableId, Gen::ErrorPass& ep)
{
    try
    {
        auto p = loadCrapsTable(toTableId);  // Create new CrapsTable, throws
        Gbl::pTable->close();                // Close current table
        delete pCurrentCrapsTable_;          // Clean up its memory
        Gbl::pTable = p;                     // New table accssible in globals
        pCurrentCrapsTable_ = p;             // Manage it's memory
        tables_[0] = p;                       // replace table in container
        return Gen::ReturnCode::Success;
    }
    catch (const std::runtime_error& e)
    {
        ep.diag = "TableManager::switchCrapsTable(): ";
        ep.append(e.what());
        return Gen::ReturnCode::Fail;
    }
}

//----------------------------------------------------------------

size_t
TableManager::retrieveMaxSessions()
{
    return Gbl::pConfigMgr->getInt(
        Ctrl::ConfigManager::KeyTableMaxSessions).value();
}

//----------------------------------------------------------------

size_t
TableManager::retrieveMaxRecentRolls()
{
    return Gbl::pConfigMgr->getInt(
        Ctrl::ConfigManager::KeyTableMaxRecentRolls).value();
}

//----------------------------------------------------------------

std::filesystem::path
TableManager::formTablePath(const std::string& fileName)
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysTables).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / fileName;
    return path;
}

//----------------------------------------------------------------
