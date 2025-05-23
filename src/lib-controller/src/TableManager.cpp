//----------------------------------------------------------------
//
// File: TableManager.cpp
//
//----------------------------------------------------------------

#include <controller/TableManager.h>

using namespace Ctrl;

//----------------------------------------------------------------

TableManager::TableDescriptions
TableManager::loadTableChoices()
{
    // TODO: read directory and build up table choices
    return {
        { "Standard Table",   "a fine table",  1, std::chrono::system_clock::now()},
        { "No 7-Out Table",   "a crazy table", 2, std::chrono::system_clock::now()},
        { "Fast-Paced Table", "giddy yap",     3, std::chrono::system_clock::now()},
    };
}

//----------------------------------------------------------------

Craps::CrapsTable*
TableManager::loadCrapsTable(unsigned tableId)
{
    // TODO: read from file and create table
    return new Craps::CrapsTable();
}

//----------------------------------------------------------------
