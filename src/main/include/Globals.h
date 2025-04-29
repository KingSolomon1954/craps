//----------------------------------------------------------------
//
// File: Globals.h
//
//----------------------------------------------------------------

#pragma once

namespace App {

typedef unsigned Money;

class CrapsTable;     // fwd    
class PlayerManager;  // fwd

class Gbl
{
public:
    static CrapsTable*    pTable;
    static PlayerManager* pPlayerMgr;
};
    
} // namespace App

//----------------------------------------------------------------
