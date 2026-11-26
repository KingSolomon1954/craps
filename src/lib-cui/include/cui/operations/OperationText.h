//----------------------------------------------------------------
//
// File: OperationText.h
//
// Small text helpers for the operations' prompts and validators.
//
//----------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace Cui {

/// The text without leading or trailing white space
inline std::string
trimmed(const std::string& s)
{
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) b++;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) e--;
    return s.substr(b, e - b);
}

/// Whether the two are the same apart from upper and lower case
inline bool
sameIgnoringCase(const std::string& a, const std::string& b)
{
    return a.size() == b.size() &&
        std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
            return std::tolower(static_cast<unsigned char>(x)) ==
                   std::tolower(static_cast<unsigned char>(y)); });
}

} // namespace Cui

//----------------------------------------------------------------
