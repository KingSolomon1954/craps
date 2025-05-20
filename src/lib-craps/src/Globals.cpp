//----------------------------------------------------------------
//
// File: Globals.cpp
//
//----------------------------------------------------------------

#include <craps/Globals.h>
#include <craps/CrapsTable.h>
#include <craps/EventManager.h>
#include <craps/PlayerManager.h>

using namespace Craps;

EventManager*  Gbl::pEventMgr  = NULL;
CrapsTable*    Gbl::pTable     = NULL;
PlayerManager* Gbl::pPlayerMgr = NULL;

//----------------------------------------------------------------

