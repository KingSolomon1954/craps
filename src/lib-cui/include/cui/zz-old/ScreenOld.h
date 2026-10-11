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
    enum class ScreenType
    {
        Full,
        Overlay
    };

    explicit Screen();
    virtual ~Screen() = default;

    // Lifecycle hooks
    virtual void onAttach();  // became top (pushed or set)
    virtual void onDetach();  // removed from stack
    virtual void onPause();   // covered by another screen
    virtual void onResume();  // uncovered and back on top
    virtual ScreenType type() const;
    
    // Required
    virtual void draw() = 0;  // calls wnoutrefresh() on owned windows
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
