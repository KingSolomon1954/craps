//----------------------------------------------------------------
//
// File: PanelBase.h
//
//----------------------------------------------------------------

#pragma once

#include <ncurses.h>

namespace Cui {

class PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~PanelBase() = default;
    /// @}

    /// @name Modifiers
    /// @{
    virtual void releaseNcursesResources()
    {
        if (pWin_ != nullptr)
        {
            delwin(pWin_);
            pWin_ = nullptr;
        }
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

@brief A Window physically contained within a Surface

A bounded rectangular region that forms part of a larger UI.

@li Implements virtual Panel interface
@li Provides logic common for all Panel Window classes

*/

} // namespace Cui

//----------------------------------------------------------------
