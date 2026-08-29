//----------------------------------------------------------------
//
// File: ScreenCrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/ScreenBase.h>
#include <cui/WindowHeader.h>
#include <cui/WindowRollHistory.h>
#include <cui/WindowWindowPlayerArea.h>
#include <cui/WindowMessages.h>
#include <cui/WindowAnimation.h>
#include <cui/WindowHouseBrief.h>
#include <cui/WindowPlayerBrief.h>
#include <ncurses.h>

namespace Cui {

class ScreenCrapsTable : public ScreenBase
{
public:
    /// @name Lifecycle
    /// @{
   ~ScreenCrapsTable() = default;
    void releaseNcursesResources() override;
    /// @}
    
    /// @name Modifiers
    /// @{
    void draw()                     override;  // from Surface
    void onPause()                  override;  // from Surface
    void onResume()                 override;  // from Surface
    void handleKey(int ch)          override;  // from Surface
    void onDetach()                 override;  // from Surface
    void onAttach(Surface* pParent) override;  // from Surface

    void setAllPlayersView();                  // MenuPlayerViews calls this
    void setNextPlayerView();                  // MenuPlayerViews calls this
    void setPrevPlayerView();                  // MenuPlayerViews calls this
    
    WINDOW* lendWindow();                      // Just for WindowPlayerArea
    /// @}

    /// @name Observers
    /// @{
    static ScreenCrapsTable& instance();
    /// @}

private:
    // Order doesn't matter    

    // These window classes are not Surfaces.
    // They don't get pushed/popped or process input keys.
    // But they do have their own ncurses WINDOW*.
    // 
    WindowHeader           wHeader_;
    WindowRollHistory      wRollHistory_;
    WindowWindowPlayerArea wPlayerArea_;
    WindowMessages         wMessages_;
    WindowAnimation        wAnimation_;
    WindowHouseBrief       wHouseBrief_;
    WindowPlayerBrief      wPlayerBrief_;
    
    ScreenCrapsTable();  // private ctor
    void drawNavBar();
};

/*-----------------------------------------------------------*//**

@class ScreenCrapsTable

@brief Craps Table Screen implementation

@li owns size of each window
@li relationship between windows
@li screen-level borders
@li screen-level junctions
@li overall composition

*/

// TODO: subscribe to players leaving/joining table
// TODO: update PlayerList upon notification

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
