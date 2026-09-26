//----------------------------------------------------------------
//
// File: CuiStructs.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui
{

struct WindowSize
{
    int rows = 0;
    int cols = 0;
};

struct WindowPosition
{
    int row = 0;
    int col = 0;
};

struct WindowRect
{
    int row  = 0;
    int col  = 0;
    int rows = 0;
    int cols = 0;

    int bottom() const { return row + rows - 1; }
    int right()  const { return col + cols - 1; }
};

enum class Direction
{
    Right,
    Left,
    Below,
    Above
};

enum class LocationKind
{
    Menu,
    Dialog,
    Panel,
    FullScreen,
    Independent
};

struct LocationRequest
{
    WindowSize size;
    LocationKind kind = LocationKind::Menu;
    Direction direction = Direction::Right;
};

//----------------------------------------------------------------

}  // namespace Cui


namespace ColorPairs
{
    constexpr short DefaultScreen  = 1;
    constexpr short SevenOut       = 2;
    constexpr short PassLineWinner = 3;
    constexpr short Point          = 4;

    constexpr short Player1        = 5;
    constexpr short Player2        = 6;
    constexpr short Player3        = 7;
    constexpr short Player4        = 8;
    constexpr short Player5        = 9;
    constexpr short Player6        = 10;
}

//----------------------------------------------------------------
