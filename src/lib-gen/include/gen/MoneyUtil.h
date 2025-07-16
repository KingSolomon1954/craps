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

// String conversions
std::string toString(Money amount,
                     bool showDollar = true,
                     bool withCommas = true);  // "$1,234"
std::string toStringNoCommas(Money amount);    // "$1234"
std::string toStringNoDollar(Money amount);    // "1,234"
std::string toStringPlain   (Money amount);    // "1234"
Money fromString(const std::string& str);      // Accepts "$1,234", throws

// YAML support
YAML::Node toYAML(Money value);
Money fromYAML(const YAML::Node& node);

}}  // namespace Gen::MoneyUtil

//----------------------------------------------------------------
