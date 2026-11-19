//----------------------------------------------------------------
//
// File: LineInputTest.cpp
//
//----------------------------------------------------------------

#include <cui/core/LineInput.h>
#include <string>
#include <doctest/doctest.h>

using Cui::LineInput;

namespace {

void
type(LineInput& li, const std::string& text)
{
    for (char c : text) (void) li.handleKey(static_cast<unsigned char>(c));
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("LineInput: digits only by default")
{
    LineInput li(10);
    type(li, "12ab 3-");
    CHECK(li.value() == "123");
    CHECK_FALSE(li.handleKey('x'));       // Not handled here
}

//----------------------------------------------------------------

TEST_CASE("LineInput: player names take printable ASCII")
{
    LineInput li(16, LineInput::Charset::PlayerName);
    type(li, "Mr. O'Neil-2");
    CHECK(li.value() == "Mr. O'Neil-2");
    CHECK(li.cursorPosition() == 12);
}

//----------------------------------------------------------------

TEST_CASE("LineInput: player names refuse control and extended codes")
{
    LineInput li(16, LineInput::Charset::PlayerName);
    CHECK_FALSE(li.handleKey(27));        // Esc
    CHECK_FALSE(li.handleKey('\n'));
    CHECK_FALSE(li.handleKey(0xE9));      // Not ASCII
    CHECK_FALSE(li.handleKey(0x1000));    // ncurses KEY_ range
    CHECK(li.value().empty());
}

//----------------------------------------------------------------

TEST_CASE("LineInput: editing keys still work with names")
{
    LineInput li(16, LineInput::Charset::PlayerName);
    type(li, "Howe");
    CHECK(li.handleKey(127));             // Backspace
    type(li, "ie");
    CHECK(li.value() == "Howie");
    li.setValue("Al");
    CHECK(li.cursorPosition() == 2);
}

//----------------------------------------------------------------
