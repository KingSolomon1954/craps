//----------------------------------------------------------------
//
// File: ViewSurface.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui
{

class ViewSurface
{
public:
    virtual ~ViewSurface() = default;

    virtual void onAttach()  {}
    virtual void onDetach()  {}
    virtual void onPause()   {}
    virtual void onResume()  {}

    virtual void draw() = 0;
    virtual void handleKey(int ch) = 0;
};


/*-----------------------------------------------------------*//**

@class ViewSurface

@brief Common abstraction for anything occupying a visible/input surface

@li Provides a generic UI surface contract
@li Allow ConsoleManager to work with just surface abstractions.
@li Has no knowledge of ScreenBase, MenuBase, or DialogBase.

*/

} // namespace Cui

//----------------------------------------------------------------
