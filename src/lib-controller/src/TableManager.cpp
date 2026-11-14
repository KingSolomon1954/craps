//----------------------------------------------------------------
//
// File: TableManager.cpp
//
//----------------------------------------------------------------

#include <filesystem>
#include <controller/TableManager.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <gen/Logger.h>
#include <memory>
#include <stdexcept>

using namespace Ctrl;

namespace {

// Top-level section of a table file
constexpr const char* KeyTable = "Table";

} // namespace

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
    pCurrentCrapsTable_ = loadStartingCrapsTable(pCurrentFile_);
    Gbl::pTable = pCurrentCrapsTable_;  // Make it accessible via globals
    tables_.push_back(pCurrentCrapsTable_);
}

//----------------------------------------------------------------

TableManager::~TableManager()
{
    delete pCurrentCrapsTable_;
}

//----------------------------------------------------------------

const TableManifest::TableList&
TableManager::getTableList() const
{
    return manifest_.getTableList();
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
    ep.setDescription("TableManager::getTable(): no such tableId; \"" +
                      tableId + "\"");
    return nullptr;
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadCrapsTable(const std::string& fileName,
                             std::unique_ptr<YamlFile>& file)
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

    auto newFile = std::make_unique<YamlFile>(
        TableManager::formTablePath(fileName));
    newFile->load(KeyTable);                                   // throws

    std::unique_ptr<Craps::CrapsTable> table(
        new Craps::CrapsTable(tableId, config));
    try
    {
        table->fromYAML(newFile->getSection(KeyTable));        // throws
    }
    catch (const std::runtime_error& e)
    {
        throw std::runtime_error("TableManager::loadCrapsTable(): \"" +
            newFile->getPath().string() + "\"; " + e.what());
    }

    file = std::move(newFile);
    return table.release();
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadStartingCrapsTable(std::unique_ptr<YamlFile>& file)
{
    std::string fileName =
        Gbl::pConfigMgr->getString(ConfigManager::KeyTableStart).value();
    return loadCrapsTable(fileName, file);
}

//----------------------------------------------------------------
//
// Called once at quit. The table shuts down its players too, so this
// must come before PlayerManager::shutdown().
//
void
TableManager::shutdown()
{
    pCurrentCrapsTable_->shutdown();   // Players and table; merges stats
    saveCurrentTable();
}

//----------------------------------------------------------------
//
// Writes the current table's file from a fresh snapshot of the table.
// A failure is logged.
//
void
TableManager::saveCurrentTable() const
{
    try
    {
        pCurrentFile_->setSection(KeyTable, pCurrentCrapsTable_->toYAML());
        pCurrentFile_->save();                                 // throws
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(std::string("TableManager::saveCurrentTable(): ") + e.what());
    }
}

//----------------------------------------------------------------

Gen::ReturnCode
TableManager::switchCrapsTable(
    const Craps::TableId& toTableId, Gen::ErrorPass& ep)
{
    try
    {
        std::unique_ptr<YamlFile> newFile;
        auto p = loadCrapsTable(toTableId, newFile);  // New table, throws
        Gbl::pTable->close();                // Close current table
        saveCurrentTable();                  // Save the table we are leaving.
                                             // A failure is logged and must
                                             // not block the switch.
        delete pCurrentCrapsTable_;          // Clean up its memory
        Gbl::pTable = p;                     // New table accssible in globals
        pCurrentCrapsTable_ = p;             // Manage it's memory
        pCurrentFile_ = std::move(newFile);  // and its file
        tables_[0] = p;                      // replace table in container
        UndoManager::instance().clear();     // Start with zero undo's
        return Gen::ReturnCode::Success;
    }
    catch (const std::runtime_error& e)
    {
        EP_SET(ep, Gen::ErrorPass::ErrorName::CaughtException,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate, 
                   "TableManager::switchCrapsTable(): ");
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
