//----------------------------------------------------------------
//
// File: Globals.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

// Note: Globals in unamed namespace

namespace Gen {
    class BuildInfo;       // fwd    
}

namespace Craps {
    class CrapsTable;      // fwd    
}

namespace Ctrl {
    class ConfigManager;   // fwd    
    class EventHandler;    // fwd    
    class PlayerManager;   // fwd
    class TableManager;    // fwd
    class UndoManager;     // fwd
}

class Gbl
{
public:
    static Gen::BuildInfo*      pBuildInfo;
    static Ctrl::ConfigManager* pConfigMgr;
    static Ctrl::EventHandler*  pEventHandler;
    static Craps::CrapsTable*   pTable;
    static Ctrl::PlayerManager* pPlayerMgr;
    static Ctrl::TableManager*  pTableMgr;
    static Ctrl::UndoManager*   pUndoMgr;

    static const std::string appNameScreen;
    static const std::string appNameExec;
};

//----------------------------------------------------------------
