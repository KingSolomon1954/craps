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
    explicit Screen(ConsoleView& view) : view_(view) {}
    virtual ~Screen() = default;

    // Lifecycle hooks
    virtual void onAttach() {}  // became top (pushed or set)
    virtual void onDetach() {}  // removed from stack
    virtual void onPause()  {}  // covered by another screen
    virtual void onResume() {}  // uncovered and back on top
    
    // Required
    virtual void draw() = 0;    // call wnoutrefresh() on owned windows
    virtual void handleKey(int ch) = 0;

protected:
    enum class InputMode
    {
        Menu,   // single-key
        Amount, // line input for bet ammounts
        Line    // line input with Enter
    };

    ConsoleView& view_;
    InputMode inputMode_;
};

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
