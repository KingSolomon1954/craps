//----------------------------------------------------------------
//
// File: LineInput.h
//
//----------------------------------------------------------------

#pragma once

#include <cstddef>
#include <string>

namespace Cui
{

/**
 * @brief Edits a bounded, single-line string using ncurses key codes.
 *
 * LineInput owns the input buffer and cursor position, but does not draw
 * anything. The owning surface is responsible for rendering value() and
 * positioning the terminal cursor at cursorPosition().
 */
class LineInput
{
public:
    /// Which characters the field accepts.
    enum class Charset
    {
        Digits,      ///< 0-9 only, for amounts
        PlayerName   ///< printable ASCII, letters, digits, space, punctuation
    };

    explicit LineInput(std::size_t maxChars, Charset charset = Charset::Digits);

    /// Process an editing key. Returns false for keys not handled here.
    bool handleKey(int ch);

    void clear();
    void setValue(std::string value);

    const std::string& value()   const noexcept { return value_; }
    std::size_t cursorPosition() const noexcept { return cursorPos_; }
    std::size_t maxChars()       const noexcept { return maxChars_; }

private:
    std::string value_;
    std::size_t cursorPos_ = 0;
    std::size_t maxChars_;
    Charset     charset_;

    bool accepts(int ch) const;
};

} // namespace Cui

//----------------------------------------------------------------
