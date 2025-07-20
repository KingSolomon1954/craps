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
}

namespace Ctrl {
    class ConfigManager;   // fwd    
    class EventLoop;       // fwd
    class EventManager;    // fwd
    class GameController;  // fwd
    class PlayerManager;   // fwd
    class TableManager;    // fwd
    class ViewIntfc;       // fwd
}

class Gbl
{
public:
    static Gen::BuildInfo*       pBuildInfo;
    static Ctrl::ConfigManager*  pConfigMgr;
    static Craps::CrapsTable*    pTable;
    static Ctrl::EventLoop*      pEventLoop;
    static Ctrl::EventManager*   pEventMgr;
    static Ctrl::GameController* pGameCtrl;
    static Ctrl::PlayerManager*  pPlayerMgr;
    static Ctrl::TableManager*   pTableMgr;
    static Ctrl::ViewIntfc*      pView;

    static const std::string appNameScreen;
    static const std::string appNameExec;
};

//----------------------------------------------------------------
