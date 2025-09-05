//----------------------------------------------------------------
//
// File: MenuBetting.cpp
//
//----------------------------------------------------------------

#include <cui/MenuBetting.h>
#include <cui/ConsoleView.h>
#include <cui/ScreenCrapsTable.h>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting(ConsoleView& view)
    : MenuBase(view, 5, 6)  // TODO Howie
{
}

//----------------------------------------------------------------

MenuBetting::~MenuBetting()
{
}

//----------------------------------------------------------------

void
MenuBetting::setOwningScreen(ScreenCrapsTable* pOwning)
{
    pOwning_ = pOwning;
}

//----------------------------------------------------------------
//
// Override Screen base class
//
void
MenuBetting::drawMenu()
{
    // TODo
}

 //----------------------------------------------------------------
//
// Override Screen base class
//
void
MenuBetting::handleMenuKey(int ch)
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
