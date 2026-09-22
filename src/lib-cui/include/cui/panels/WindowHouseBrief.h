//----------------------------------------------------------------
//
// File: WindowHouseBrief.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>

namespace Cui
{

    class WindowHouseBrief : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowHouseBrief() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    /// @}

    /// @name Observers
    /// @{
    static WindowHouseBrief& instance();
    /// @}

private:
    struct Layout
    {
        using L = LayoutCrapsScreen;

        // Sizing and location based on LayoutCrapsScreen
        static constexpr int houseBriefTopRow   = L::houseBriefBorderTopRow   + 1;
        static constexpr int houseBriefBotRow   = L::houseBriefBorderBotRow   - 1;
        static constexpr int houseBriefLeftCol  = L::houseBriefBorderLeftCol  + 1;
        static constexpr int houseBriefRightCol = L::houseBriefBorderRightCol - 1;
        static constexpr int houseBriefHeight   = houseBriefBotRow   - houseBriefTopRow  + 1;
        static constexpr int houseBriefWidth    = houseBriefRightCol - houseBriefLeftCol + 1;
    };

private:
    WindowHouseBrief();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void populate();
    void populateBalance();
    void populateNumBets();
    void populatePct();
    void populateOnTable();
    void populateLast();
};

} // namespace Cui

//----------------------------------------------------------------
