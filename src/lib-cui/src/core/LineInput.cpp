//----------------------------------------------------------------
//
// File: LineInput.cpp
//
//----------------------------------------------------------------

#include <cui/core/LineInput.h>
#include <ncurses.h>
#include <cctype>
#include <utility>

namespace Cui {

//----------------------------------------------------------------

LineInput::LineInput(std::size_t maxChars, Charset charset)
    : maxChars_(maxChars)
    , charset_(charset)
{
}

//----------------------------------------------------------------

bool
LineInput::accepts(int ch) const
{
    // Negative values and ncurses KEY_ codes (above 255) never match.
    if (ch < 0 || ch > 255) return false;

    if (charset_ == Charset::Digits)
    {
        return std::isdigit(static_cast<unsigned char>(ch)) != 0;
    }
    return ch >= 0x20 && ch <= 0x7e;
}

//----------------------------------------------------------------

bool
LineInput::handleKey(int ch)
{
    if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
    {
        if (cursorPos_ > 0)
        {
            value_.erase(cursorPos_ - 1, 1);
            --cursorPos_;
        }
        return true;
    }

    if (ch == KEY_DC)
    {
        if (cursorPos_ < value_.size())
            value_.erase(cursorPos_, 1);
        return true;
    }

    if (ch == KEY_LEFT)
    {
        if (cursorPos_ > 0)
            --cursorPos_;
        return true;
    }

    if (ch == KEY_RIGHT)
    {
        if (cursorPos_ < value_.size())
            ++cursorPos_;
        return true;
    }

    if (accepts(ch))
    {
        if (value_.size() >= maxChars_)
        {
            beep();
            return true;
        }

        value_.insert(cursorPos_, 1, static_cast<char>(ch));
        ++cursorPos_;
        return true;
    }

    return false;
}

//----------------------------------------------------------------

void
LineInput::clear()
{
    value_.clear();
    cursorPos_ = 0;
}

//----------------------------------------------------------------

void
LineInput::setValue(std::string value)
{
    if (value.size() > maxChars_)
        value.resize(maxChars_);

    value_ = std::move(value);
    cursorPos_ = value_.size();
}

} // namespace Cui

//----------------------------------------------------------------
