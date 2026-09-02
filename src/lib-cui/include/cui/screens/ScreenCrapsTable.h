//----------------------------------------------------------------
//
// File: ScreenCrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/ScreenBase.h>

namespace Cui {

class ScreenCrapsTable : public ScreenBase
{
public:
    /// @name Lifecycle
    /// @{
   ~ScreenCrapsTable() = default;
    /// @}
    
    /// @name Modifiers
    /// @{
    void draw()                         override;  // from Surface
    void handleKey(int ch)              override;  // from Surface
    void onPause()                      override;  // from Surface
    void onResume()                     override;  // from Surface
    void onDetach()                     override;  // from Surface
    void onAttach(SurfaceBase* pParent) override;  // from Surface
    /// @}

    /// @name Observers
    /// @{
    static ScreenCrapsTable& instance();
    /// @}

private:
    ScreenCrapsTable();  // private ctor
    void createWindow();
    void registerWindow();
    void fillWindow();
    void drawNavBar();
};

/*-----------------------------------------------------------*//**

@class ScreenCrapsTable

@brief Craps Table Screen

@li owns size of each Panel (in LayoutCrapsScreen)
@li relationship between Panels
@li screen-level borders
@li screen-level junctions
@li overall composition of its Panels
@li does not perform key input. MenuBetting does that.

*/

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
