//----------------------------------------------------------------
//
// File: Globals.cpp
//
//----------------------------------------------------------------

#include <controller/Globals.h>
#include <controller/EventManager.h>
#include <controller/PlayerManager.h>
#include <craps/CrapsTable.h>
#include <gen/BuildInfo.h>
#include <gen/MultiLayerCfg.h>

Ctrl::EventManager*  Gbl::pEventMgr  = NULL;
Ctrl::PlayerManager* Gbl::pPlayerMgr = NULL;
Craps::CrapsTable*   Gbl::pTable     = NULL;
Gen::BuildInfo*      Gbl::pBuildInfo = NULL;
Gen::MultiLayerCfg*  Gbl::pCfg       = NULL;

const std::string Gbl::appNameScreen = "Royal Craps";
const std::string Gbl::appNameExec   = "RoyalCraps";

//----------------------------------------------------------------

