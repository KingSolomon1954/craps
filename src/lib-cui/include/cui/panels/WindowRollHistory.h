//----------------------------------------------------------------
//
// File: WindowRollHistory.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <deque>

#if 1
namespace
{
    static constexpr short PairSevenOut       = 1;
    static constexpr short PairPassLineWinner = 2;
}
#endif

namespace Cui
{

    class WindowRollHistory : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowRollHistory() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    void onDiceNewValue(unsigned d1, unsigned d2, unsigned rollCount);
    void onSevenOut();
    void onPassLineWinner();
    /// @}

    /// @name Observers
    /// @{
    static WindowRollHistory& instance();
    /// @}

private:
    struct Layout
    {
        using L = LayoutCrapsScreen;

        // Sizing and location based on LayoutCrapsScreen
        static constexpr int rollHistTopRow   = L::rollHistBorderTopRow   + 1;
        static constexpr int rollHistBotRow   = L::rollHistBorderBotRow   - 1;
        static constexpr int rollHistLeftCol  = L::rollHistBorderLeftCol  + 1;
        static constexpr int rollHistRightCol = L::rollHistBorderRightCol - 1;
        static constexpr int rollHistHeight   = rollHistBotRow   - rollHistTopRow  + 1;
        static constexpr int rollHistWidth    = rollHistRightCol - rollHistLeftCol + 1;
    };

    struct Roll
    {
        unsigned d1 = 0;
        unsigned d2 = 0;
        bool passLineWinner = false;
        bool sevenOut = false;
    };

    enum class Format
    {
        A, // │ [A]   7(3,4) 8(4,4) 6(3,3) 11(6,5)  │
        B, // │ [B]   7(34) 8(44) 6(33) 11(65)      │
        C, // │ [C]   7,8,6,11                      │
        D  // │ [D]   7 8 6 11                      │
    };     

    std::deque<Roll> rolls_;  // newest first
    static constexpr std::size_t MaxSize = 100;
    static constexpr std::size_t CharLimit = Layout::rollHistWidth - 2;  // space on either side

private:
    WindowRollHistory();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void populate();

    void add(const Roll& r);
    std::string format(Format fmt, const Roll& r) const;
    const Roll& operator[](std::size_t index) const;
};

} // namespace Cui

//----------------------------------------------------------------
