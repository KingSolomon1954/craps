//----------------------------------------------------------------
//
// File: GameMain.cpp
//
//----------------------------------------------------------------

#include <controller/GameMain.h>
#include <controller/ConfigManager.h>
#include <controller/EventHandler.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/TableManager.h>
#include <cui/CuiMain.h>
#include <gen/BuildInfo.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Ctrl;

GameMain* GameMain::instancePtr_ = nullptr;

/*-----------------------------------------------------------*//**

Constructor

@param[in] argc
    Number of items in argv array.

@param[in] argv
    Array of command line args.

@internal

Constructor does not return until the game is over. It initializes all
the non-trivial classes in specific order, then it blocks forever
waiting on a signal. The signal indicates the game is over. It then
invokes an explicit shutdown order. The unique_ptrs go out of scope upon
returning and their destructors are called.

*/
GameMain::GameMain(int argc, char* argv[])
{
    instancePtr_ = this;
    std::unique_ptr<Gen::BuildInfo>       pBuildInfo(initBuildInfo());         (void) pBuildInfo;
    logStartBanner();                     // Held until log file is known
    std::unique_ptr<Ctrl::ConfigManager>  pCfg(initConfigManager(argc, argv)); (void) pCfg;
    setupLogging();                       // Log file now known
    std::unique_ptr<Ctrl::EventHandler>   pEventHandler(initEventHandler());   (void) pEventHandler;
    std::unique_ptr<Ctrl::TableManager>   pTablerMgr(initTableManager());      (void) pTablerMgr;
    std::unique_ptr<Ctrl::PlayerManager>  pPlayerMgr(initPlayerManager());     (void) pPlayerMgr;
    disableConsoleLogging();              // No more writing to screen
    initView();

    signalHandler_.waitForTerminate();  // Blocks until signal

    explicitShutdown();
}

//----------------------------------------------------------------

GameMain*
GameMain::instance()
{
    return instancePtr_;
}

//----------------------------------------------------------------

void
GameMain::terminateApp()
{
    signalHandler_.terminate();
}

//----------------------------------------------------------------

void
GameMain::logStartBanner()
{
    Gen::Logger::instance().logInfo("--------------------------------------");
    Gen::Logger::instance().logInfo("Starting " + Gbl::pBuildInfo->shortInfo());
    Gen::Logger::instance().logInfo("--------------------------------------");
}

//----------------------------------------------------------------
//
// Called once configuration is resolved. Messages logged until now
// were held by the logger; this gives it the log file and they are
// written first.
//
void
GameMain::setupLogging()
{
    assert(Gbl::pConfigMgr != nullptr);
    
    // Setup log file name.
    std::string d = Gbl::pConfigMgr->getString(ConfigManager::KeyDirsUsrLog).value();
    std::string f = "/" + Gbl::appNameExec + ".log";
    
    Gen::Logger::instance().setOutputFile(d + f);

    bool debug = Gbl::pConfigMgr->getBool(ConfigManager::KeyDebugLogging).value();
    Gen::Logger::instance().setDebugLevel(debug);
    
    bool trace = Gbl::pConfigMgr->getBool(ConfigManager::KeyTraceLogging).value();
    Gen::Logger::instance().setTraceLevel(trace);

}

//----------------------------------------------------------------

void
GameMain::disableConsoleLogging()
{
    Gen::Logger::instance().disableConsoleLogging();
}

//----------------------------------------------------------------

Gen::BuildInfo*
GameMain::initBuildInfo()
{
    auto p = new Gen::BuildInfo(Gbl::appNameScreen);
    Gbl::pBuildInfo = p;
    return p;
}

//----------------------------------------------------------------

Ctrl::ConfigManager*
GameMain::initConfigManager(int argc, char* argv[])
{
    auto* p = new Ctrl::ConfigManager(argc, argv);
    Gbl::pConfigMgr = p;
    return p;
}

//----------------------------------------------------------------

Ctrl::EventHandler*
GameMain::initEventHandler()
{
    auto p = new Ctrl::EventHandler();
    Gbl::pEventHandler = p;
    return p;
}

//----------------------------------------------------------------

TableManager*
GameMain::initTableManager()
{
    auto p = new TableManager();  // And creates initial CrapsTable
    Gbl::pTableMgr = p;
    return p;
}

//----------------------------------------------------------------

PlayerManager*
GameMain::initPlayerManager()
{
    auto p = new PlayerManager();
    Gbl::pPlayerMgr = p;
    return p;
}

//----------------------------------------------------------------

void
GameMain::initView()
{
    std::string v = Gbl::pConfigMgr->getString(ConfigManager::KeyViewType).value();
    if (v == "console")
    {
        // Just reference it so it can init()
        auto& cui = Cui::CuiMain::instance();
        return;
    }
    if (v == "graphical")
    {
        // auto& gui = Gui::GuiMain::instance();
        return;
    }

    std::string diag = "Invalid value for config parameter:\"" +
        std::string(ConfigManager::KeyViewType) +
        "\". At this time only console (--con), the default, is available. "
        "Future options for GUI and CmdLine are not implemented yet.";
        
    throw std::invalid_argument(diag);
}

//----------------------------------------------------------------

void
GameMain::shutdownView()
{
    std::string v = Gbl::pConfigMgr->getString(ConfigManager::KeyViewType).value();
    if (v == "console")
    {
        Cui::CuiMain::instance().shutdown();
        return;
    }
    
    if (v == "graphical")
    {
        // Gui::GuiMain::instance().shutdown();
        return;
    }
}

//----------------------------------------------------------------

void
GameMain::explicitShutdown()
{
    // View goes first. This stops the View thread, so nothing reads the
    // model while it shuts down, and gives the console back. Events the
    // model publishes from here on are dropped by the View.
    shutdownView();

    // Remember who is playing where while the table still has them.
    const UserState state = gatherUserState();

    // Order matters. The table shuts its players down, merging this
    // session into their stats, and saves itself. Only then can
    // PlayerManager save the players.
    Gbl::pTableMgr ->shutdown();
    Gbl::pPlayerMgr->shutdown();

    saveUserState(state);

    // Gbl::pEventHandler->shutdown();  // N/A
    // Gbl::pConfigMgr   ->shutdown();  // N/A
    // Gbl::pBuildInfo   ->shutdown();  // N/A
}

//----------------------------------------------------------------
//
// Collects what the next run should start with: the table, the user
// player and the players at the table.
//
UserState
GameMain::gatherUserState() const
{
    UserState state;
    state.table      = Gbl::pTableMgr->getCurrentFileName();
    state.userPlayer = Gbl::pPlayerMgr->getUserFileName();
    for (auto& f : Gbl::pPlayerMgr->getTablePlayerFileNames())
    {
        if (f != state.userPlayer)
        {
            state.buddies.push_back(std::move(f));
        }
    }
    if (state.buddies.size() > UserState::MaxBuddies)
    {
        state.buddies.resize(UserState::MaxBuddies);
    }
    return state;
}

//----------------------------------------------------------------
//
// Writes state.yaml. A failure is logged; it never prevents quitting.
//
void
GameMain::saveUserState(const UserState& state) const
{
    if (state.table.empty() || state.userPlayer.empty())
    {
        return;  // Nothing worth remembering
    }
    try
    {
        state.save(Gbl::pConfigMgr->getString(
            ConfigManager::KeyDirsUsrState).value());
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(std::string("GameMain::saveUserState(): ") + e.what());
    }
}

//----------------------------------------------------------------
