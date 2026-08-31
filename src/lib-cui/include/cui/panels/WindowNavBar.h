//----------------------------------------------------------------
//
// File: WindowNavBar.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutConsole.h>

namespace Cui
{
    
class WindowNavBar : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowNavBar() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    void configure(const std::string& testLine);
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
        // Derive our ncurses WINDOW dimensions
        using C = LayoutConsole;
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
    
    WindowNavBar();         // private ctor
    void drawStaticContent();
    void populate();

    void doControl();
    void doBetting();
    void doView();
    void doStats();
    void doHelp();
    void doQuit();
};

} // namespace Cui

//----------------------------------------------------------------
