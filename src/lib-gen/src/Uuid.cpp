//----------------------------------------------------------------
//
// File: Uuid.cpp
//
//----------------------------------------------------------------

#include "gen/Uuid.h"
#include <array>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace Gen;

/*-----------------------------------------------------------*//**

Generate a UUID v4 string.

Uses C++ random.

@return
    A random UUID as a string.
*/
Uuid
Gen::generateUuid()
{
    std::random_device rd;
    std::uniform_int_distribution<uint32_t> dist(0, 255);

    std::array<uint8_t, 16> bytes{};
    for (auto& byte : bytes)
    {
        byte = static_cast<uint8_t>(dist(rd));
    }

    // Set UUID version to 4
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    // Set UUID variant to 10xx
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    // Format into UUID string
    std::ostringstream oss;
    for (size_t i = 0; i < bytes.size(); ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10) oss << '-';
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

//----------------------------------------------------------------
