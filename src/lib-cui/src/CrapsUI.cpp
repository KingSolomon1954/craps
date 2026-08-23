//----------------------------------------------------------------
//
// File: CrapsUi.cpp
//
//----------------------------------------------------------------



void CrapsUI::init()
{
    pConsoleMgr_->init();

    createSurfaces();

    pConsoleMgr_->setSurface(screenCrapsTable_.get());
}


void CrapsUI::run()
{
    pConsoleMgr_->run();
}


void createSurfaces()
{
    pConsoleMgr_       = ConsoleManager::instance();
    pScreenCrapsTable_ = ScreenCrapsTable::instance();
    pBettingMenu_      = MenuBetting::instance();
    pSessionHistory_   = StatsSessionHistory::instance();
    ...
}


//----------------------------------------------------------------

void
ScreenCrapsTable::createContentWindows()
{
    using L = LayoutCrapsScreen;

    w_.header      = CuiUtils::newContentWindow(L::headerWinTopRow,      L::headerWinBotRow,      L::headerWinLeftCol,      L::headerWinRightCol);
    w_.rollHistory = CuiUtils::newContentWindow(L::rollHistWinTopRow,    L::rollHistWinBotRow,    L::rollHistWinLeftCol,    L::rollHistWinRightCol);
    w_.playerArea  = CuiUtils::newContentWindow(L::playerAreaWinTopRow,  L::playerAreaWinBotRow,  L::playerAreaWinLeftCol,  L::playerAreaWinRightCol);
    w_.message     = CuiUtils::newContentWindow(L::messageWinTopRow,     L::messageWinBotRow,     L::messageWinLeftCol,     L::messageWinRightCol);
    w_.animation   = CuiUtils::newContentWindow(L::animationWinTopRow,   L::animationWinBotRow,   L::animationWinLeftCol,   L::animationWinRightCol);
    w_.houseBrief  = CuiUtils::newContentWindow(L::houseBriefWinTopRow,  L::houseBriefWinBotRow,  L::houseBriefWinLeftCol,  L::houseBriefWinRightCol);
    w_.playerBrief = CuiUtils::newContentWindow(L::playerBriefWinTopRow, L::playerBriefWinBotRow, L::playerBriefWinLeftCol, L::playerBriefWinRightCol);

    assert(w_.header      != nullptr);
    assert(w_.rollHistory != nullptr);
    assert(w_.playerArea  != nullptr);
    assert(w_.message     != nullptr);
    assert(w_.animation   != nullptr);
    assert(w_.houseBrief  != nullptr);
    assert(w_.playerBrief != nullptr);
}

ScreenCrapsTable::~ScreenCrapsTable()
{
    if (w_.header)      delwin(w_.header);
    if (w_.rollHistory) delwin(w_.rollHistory);
    if (w_.playerArea)  delwin(w_.playerArea);
    if (w_.message)     delwin(w_.message);
    if (w_.animation)   delwin(w_.animation);
    if (w_.houseBrief)  delwin(w_.houseBrief);
    if (w_.playerBrief) delwin(w_.playerBrief);
}


// ConsoleView::populateNavBar()

// doupdate();  called by ConsoleView


// TODO Move this to WindowDiceAnimation

    // When time comes, use conditional test for pip drawing
    // auto pip = [&](int y, int x) {
    //     if (view_.useUnicodePips) {
    //         const wchar_t pipChar[] = L"●";
    //         mvwaddwstr(w, y, x, pipChar);
    //     } else {
    //         mvwaddch(w, y, x, '*'); // ASCII fallback
    //     }
    // };


        // Move to WindowHeader class
        static constexpr int headerWinTopRow      = 1;
        static constexpr int headerWinBotRow      = 1;
        static constexpr int headerWinLeftCol     = 1;
        static constexpr int headerWinRightCol    = 68;
        static constexpr int headerWinHeight      = 1;
        static constexpr int headerWinWidth       = 67;

        // Move to WindowRollHistory class
        static constexpr int rollHistWinTopRow      = 3;
        static constexpr int rollHistWinBotRow      = 3;
        static constexpr int rollHistWinLeftCol     = 1;
        static constexpr int rollHistWinRightCol    = 68;
        static constexpr int rollHistWinHeight      = 1;
        static constexpr int rollHistWinWidth       = 67;
        
        // Move to WindowMessage class
        static constexpr int messageWinTopRow      = 25;
        static constexpr int messageWinBotRow      = 37;
        static constexpr int messageWinLeftCol     = 1;
        static constexpr int messageWinRightCol    = 68;
        static constexpr int messageWinHeight      = 13;
        static constexpr int messageWinWidth       = 67;
        
        // Move to WindowAnimation class
        static constexpr int animationWinTopRow      = 1;
        static constexpr int animationWinBotRow      = 23;
        static constexpr int animationWinLeftCol     = 70;
        static constexpr int animationWinRightCol    = 98;
        static constexpr int animationWinHeight      = 23;
        static constexpr int animationWinWidth       = 24;
        
        // Move to WindowHouseBrief class
        static constexpr int houseBriefWinTopRow      = 25;
        static constexpr int houseBriefWinBotRow      = 29;
        static constexpr int houseBriefWinLeftCol     = 70;
        static constexpr int houseBriefWinRightCol    = 98;
        static constexpr int houseBriefWinHeight      = 5;
        static constexpr int houseBriefWinWidth       = 24;
        
        // Move to WindowPlayerBrief class
        static constexpr int playerBriefWinTopRow      = 31;
        static constexpr int playerBriefWinBotRow      = 37;
        static constexpr int playerBriefWinLeftCol     = 70;
        static constexpr int playerBriefWinRightCol    = 98;
        static constexpr int playerBriefWinHeight      = 7;
        static constexpr int playerBriefWinWidth       = 24;


        // Move to WindowNavBar class
        static constexpr int navBarWinTopRow      = 39;
        static constexpr int navBarWinBotRow      = 39;
        static constexpr int navBarWinLeftCol     = 1;
        static constexpr int navBarWinRightCol    = 98;
        static constexpr int navBarWinHeight      = 1;
        static constexpr int navBarWinWidth       = 98;
        
        // Dice Drawing
        // TODO move this to animation window rendering 
        static constexpr int diceHeight = 5;
        static constexpr int diceWidth = 7;


