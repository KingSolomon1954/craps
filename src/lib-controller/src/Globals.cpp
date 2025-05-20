//----------------------------------------------------------------
//
// File: Globals.cpp
//
//----------------------------------------------------------------

#include <string>
#include <controller/Globals.h>
#include <craps/CrapsTable.h>
#include <craps/EventManager.h>
#include <craps/PlayerManager.h>

Craps::EventManager*  Gbl::pEventMgr  = NULL;
Craps::CrapsTable*    Gbl::pTable     = NULL;
Craps::PlayerManager* Gbl::pPlayerMgr = NULL;

const std::string Gbl::appNameScreen = "Royal Craps";
const std::string Gbl::appNameExec   = "RoyalCraps";

//----------------------------------------------------------------

