//----------------------------------------------------------------
//
// File: PanelBase.h
//
//----------------------------------------------------------------

#pragma once

#include <ncurses.h>
#include <cassert>
#include <stdexcept>
#include <string>

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
    virtual void draw() = 0;
    
    virtual void newWindow(int nlines, int ncols,
                           int topRow, int leftCol,
                           const std::string& panelName)
    {
        pWin_ = newwin(nlines, ncols, topRow, leftCol);
        if (pWin_ == nullptr)
        {
            assert(pWin_ != nullptr);
            std::string s = "PanelBase::newWindow(): "
                            "Unable to create ncurses WINDOW for: ";
            throw std::runtime_error(s + panelName);
        }
    }

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
    WINDOW* pWin_ = nullptr;
    
private:
    
};

/*-----------------------------------------------------------*//**

@class PanelBase

@brief A Window physically contained within a full screen Surface

@li Concrete Panel classes are owned their full screen window
@li Panels do not draw borders
@li Panels are placed inside borders already drawn by 
    their full screen owner
@li Base class contains the ncurses pWin_ WINDOW*
@li Provides cleanup logic common to all Panels

*/

} // namespace Cui

//----------------------------------------------------------------
