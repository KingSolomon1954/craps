//----------------------------------------------------------------
//
// File: MoneyUtil.cpp
//
//----------------------------------------------------------------

#include <gen/MoneyUtil.h>

using namespace Gen;

//----------------------------------------------------------------
//
// Convert Money to string with formatting options
//
std::string
MoneyUtil::toString(Money amount, bool showDollar, bool withCommas)
{
    std::ostringstream oss;

    if (showDollar) oss << '$';

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

//----------------------------------------------------------------

std::string
MoneyUtil::toStringNoCommas(Money amount)
{
    constexpr bool showDollar = true;
    constexpr bool showCommas = false;
    return MoneyUtil::toString(amount, showDollar, showCommas);
}

//----------------------------------------------------------------

std::string
MoneyUtil::toStringNoDollar(Money amount)
{
    constexpr bool showDollar = false;
    constexpr bool showCommas = true;
    return MoneyUtil::toString(amount, showDollar, showCommas);
}

//----------------------------------------------------------------

std::string
MoneyUtil::toStringPlain(Money amount)
{
    return std::to_string(amount);
}

//----------------------------------------------------------------
//
// Convert string to Money
//
Money
MoneyUtil::fromString(const std::string& str)
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

//----------------------------------------------------------------

YAML::Node
MoneyUtil::toYAML(Money value)
{
    YAML::Node node;
    node = value;
    return node;
}

//----------------------------------------------------------------

Money
MoneyUtil::fromYAML(const YAML::Node& node)
{
    if (!node.IsScalar())
    {
        throw std::runtime_error("Money YAML node must be scalar");
    }

    return node.as<Money>();
}

//----------------------------------------------------------------

