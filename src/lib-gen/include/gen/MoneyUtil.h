//----------------------------------------------------------------
//
// File: MoneyUtil.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace Gen {

using Money = unsigned;

namespace MoneyUtil {

// Convert Money to string with formatting options
inline std::string toString(Money amount, bool showDollar = true, bool withCommas = true)
{
    std::ostringstream oss;

    if (showDollar)
        oss << '$';

    std::string digits = std::to_string(amount);

    if (withCommas)
    {
        std::string withSep;
        int count = 0;
        for (auto it = digits.rbegin(); it != digits.rend(); ++it)
        {
            if (count && count % 3 == 0)
                withSep.insert(0, 1, ',');
            withSep.insert(0, 1, *it);
            ++count;
        }
        oss << withSep;
    }
    else
    {
        oss << digits;
    }

    return oss.str();
}

// Convert string to Money
inline Money fromString(const std::string& str)
{
    std::string clean = str;

    // Strip dollar sign and commas
    clean.erase(std::remove(clean.begin(), clean.end(), '$'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), ','), clean.end());

    try
    {
        size_t idx = 0;
        unsigned long long value = std::stoull(clean, &idx);
        if (idx != clean.size())
            throw std::invalid_argument("Trailing characters");

        if (value > std::numeric_limits<Money>::max())
            throw std::out_of_range("Money value too large");

        return static_cast<Money>(value);
    }
    catch (const std::exception& e)
    {
        throw std::invalid_argument("Invalid money string: \"" + str + "\"");
    }
}

// YAML support
inline YAML::Node toYAML(Money value)
{
    YAML::Node node;
    node = value;
    return node;
}

inline Money fromYAML(const YAML::Node& node)
{
    if (!node.IsScalar())
        throw std::runtime_error("Money YAML node must be scalar");

    return node.as<Money>();
}

}}  // namespace Gen::MoneyUtil

//----------------------------------------------------------------
