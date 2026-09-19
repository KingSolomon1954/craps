//----------------------------------------------------------------
//
// File: WindowAnimation.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <gen/TimerManager.h>

namespace Cui
{

    class WindowAnimation : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowAnimation() = default;
    static WindowAnimation& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    void onDiceThrowStart();
    void onDiceNewValue(int d1, int d2, int rollCount);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    
    struct Layout
    {
        using L = LayoutCrapsScreen;
        
        // Sizing and location based on LayoutCrapsScreen
        static constexpr int animationTopRow   = L::animationBorderTopRow   + 1;
        static constexpr int animationBotRow   = L::animationBorderBotRow   - 1;
        static constexpr int animationLeftCol  = L::animationBorderLeftCol  + 1;
        static constexpr int animationRightCol = L::animationBorderRightCol - 1;
        static constexpr int animationHeight   = animationBotRow   - animationTopRow  + 1;
        static constexpr int animationWidth    = animationRightCol - animationLeftCol + 1;
    };

    enum class AnimationState
    {
        NoRoll,
        Animating,
        ShowingRoll
    };

    struct DiceRoll
    {
        unsigned rollCount = 0;
        unsigned value = 0;
        unsigned d1 = 0;
        unsigned d2 = 0;
    };

    AnimationState state_ = AnimationState::NoRoll;
    DiceRoll lastRoll_;
    Gen::TimerManager::TimerId timerId_;
    
private:    
    WindowAnimation();
    void drawBanner();
    void drawNoRoll();
    void drawAnimationFrame();
    void drawFinalDice();
    void drawDie(int top, int left, int value);
    void startAnimation();
    void stopAnimation();
    void animateFrame();
    void enqueueDraw();
};

} // namespace Cui

//----------------------------------------------------------------
