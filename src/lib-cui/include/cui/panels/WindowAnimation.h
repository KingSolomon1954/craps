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
        ZeroRoll,
        Animating,
        ShowingRoll
    };

    enum class AnimationPhase
    {
        Falling,
        Settling,
        Done
    };

    struct DiceRoll
    {
        unsigned rollCount = 0;
        unsigned value = 0;
        unsigned d1 = 0;
        unsigned d2 = 0;
    };


    Gen::TimerManager::TimerId timerId_;
    DiceRoll lastRoll_;
    AnimationState state_ = AnimationState::ZeroRoll;
    AnimationPhase animationPhase_ = AnimationPhase::Falling;

private:
    // Frame & dice drawing vars and constants
    int animationY_ = 1;
    int settleIndex_ = 0;
    
    static constexpr int DieWidth   = 7;
    static constexpr int DieHeight  = 5;
    static constexpr int DieSpacing = 4;

    static constexpr int TotalWidth = DieWidth * 2 + DieSpacing;
    static constexpr int BaseX =
        std::max(1, (Layout::animationWidth - TotalWidth) / 2);

    static constexpr int Die1X = BaseX;
    static constexpr int Die2X = BaseX + DieWidth + DieSpacing;

    static constexpr int LandingY = Layout::animationHeight - DieHeight;

private:    
    WindowAnimation();
    void drawBanner();
    void drawZeroRoll();
    void drawAnimation();
    void drawShowingRoll();
    void renderFallingFrame();
    void renderSettlingFrame();
    void renderFinalFrame();
    void drawDie(int top, int left, int value);
    void startAnimation();
    void stopAnimation();
    void enqueueDraw();
    int  randomDieValue();
    int  clampDieX(int j);
    int  randomJitter(int max);
    int  j2Helper(int j1);
    std::wstring middleFinishedRoll();
};

} // namespace Cui

//----------------------------------------------------------------
