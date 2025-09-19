//----------------------------------------------------------------
//
// File: Globals.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

// Note: Globals in unameed namespace

namespace Gen {
    class BuildInfo;       // fwd    
}

namespace Craps {
    class CrapsTable;      // fwd    
    class EventManager;    // fwd    
}

namespace Ctrl {
    class ConfigManager;   // fwd    
    class EventManager;    // fwd
    class GameController;  // fwd
    class PlayerManager;   // fwd
    class TableManager;    // fwd
    class ViewInterface;   // fwd
    class UndoManager;     // fwd
}

class Gbl
{
public:
    static Gen::BuildInfo*       pBuildInfo;
    static Ctrl::ConfigManager*  pConfigMgr;
    static Craps::CrapsTable*    pTable;
    static Craps::EventManager*  pEventMgr;
    static Ctrl::GameController* pGameCtrl;
    static Ctrl::PlayerManager*  pPlayerMgr;
    static Ctrl::TableManager*   pTableMgr;
    static Ctrl::ViewInterface*  pView;
    static Ctrl::UndoManager*    pUndoMgr;

    static const std::string appNameScreen;
    static const std::string appNameExec;
};

//----------------------------------------------------------------
