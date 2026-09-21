//----------------------------------------------------------------
//
// File: WindowTitleBar.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>

namespace Cui
{

    class WindowTitleBar : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowTitleBar() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    void onDiceNewValue(unsigned d1, unsigned d2, unsigned rollCount);
    void onPointEstablished(unsigned point);
    void onSevenOut();
    void onPassLineWinner();
    /// @}

    /// @name Observers
    /// @{
    static WindowTitleBar& instance();
    /// @}

private:
    struct Layout
    {
        using L = LayoutCrapsScreen;

        // Sizing and location based on LayoutCrapsScreen
        static constexpr int titleBarTopRow   = L::titleBarBorderTopRow   + 1;
        static constexpr int titleBarBotRow   = L::titleBarBorderBotRow   - 1;
        static constexpr int titleBarLeftCol  = L::titleBarBorderLeftCol  + 1;
        static constexpr int titleBarRightCol = L::titleBarBorderRightCol - 1;
        static constexpr int titleBarHeight   = titleBarBotRow   - titleBarTopRow  + 1;
        static constexpr int titleBarWidth    = titleBarRightCol - titleBarLeftCol + 1;

        // Field: Point
        static constexpr std::string_view f1Label = "Point ";
        static constexpr int f1LabelRow = 0;   // relative to window 0
        static constexpr int f1LabelCol = 1;   // relative to window 0
        static constexpr int f1Row      = f1LabelRow; 
        static constexpr int f1Col      = 7;
        static constexpr std::string_view f1Fmt = "%-2u";

        // Field: Roll Count
        static constexpr std::string_view f2Label = "Roll ";
        static constexpr int f2LabelRow = 0;   // relative to window 0
        static constexpr int f2LabelCol = 12;  // relative to window 0
        static constexpr int f2Row      = f2LabelRow; 
        static constexpr int f2Col      = 17;
        static constexpr std::string_view f2Fmt = "%-5u";

        // Field: Table Name
        static constexpr int f3Row      = 0;
        static constexpr int f3Col      = 23;
        static constexpr std::string_view f3Fmt = "%16s";
    };
    
    unsigned rollCount_ = 0;
    unsigned point_ = 0;
    std::string tableName_;

private:
    WindowTitleBar();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void populate();
    void populatePoint();
    void populateRollCount();
    void populateTableName();
    void populateTableMaxOdds();
};

} // namespace Cui

//----------------------------------------------------------------
