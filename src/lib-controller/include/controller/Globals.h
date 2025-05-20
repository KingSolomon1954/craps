//----------------------------------------------------------------
//
// File: Globals.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

// Note: Globals in unameed namespace

namespace Craps {
    class CrapsTable;     // fwd    
    class EventManager;   // fwd
    class PlayerManager;  // fwd
}
    
class Gbl
{
public:
    using Money = unsigned;

    static Craps::CrapsTable*    pTable;
    static Craps::EventManager*  pEventMgr;
    static Craps::PlayerManager* pPlayerMgr;

    static const std::string appNameScreen;
    static const std::string appNameExec;
};
    
//----------------------------------------------------------------
