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

    std::string toString(Money amount,
                         bool showDollar = true,
                         bool withCommas = true);

#if 0    
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
#endif

    Money fromString(const std::string& str);


    YAML::Node toYAML(Money value);
    Money fromYAML(const YAML::Node& node);

}}  // namespace Gen::MoneyUtil

//----------------------------------------------------------------
