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
    
    manifest_.loadFromFile();  // Load tables.yaml manifest
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
TableManager::loadCrapsTable(const Craps::CrapsTable::TableId& tableId)
{
    return Craps::CrapsTable::fromFile(tableId);
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadStartingCrapsTable()
{
    Craps::CrapsTable::TableId tid =
        Gbl::pConfigMgr->getString(ConfigManager::KeyTableStartId).value();
    return loadCrapsTable(tid);
}

//----------------------------------------------------------------

Gen::ReturnCode
TableManager::switchCrapsTable(
    const Craps::CrapsTable::TableId& toTableId, Gen::ErrorPass& ep)
{
    try
    {
        auto p = loadCrapsTable(toTableId);  // Create new CrapsTable, throws
        Gbl::pTable->close();                // Close current table
        delete pCurrentCrapsTable_;          // Clean up its memory
        Gbl::pTable = p;                     // New table accssible in globals
        pCurrentCrapsTable_ = p;             // Manage it's memory
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
