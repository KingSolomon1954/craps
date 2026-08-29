//----------------------------------------------------------------
//
// File: WindowNavBar.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/LayoutConsole>
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
   ~WindowNavBar() = default;
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
        using C = LayoutConsole;
        
        // Sizing and location of Window based on LayoutConsole
        static constexpr int navTopRow   = C::navBorderTopRow   + 1;
        static constexpr int navBotRow   = C::navBorderBotRow   - 1;
        static constexpr int navLeftCol  = C::navBorderLeftCol  + 1;
        static constexpr int navRightCol = C::navBorderRightCol - 1;
        static constexpr int navHeight   = C::navBorderHeight   - 2;
        static constexpr int navWidth    = C::navBorderWidth    - 2;

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
