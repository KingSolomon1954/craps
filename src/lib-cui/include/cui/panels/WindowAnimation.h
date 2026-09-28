//----------------------------------------------------------------
//
// File: WindowAnimation.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/ColorManager.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <craps/CrapsTypes.h>
#include <gen/TimerManager.h>
#include <string>

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
    void onNewShooter(const Craps::PlayerId& id);
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

    std::string     shooter_;
    Craps::PlayerId shooterPlayerId_;
    PlayerColor     shooterPlayerColor_;
    
    std::string     prevShooter_;
    Craps::PlayerId prevShooterPlayerId_;
    PlayerColor     prevShooterPlayerColor_;

private:
    // Frame & dice drawing vars and constants
    int animationY_ = 2;
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
    void drawShowingShooter();
    void drawAnimationShooter();
    void drawShooter(const std::string& name, PlayerColor pc);
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
