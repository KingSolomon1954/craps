//----------------------------------------------------------------
//
// File: Globals.cpp
//
//----------------------------------------------------------------

#include <controller/Globals.h>
#include <controller/ConfigManager.h>
#include <controller/EventLoop.h>
#include <controller/GameController.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <controller/ViewIntfc.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <craps/EventManager.h>
#include <gen/BuildInfo.h>

Gen::BuildInfo*        Gbl::pBuildInfo = nullptr;
Ctrl::ConfigManager*   Gbl::pConfigMgr = nullptr;
Craps::CrapsTable*     Gbl::pTable     = nullptr;
Ctrl::EventLoop*       Gbl::pEventLoop = nullptr;
Craps::EventManager*   Gbl::pEventMgr  = nullptr;
Ctrl::GameController*  Gbl::pGameCtrl  = nullptr;
Ctrl::TableManager*    Gbl::pTableMgr  = nullptr;
Ctrl::PlayerManager*   Gbl::pPlayerMgr = nullptr;
Ctrl::ViewIntfc*       Gbl::pView      = nullptr;
Ctrl::UndoManager*     Gbl::pUndoMgr   = nullptr;

const std::string Gbl::appNameScreen = "Royal Craps";
const std::string Gbl::appNameExec   = "royalcraps";

//----------------------------------------------------------------

