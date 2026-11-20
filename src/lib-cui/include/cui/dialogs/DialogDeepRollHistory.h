//----------------------------------------------------------------
//
// File: DialogDeepRollHistory.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <cui/core/CuiStructs.h>
#include <cui/core/RollLog.h>
#include <string>

//----------------------------------------------------------------

namespace Cui {

class DialogDeepRollHistory : public DialogBase
{
public:
    /// The grid reads newest first down column 1, then column 2, ...
    static constexpr int GridRows = 20;
    static constexpr int GridCols = 6;
    static_assert(GridRows * GridCols == RollLog::Capacity);

    /// @name Lifecycle
    /// @{
   ~DialogDeepRollHistory() = default;
    static DialogDeepRollHistory& instance();
    DialogDeepRollHistory(const DialogDeepRollHistory&)            = delete;
    DialogDeepRollHistory& operator=(const DialogDeepRollHistory&) = delete;
    DialogDeepRollHistory(DialogDeepRollHistory&&)                 = delete;
    DialogDeepRollHistory& operator=(DialogDeepRollHistory&&)      = delete;
    /// @}

    /// @name Modifiers
    /// @{
    void show();                               // Pops up over the current surface
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;

    /// Text of one cell, right justified, e.g. " 7(3,4)" and "10(6,5)".
    static std::string cellText(const RollEntry& roll);
    /// Roll index shown in grid position (row, col); newest is 0.
    static std::size_t indexAt(int row, int col);
    /// @}

private:
    DialogDeepRollHistory();

    static constexpr int CellWidth = 7;                  // "10(6,5)"
    static constexpr int CellSpan  = CellWidth + 2;      // blank on each side
    static constexpr int WinCols   = 2 + GridCols * CellSpan + (GridCols - 1);
    static constexpr int WinRows   = 3 + GridRows + 3;   // title, grid, prompt

    WindowPosition winPos_{};
    WindowSize     winSize_{WinRows, WinCols};

    void drawBorders();
    void drawGrid();
    void back();
};

/*-----------------------------------------------------------*//**

@class DialogDeepRollHistory

@brief Popup listing the last 120 rolls, most detailed format only

@li Reads RollLog; keeps nothing of its own.
@li Rolls keep the colors the roll history uses: seven out, pass line
    winner and point established.
@li Does not depend on any menu. Whatever menu offers it calls show().
*/

} // namespace Cui

//----------------------------------------------------------------
