//----------------------------------------------------------------
//
// File: DialogBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Surface.h>

namespace Cui {

class WINDOW; // fwd

class DialogBase : public Surface
{
public:
    /// @name Lifecycle
    /// @{
   ~DialogBase() { if (pWin_) delwin(pWin_); }
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

protected:
    WINDOW* pWin_ = nullptr;

private:
    
};

/*-----------------------------------------------------------*//**

@class DialogBase

@brief Specialized surface type for dialog boxes

@li Implements virtual Surface interface
@li Provides logic common for all dialog classes

*/

} // namespace Cui

//----------------------------------------------------------------
