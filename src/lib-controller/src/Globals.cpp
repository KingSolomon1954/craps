//----------------------------------------------------------------
//
// File: Globals.cpp
//
//----------------------------------------------------------------

#include <controller/Globals.h>
#include <controller/ConfigManager.h>
#include <controller/EventHandler.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <gen/BuildInfo.h>
#include <gen/EventManager.h>

Gen::BuildInfo*        Gbl::pBuildInfo   = nullptr;
Ctrl::ConfigManager*   Gbl::pConfigMgr    = nullptr;
Ctrl::EventHandler*    Gbl::pEventHandler = nullptr;
Craps::CrapsTable*     Gbl::pTable        = nullptr;
Ctrl::TableManager*    Gbl::pTableMgr     = nullptr;
Ctrl::PlayerManager*   Gbl::pPlayerMgr    = nullptr;
Ctrl::UndoManager*     Gbl::pUndoMgr      = nullptr;

const std::string Gbl::appNameScreen = "Royal Craps";
const std::string Gbl::appNameExec   = "royalcraps";

//----------------------------------------------------------------

