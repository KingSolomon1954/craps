//----------------------------------------------------------------
//
// File: LayoutCrapsScreen.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui {
    
struct LayoutCrapsScreen
{
    // Header Window occupies rows 0-2, cols 0-69
    static constexpr int headerBorderTopRow   = 0;
    static constexpr int headerBorderBotRow   = 2;
    static constexpr int headerBorderLeftCol  = 0;
    static constexpr int headerBorderRightCol = 69;

    // Dice Roll History occupies rows 2-4, cols 0-69
    static constexpr int rollHistBorderTopRow   = 2;
    static constexpr int rollHistBorderBotRow   = 4;
    static constexpr int rollHistBorderLeftCol  = 0;
    static constexpr int rollHistBorderRightCol = 69;

    // Player Area occupies rows 4-24, cols 0-69
    static constexpr int playerAreaBorderTopRow   = 4;
    static constexpr int playerAreaBorderBotRow   = 24;
    static constexpr int playerAreaBorderLeftCol  = 0;
    static constexpr int playerAreaBorderRightCol = 69;

    // Message Area occupies rows 24-38, cols 0-69
    static constexpr int messageBorderTopRow   = 24;
    static constexpr int messageBorderBotRow   = 38;
    static constexpr int messageBorderLeftCol  = 0;
    static constexpr int messageBorderRightCol = 69;

    // Dice Animation occupies rows 0-24, cols 69-99
    static constexpr int animationBorderTopRow   = 0;
    static constexpr int animationBorderBotRow   = 24;
    static constexpr int animationBorderLeftCol  = 69;
    static constexpr int animationBorderRightCol = 99;

    // House Brief Info occupies rows 24-30, cols 69-99
    static constexpr int houseBriefBorderTopRow   = 24;
    static constexpr int houseBriefBorderBotRow   = 30;
    static constexpr int houseBriefBorderLeftCol  = 69;
    static constexpr int houseBriefBorderRightCol = 99;

    // Player Brief Info occupies rows 30-38, cols 69-99
    static constexpr int playerBriefBorderTopRow   = 30;
    static constexpr int playerBriefBorderBotRow   = 38;
    static constexpr int playerBriefBorderLeftCol  = 69;
    static constexpr int playerBriefBorderRightCol = 99;

    static void draw();
};

} // namespace Cui

//----------------------------------------------------------------
