//----------------------------------------------------------------
//
// File: LayoutCrapsScreen.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/layouts/LayoutConsole.h>
#include <ncurses.h>
#include <cassert>

namespace Cui {
    
class LayoutCrapsScreen
{
public:
    static void init(WINDOW* pWin);
    static LayoutCrapsScreen& instance();

    void draw();

    void drawExternalJunctionsAllPlayers();
    void eraseExternalJunctionsAllPlayers();
    void drawExternalJunctionsOnePlayer();
    void eraseExternalJunctionsOnePlayer();

    // Disallow
    LayoutCrapsScreen(const LayoutCrapsScreen&)            = delete;
    LayoutCrapsScreen& operator=(const LayoutCrapsScreen&) = delete;
    
    static constexpr int height   = LayoutConsole::height;
    static constexpr int width    = LayoutConsole::width;
    static constexpr int leftCol  = LayoutConsole::leftCol;
    static constexpr int rightCol = LayoutConsole::rightCol;
    static constexpr int topRow   = LayoutConsole::topRow;
    static constexpr int botRow   = LayoutConsole::botRow;
    
    // Title bar occupies rows 0-2, cols 0-69
    static constexpr int titleBorderTopRow   = topRow;
    static constexpr int titleBorderBotRow   = topRow + 2;
    static constexpr int titleBorderLeftCol  = leftCol;
    static constexpr int titleBorderRightCol = 69;
    static constexpr int titleBorderHeight   = titleBorderBotRow   - titleBorderTopRow  + 1;
    static constexpr int titleBorderWidth    = titleBorderRightCol - titleBorderLeftCol + 1;

    // Dice Roll History occupies rows 2-4, cols 0-69
    static constexpr int rollHistBorderTopRow   = titleBorderBotRow;
    static constexpr int rollHistBorderBotRow   = rollHistBorderTopRow + 2;
    static constexpr int rollHistBorderLeftCol  = leftCol;
    static constexpr int rollHistBorderRightCol = 69;
    static constexpr int rollHistBorderHeight   = rollHistBorderBotRow   - rollHistBorderTopRow  + 1;
    static constexpr int rollHistBorderWidth    = rollHistBorderRightCol - rollHistBorderLeftCol + 1;

    // Player Area occupies rows 4-24, cols 0-69
    static constexpr int playerAreaBorderTopRow   = rollHistBorderBotRow;
    static constexpr int playerAreaBorderBotRow   = playerAreaBorderTopRow + 20;
    static constexpr int playerAreaBorderLeftCol  = leftCol;
    static constexpr int playerAreaBorderRightCol = 69;
    static constexpr int playerAreaBorderHeight   = playerAreaBorderBotRow   - playerAreaBorderTopRow  + 1;
    static constexpr int playerAreaBorderWidth    = playerAreaBorderRightCol - playerAreaBorderLeftCol + 1;

    // Message Area occupies rows 24-38, cols 0-69
    static constexpr int messageBorderTopRow   = playerAreaBorderBotRow;
    static constexpr int messageBorderBotRow   = messageBorderTopRow + 14;
    static constexpr int messageBorderLeftCol  = leftCol;
    static constexpr int messageBorderRightCol = 69;
    static constexpr int messageBorderHeight   = messageBorderBotRow   - messageBorderTopRow  + 1;
    static constexpr int messageBorderWidth    = messageBorderRightCol - messageBorderLeftCol + 1;

    // Dice Animation occupies rows 0-24, cols 69-99
    static constexpr int animationBorderTopRow   = topRow;
    static constexpr int animationBorderBotRow   = animationBorderTopRow + 24;
    static constexpr int animationBorderLeftCol  = 69;
    static constexpr int animationBorderRightCol = rightCol;
    static constexpr int animationBorderHeight   = animationBorderBotRow   - animationBorderTopRow  + 1;
    static constexpr int animationBorderWidth    = animationBorderRightCol - animationBorderLeftCol + 1;

    // House Brief Info occupies rows 24-30, cols 69-99
    static constexpr int houseBriefBorderTopRow   = animationBorderBotRow;
    static constexpr int houseBriefBorderBotRow   = houseBriefBorderTopRow + 6;
    static constexpr int houseBriefBorderLeftCol  = 69;
    static constexpr int houseBriefBorderRightCol = rightCol;
    static constexpr int houseBriefBorderHeight   = houseBriefBorderBotRow   - houseBriefBorderTopRow  + 1;
    static constexpr int houseBriefBorderWidth    = houseBriefBorderRightCol - houseBriefBorderLeftCol + 1;

    // Player Brief Info occupies rows 30-38, cols 69-99
    static constexpr int playerBriefBorderTopRow   = houseBriefBorderBotRow;
    static constexpr int playerBriefBorderBotRow   = playerBriefBorderTopRow + 8;
    static constexpr int playerBriefBorderLeftCol  = 69;
    static constexpr int playerBriefBorderRightCol = rightCol;
    static constexpr int playerBriefBorderHeight   = playerBriefBorderBotRow   - playerBriefBorderTopRow  + 1;
    static constexpr int playerBriefBorderWidth    = playerBriefBorderRightCol - playerBriefBorderLeftCol + 1;

    static_assert(titleBorderTopRow   == 0);
    static_assert(titleBorderBotRow   == 2);
    static_assert(titleBorderLeftCol  == 0);
    static_assert(titleBorderRightCol == 69);
    static_assert(titleBorderHeight   == 3);
    static_assert(titleBorderWidth    == 70);

    static_assert(rollHistBorderTopRow   == 2);
    static_assert(rollHistBorderBotRow   == 4);
    static_assert(rollHistBorderLeftCol  == 0);
    static_assert(rollHistBorderRightCol == 69);
    static_assert(rollHistBorderHeight   == 3);
    static_assert(rollHistBorderWidth    == 70);

    static_assert(playerAreaBorderTopRow   == 4);
    static_assert(playerAreaBorderBotRow   == 24);
    static_assert(playerAreaBorderLeftCol  == 0);
    static_assert(playerAreaBorderRightCol == 69);
    static_assert(playerAreaBorderHeight   == 21);
    static_assert(playerAreaBorderWidth    == 70);

    static_assert(messageBorderTopRow   == 24);
    static_assert(messageBorderBotRow   == 38);
    static_assert(messageBorderLeftCol  == 0);
    static_assert(messageBorderRightCol == 69);
    static_assert(messageBorderHeight   == 15);
    static_assert(messageBorderWidth    == 70);

    static_assert(animationBorderTopRow   == 0);
    static_assert(animationBorderBotRow   == 24);
    static_assert(animationBorderLeftCol  == 69);
    static_assert(animationBorderRightCol == 99);
    static_assert(animationBorderHeight   == 25);
    static_assert(animationBorderWidth    == 31);

    static_assert(houseBriefBorderTopRow   == 24);
    static_assert(houseBriefBorderBotRow   == 30);
    static_assert(houseBriefBorderLeftCol  == 69);
    static_assert(houseBriefBorderRightCol == 99);
    static_assert(houseBriefBorderHeight   == 7);
    static_assert(houseBriefBorderWidth    == 31);

    static_assert(playerBriefBorderTopRow   == 30);
    static_assert(playerBriefBorderBotRow   == 38);
    static_assert(playerBriefBorderLeftCol  == 69);
    static_assert(playerBriefBorderRightCol == 99);
    static_assert(playerBriefBorderHeight   == 9);
    static_assert(playerBriefBorderWidth    == 31);

private:
    LayoutCrapsScreen() = default;
    static inline WINDOW* pWin_ = nullptr;

};

} // namespace Cui

//----------------------------------------------------------------
