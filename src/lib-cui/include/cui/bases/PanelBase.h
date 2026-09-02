//----------------------------------------------------------------
//
// File: PanelBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/SurfaceBase.h>
#include <ncurses.h>
#include <cassert>
#include <stdexcept>
#include <string>

namespace Cui {

class PanelBase : public SurfaceBase
{
public:
    /// @name Lifecycle
    /// @{
   ~PanelBase() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void handleKey(int ch) override  // Satisfy SurfaceBase
    {
        // Empty, 
    }
    /// @}

    /// @name Observers
    /// @{
    /// @}

protected:
    
private:
    
};

/*-----------------------------------------------------------*//**

@class PanelBase

@brief A Window physically contained within a full screen Surface

@li Concrete Panel classes are owned their full screen window
@li Panels do not participate with SurfaceManager pushing or popping
@li Panels do not draw borders
@li Panels are placed inside borders already drawn by their
    full screen owner
@li SurfaceBase contains the ncurses pWin_ WINDOW*
@li Panels do register with SurfaceManager to participate in clean shutdown
*/

} // namespace Cui

//----------------------------------------------------------------
