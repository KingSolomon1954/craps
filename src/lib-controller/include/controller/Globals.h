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
    class EventManager;      // fwd
    class PlayerManager;     // fwd
}

class Gbl
{
public:
    using Money = unsigned;

    static Craps::CrapsTable*     pTable;
    static Ctrl::EventManager*    pEventMgr;
    static Ctrl::PlayerManager*   pPlayerMgr;
    static Gen::BuildInfo*        pBuildInfo;
    static Gen::MultiLayerConfig* pCfg;

    static const std::string appNameScreen;
    static const std::string appNameExec;
};
    
//----------------------------------------------------------------
