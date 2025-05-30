//----------------------------------------------------------------
//
// File: TableManager.cpp
//
//----------------------------------------------------------------

#include <controller/TableManager.h>
#include <controller/ConfigManager.h>

using namespace Ctrl;

//----------------------------------------------------------------

TableManager::TableManager()
{
    // init with last table played
    // loadFromFile(tid);
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
    TableId tid = Gbl::pConfigMgr->getString(ConfigManager::KeyStartTable).value();
    return loadCrapsTable(tid);
}

//----------------------------------------------------------------
