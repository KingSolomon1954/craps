//----------------------------------------------------------------
//
// File: ErrorPass.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

namespace Gen {

struct ErrorPass
{
    std::string diag;
    std::string funcName;
    unsigned lineNum;

    void append(const std::string& s);
    void prepend(const std::string& s);
};

} // namespace Gen
