//----------------------------------------------------------------
//
// File: Globals.cpp
//
//----------------------------------------------------------------

#include <controller/Globals.h>
#include <controller/EventManager.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <craps/CrapsTable.h>
#include <gen/BuildInfo.h>
#include <gen/MultiLayerConfig.h>

Gen::BuildInfo*        Gbl::pBuildInfo = NULL;
Gen::MultiLayerConfig* Gbl::pCfg       = NULL;
Ctrl::EventManager*    Gbl::pEventMgr  = NULL;
Ctrl::TableManager*    Gbl::pTableMgr  = NULL;
Ctrl::PlayerManager*   Gbl::pPlayerMgr = NULL;
Craps::CrapsTable*     Gbl::pTable     = NULL;

const std::string Gbl::appNameScreen = "Royal Craps";
const std::string Gbl::appNameExec   = "RoyalCraps";

//----------------------------------------------------------------

