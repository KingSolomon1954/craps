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
    
}  // namespace Cui
