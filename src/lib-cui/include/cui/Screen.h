//----------------------------------------------------------------
//
// File: Screen.h
//
//----------------------------------------------------------------

#pragma once
#include <string>
#include <ncurses.h>

namespace Cui {

class ConsoleView;  // fwd
    
class Screen
{
public:
    virtual ~Screen() = default;

    // Lifecycle hooks
    virtual void onAttach() {}   // became top (pushed or set)
    virtual void onDetach() {}   // removed from stack
    virtual void onPause()  {}   // covered by another screen
    virtual void onResume() {}   // uncovered and back on top
    virtual void onActivate(ConsoleView& view) {} // optional hook
    
    // Required
    virtual void draw() = 0;     // call wnoutrefresh() on owned windows
    virtual void handleInput(const std::string& input) = 0;
};

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
