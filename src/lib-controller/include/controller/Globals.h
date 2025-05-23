//----------------------------------------------------------------
//
// File: Globals.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

// Note: Globals in unameed namespace

namespace Gen {
    class BuildInfo;         // fwd    
    class MultiLayerConfig;  // fwd    
}

namespace Craps {
    class CrapsTable;        // fwd    
}

namespace Ctrl {
    class EventManager;   // fwd
    class PlayerManager;  // fwd
    class TableManager;   // fwd
}

class Gbl
{
public:
    using Money = unsigned;

    static Gen::BuildInfo*        pBuildInfo;
    static Gen::MultiLayerConfig* pCfg;
    static Ctrl::EventManager*    pEventMgr;
    static Ctrl::TableManager*    pTableMgr;
    static Ctrl::PlayerManager*   pPlayerMgr;
    static Craps::CrapsTable*     pTable;

    static const std::string appNameScreen;
    static const std::string appNameExec;
};
    
//----------------------------------------------------------------
