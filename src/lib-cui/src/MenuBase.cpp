//----------------------------------------------------------------
//
// File: MenuBase.cpp
//
//----------------------------------------------------------------

#include <cui/MenuBase.h>
#include <cui/ConsoleView.h>

using namespace Cui;

//----------------------------------------------------------------

MenuBase::MenuBase(ConsoleView& view, int h, int w)
    : Screen(view)
    , w_(view.makeCenteredWindow(h,w))
{
}

//----------------------------------------------------------------

MenuBase::~MenuBase()
{
    if (w_) delwin(w_);
}

//----------------------------------------------------------------

Screen::ScreenType
MenuBase::type() const
{
    return Screen::ScreenType::Overlay;
}

//----------------------------------------------------------------
//
// Override Screen base class
//
void
MenuBase::draw()
{
    if (!visible_) return;  // only paint if visible
    drawMenu();
    wnoutrefresh(w_);
}

//----------------------------------------------------------------
//
// Override Screen base class
//
void
MenuBase::handleKey(int ch)
{
    if (ch == ' ')
    {
        toggle();  // Toggle menu's visibility 
    }
    else if (ch == 27)  // ESC
    {
        if (!isRoot_)
        {
            isCanceled_ = true;
            view_.popScreen();
        }
        // Root menu ignores ESC
    }
    else
    {
        handleMenuKey(ch);  // Always active
    }
}

//----------------------------------------------------------------

void
MenuBase::toggle()
{
    visible_ = !visible_;
    if (!visible_) werase(w_);
    view_.redrawUnlocked();
}

//----------------------------------------------------------------

void
MenuBase::setRootMenu(bool root)
{
    isRoot_ = root;
}

//----------------------------------------------------------------
