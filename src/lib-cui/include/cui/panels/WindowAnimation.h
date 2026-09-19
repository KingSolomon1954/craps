//----------------------------------------------------------------
//
// File: WindowAnimation.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>

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

    WindowAnimation();
    void animate();
    void drawDie(int top, int left, int value);
};

} // namespace Cui

//----------------------------------------------------------------
