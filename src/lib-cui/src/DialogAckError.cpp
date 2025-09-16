//----------------------------------------------------------------
//
// File: DialogAckError.cpp
//
//----------------------------------------------------------------

#include <cui/DialogAckError.h>
#include <cassert>
#include <cstring>
#include <cui/ConsoleView.h>
#include <gen/StringUtils.h>  // For wrapText()

using namespace Cui;

//----------------------------------------------------------------

DialogAckError::DialogAckError()
    : MenuBase(1, 1)  // Placeholder, resized in setMessage()
{
    visible_ = true;
}

//----------------------------------------------------------------

DialogAckError*
DialogAckError::instance()
{
    static DialogAckError dialogAckError;
    return &dialogAckError;
}

//----------------------------------------------------------------

DialogAckError::~DialogAckError()
{
}

//----------------------------------------------------------------

void
DialogAckError::setMessage(const std::string& msg)
{
    message_ = msg;
}

//----------------------------------------------------------------
//
// Override menu base class
//
// * Error message gets wrapped into multiple lines.
// * Box resizes accordingly (up to 80 cols).
// * Prompt is centered at the bottom.
//
void
DialogAckError::drawMenu()
{
    assert(w_ && "DialogAckError: WINDOW not initialized");

    int screenRows, screenCols;
    getmaxyx(stdscr, screenRows, screenCols);

    const int maxDialogWidth = 80;

    // Wrap message into multiple lines (max width = 76 chars for padding)
    int wrapWidth = std::min(maxDialogWidth - 4, screenCols - 4);
    std::vector<std::string> lines = Gen::StringUtils::wrapText(message_, wrapWidth);
    int msgLines = static_cast<int>(lines.size());

    // Box dimensions
    int boxWidth = std::min(maxDialogWidth, screenCols - 2);
    int boxHeight = msgLines + 4; // border + message lines + prompt

    // Center the box
    int startY = (screenRows - boxHeight) / 2;
    int startX = (screenCols - boxWidth) / 2;

    // Recreate window to fit new size
    if (w_) delwin(w_);
    w_ = newwin(boxHeight, boxWidth, startY, startX);

    // Draw border
    box(w_, 0, 0);

    // Print each wrapped message line
    for (size_t i = 0; i < lines.size(); ++i)
    {
        // Left pad with 2 for margin inside the box
        mvwprintw(w_, 1 + static_cast<int>(i), 2, "%s", lines[i].c_str());
    }

    // Print prompt at bottom
    const char* prompt = "Press any key to continue";
    int promptWidth = static_cast<int>(strlen(prompt));
    int promptX = std::max(1, (boxWidth - promptWidth) / 2);
    mvwprintw(w_, boxHeight - 2, promptX, "%s", prompt);

    // Refresh for drawing
    wnoutrefresh(w_);
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
DialogAckError::handleMenuKey(int ch)
{
    clearState();
    switch (ch)
    {
    default: doAck(); break;
    }
}

//----------------------------------------------------------------

void
DialogAckError::doAck()
{
    view_.popScreen();
}    

//----------------------------------------------------------------

void
DialogAckError::clearState()
{
    // Empty
}    

//----------------------------------------------------------------
