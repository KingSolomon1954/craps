//----------------------------------------------------------------
//
// File: Globals.h
//
//----------------------------------------------------------------

#pragma once

namespace Craps {

typedef unsigned Money;

class CrapsTable;     // fwd    
class EventManager;   // fwd
class PlayerManager;  // fwd

class Gbl
{
public:
    static CrapsTable*    pTable;
    static EventManager*  pEventMgr;
    static PlayerManager* pPlayerMgr;
};
    
} // namespace Craps

//----------------------------------------------------------------
