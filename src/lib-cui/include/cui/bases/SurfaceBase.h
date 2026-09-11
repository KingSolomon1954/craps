//----------------------------------------------------------------
//
// File: SurfaceBase.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/CuiStructs.h>
#include <ncurses.h>
#include <string>

namespace Cui
{

class SurfaceBase
{
public:
    enum class OperationResult
    {
        Unset,
        Cancel,
        Error,
        Success,
        Yes,
        No
    };

    explicit SurfaceBase(std::string surfaceName);
    virtual ~SurfaceBase() = default;

    virtual void draw() = 0;
    virtual bool handleKey(int ch) = 0;
    
    virtual void onAttach(SurfaceBase* pParent);
    virtual void onDetach();
    virtual void onPause();
    virtual void onResume();
    virtual void setOperationResult(OperationResult r);
    virtual bool shouldSkip() const;
    virtual void releaseNcursesResources();
    virtual void newWindow(int nlines, int ncols,
                           int topRow, int leftCol);
    virtual void setLocation(WindowPosition position);
    virtual LocationRequest getLocationRequest() const;

    const std::string& surfaceName() const;

protected:
    WINDOW*         pWin_            = nullptr;
    SurfaceBase*    pParentSurface_  = nullptr;
    OperationResult operationResult_ = OperationResult::Unset;
    bool            shouldSkip_      = false;

private:
    const std::string surfaceName_;
};

/*-----------------------------------------------------------*//**

@class SurfaceBase

@brief Common abstraction for anything occupying a visible/input surface

@li Provides a generic UI surface contract
@li Allow SurfaceManager to work with just surface abstractions
@li Has no knowledge of ScreenBase, MenuBase, DialogBase or PanelBase

*/

} // namespace Cui

//----------------------------------------------------------------
