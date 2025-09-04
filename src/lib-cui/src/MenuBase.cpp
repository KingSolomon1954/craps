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

WINDOW*
MenuBase::win() const
{
    return w_;
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
    wnoutrefresh(win());
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
        toggle();  // purely visual
    }
    else if (ch == 27)  // ESC
    {
        if (!isRoot_)
        {
            view_.popScreen();
        }
        // root menu ignores ESC
    }
    else
    {
        // Always active: handle key, regardless of visibility
        handleMenuKey(ch);
    }
}

//----------------------------------------------------------------

void
MenuBase::toggle()
{
    visible_ = !visible_;
    if (!visible_) werase(win());
    view_.redrawUnlocked();
}

//----------------------------------------------------------------

void
MenuBase::setRootMenu(bool root)
{
    isRoot_ = root;
}

//----------------------------------------------------------------

void
MenuBase::setOwningScreen(Screen* pOwning)
{
    pOwningScreen_ = pOwning;
}

//----------------------------------------------------------------
