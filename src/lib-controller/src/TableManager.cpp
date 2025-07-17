//----------------------------------------------------------------
//
// File: TableManager.cpp
//
//----------------------------------------------------------------

#include <controller/TableManager.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>

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
    pCurrentCrapsTable_ = loadStartingCrapsTable();
    Gbl::pTable = pCurrentCrapsTable_;  // Make it accessible via globals
}

//----------------------------------------------------------------

TableManager::~TableManager()
{
    delete pCurrentCrapsTable_;
}

//----------------------------------------------------------------

TableManager::TableDescriptions
TableManager::loadTableChoices()
{
    // TODO: read directory and build up table choices
    return {
        { "Standard Table",   "a fine table",  "1", std::chrono::system_clock::now()},
        { "No 7-Out Table",   "a crazy table", "2", std::chrono::system_clock::now()},
        { "Fast-Paced Table", "giddy yap",     "3", std::chrono::system_clock::now()},
    };
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadCrapsTable(const TableId& tableId)
{
    return Craps::CrapsTable::fromFile(tableId);
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadStartingCrapsTable()
{
    TableId tid = Gbl::pConfigMgr->getString(ConfigManager::KeyTableStart).value();
    return loadCrapsTable(tid);
}

//----------------------------------------------------------------

Gen::ReturnCode
TableManager::switchCrapsTable(const TableId& toTableId, Gen::ErrorPass& ep)
{
    // TODO try/catch
    auto p = loadCrapsTable(toTableId);  // Create new CrapsTable
    Gbl::pTable->close();                // Close current table
    delete pCurrentCrapsTable_;          // Clean up its memory
    Gbl::pTable = p;                     // New table accssible in globals
    pCurrentCrapsTable_ = p;             // Manage it's memory
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
