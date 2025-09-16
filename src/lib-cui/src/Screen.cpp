//----------------------------------------------------------------
//
// File: Screen.cpp
//
//----------------------------------------------------------------

#include <cui/Screen.h>
#include <cui/ConsoleView.h>

using namespace Cui;

//----------------------------------------------------------------

Screen::Screen()
    : view_(*ConsoleView::instance())
{
}

//----------------------------------------------------------------

void
Screen::onAttach()
{
    // Overridable. Base class does nothing.
}

//----------------------------------------------------------------

void
Screen::onDetach()
{
    // Overridable. Base class does nothing.
}

//----------------------------------------------------------------

void
Screen::onPause()
{
    // Overridable. Base class does nothing.
}

//----------------------------------------------------------------

void
Screen::onResume()
{
    // Overridable. Base class does nothing.
}

//----------------------------------------------------------------

Screen::ScreenType
Screen::type() const
{
    // Overridable. Base class assumes most screens are full screen.
    return ScreenType::Full;
}

//----------------------------------------------------------------
