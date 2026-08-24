//----------------------------------------------------------------
//
// File: CrapsUi.cpp
//
//----------------------------------------------------------------



void
CrapsUI::init()
{
    createSurfaces();
    auto pConsoleMgr = ConsoleManager::instance();
    pConsoleMgr->init(); // ncurses init, 
    pConsoleMgr->setSurface(ScreenCrapsTable_.instance());
}

//----------------------------------------------------------------

void
CrapsUI::run()
{
    ConsoleManager::instance()->run();
}

//----------------------------------------------------------------

void
createSurfaces()
{
    ? pBettingMenu_           = MenuBetting::instance();

    // Create themselves, no need for class member vars for each.
    auto w1 = ScreenCrapsTable::instance();
    auto w2 = ScreenDiceStats::instance();
    auto w3 = ScreenSessionsStats::instance();
    auto w4 = ScreenPlayerStats::instance();
    auto w5 = ScreenSessionHistory::instance();
    ...
}

//----------------------------------------------------------------

#if 0


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


#endif
