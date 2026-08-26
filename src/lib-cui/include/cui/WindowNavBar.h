//----------------------------------------------------------------
//
// File: WindowNavBar.h
//
//----------------------------------------------------------------

#pragma once

#include <cassert>

namespace Cui
{

struct WINDOW; // fwd
    
class WindowNavBar
{
public:
    /// @name Lifecycle
    /// @{
    WindowNavBar();
   ~WindowNavBar();
    /// @}

    /// @name Modifiers
    /// @{
    void draw();
    void configure();
    void clear();
    bool handleKey(int ch);
    /// @}

    /// @name Observers
    /// @{
    static WindowNavBar& instance();
    /// @}
    
private:
    struct Layout
    {
        // Nav Bar occupies rows 38-40, cols 0-99
        static constexpr int navBorderRowTop   = 38;
        static constexpr int navBorderRowBot   = 40;
        static constexpr int navBorderColLeft  = 0;
        static constexpr int navBorderColRight = 99;
        static constexpr int navBorderHeight   = navBorderBotRow   - navBorderTopRow  + 1;
        static constexpr int navBorderWidth    = navBorderRightCol - navBorderLeftCol + 1;

        // Sizing and location of Window based on LayoutConsole
        static constexpr int navTopRow   = navBorderRowTop   + 1;
        static constexpr int navBotRow   = navBorderBotRow   - 1;
        static constexpr int navLeftCol  = navBorderLeftCol  + 1;
        static constexpr int navRightCol = navBorderRightCol - 1;
        static constexpr int navHeight   = navBorderHeight   - 2;
        static constexpr int navWidth    = navBorderWidth    - 2;

        static_assert(navTopRow   == 39);
        static_assert(navBotRow   == 39);
        static_assert(navLeftCol  == 1);
        static_assert(navRightCol == 98);
        static_assert(navHeight   == 1);
        static_assert(navWidth    == 98);
    };        

    WINDOW* pWin_ = nullptr;  // The ncurses window

    void drawBorders();
    void drawExternalBorder();
    void drawExternalJunctions();
    void drawInternalBorder();
    void drawStaticContent();
    void populate();
};

} // namespace Cui

//----------------------------------------------------------------
