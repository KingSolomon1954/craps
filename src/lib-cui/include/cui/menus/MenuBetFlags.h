//----------------------------------------------------------------
//
// File: MenuBetFlags.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/core/CuiStructs.h>
#include <controller/BetFlags.h>
#include <string>
#include <vector>

namespace Cui {

class MenuBetFlags : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuBetFlags() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                                override;
    bool handleKey(int ch)                     override;
    void onAttach(SurfaceBase* pParent)        override;
    void setLocation(WindowPosition pos)       override;
    /// @}

    /// @name Observers
    /// @{
    static MenuBetFlags& instance();
    LocationRequest getLocationRequest() const override;
    /// @}

private:
    struct MenuEntry
    {
        int                    hotKey;     ///< 27 for Back
        Ctrl::BetFlags::Scope  scope;
        std::string            text;
    };

    std::vector<MenuEntry> menuEntries_;
    WindowPosition         winPos_{10, 10};  // Arbitrary, gets set later
    WindowSize             winSize_;

private:
    MenuBetFlags();  // Private ctor

    void buildMenuEntries();
    void calcSize();
    void drawBorders();
    void populate();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuBetFlags

@brief Choose which bets to turn off or on

Offers one line per kind of bet the player has that can take a flag
(odds, place, buy, lay, hardway), plus all bets and one individual bet.
Kinds the player has no bets for are not shown, so the menu is built
each time it opens. The keys are mnemonic: i, a, o, p, b, l, h.

Picking a line starts the Bet Flags workflow in OperationManager.

*/

} // namespace Cui

//----------------------------------------------------------------
