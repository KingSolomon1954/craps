//----------------------------------------------------------------
//
// File: Money.cpp
//
//----------------------------------------------------------------

#include <gen/Money.h>

#include <cmath>
#include <regex>
#include <sstream>

using namespace Gen;

//----------------------------------------------------------------

Money::Money(int64_t cents)
    : cents_(cents)
{
    // empty
}

//----------------------------------------------------------------

Money
Money::fromDollars(double dollars)
{
    return Money(static_cast<int64_t>(std::round(dollars * 100)));
}

//----------------------------------------------------------------

Money
Money::fromWholeDollars(int64_t dollars)
{
    return Money(dollars * 100);
}

//----------------------------------------------------------------

int64_t
Money::toCents() const
{
    return cents_;
}

//----------------------------------------------------------------

int64_t
Money::toWholeDollars() const
{
    return cents_ / 100;
}

//----------------------------------------------------------------

double
Money::toDouble() const
{
    return static_cast<double>(cents_) / 100.0;
}

/*-----------------------------------------------------------*//**

Example usage:

@code
Money m1 = Money::fromString("$1,234.56");
Money m2 = Money::fromString("-500");

std::cout << m1 << "\n";  // $1,234.56
std::cout << m2 << "\n";  // -$500.00

std::cout << noDollar << noCents << m1 << "\n";  // 1,234
@endcode

*/
Money
Money::fromString(const std::string& str)
{
    std::string clean = str;

    // Remove leading dollar sign and commas
    clean.erase(std::remove(clean.begin(), clean.end(), '$'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), ','), clean.end());

    std::regex pattern(R"(^-?\d+(\.\d{1,2})?$)");
    if (!std::regex_match(clean, pattern)) {
        throw std::invalid_argument("Invalid money format: " + str);
    }

    double dollars = std::stod(clean);
    return fromDollars(dollars);
}

//----------------------------------------------------------------

YAML::Node
Money::toYAML() const
{
    YAML::Node node;
    node = cents_;
    return node;
}

//----------------------------------------------------------------

void
Money::fromYAML(const YAML::Node& node)
{
    if (node.IsScalar())
    {
        cents_ = node.as<int64_t>();
    }
    else
    {
        throw std::runtime_error("Money: expected scalar YAML node");
    }
}

//----------------------------------------------------------------
//
// Stream output operator
//
namespace Gen {

std::ostream& operator<<(std::ostream& os, const Money& m)
{
    long flags = getMoneyFormat(os);

    bool withDollar = flags & static_cast<long>(MoneyFormat::ShowDollar);
    bool withCents  = flags & static_cast<long>(MoneyFormat::ShowCents);

    auto absCents = std::abs(m.cents_);
    auto dollars  = absCents / 100;
    auto cents    = absCents % 100;

    if (m.cents_ < 0) os << '-';
    if (withDollar)   os << '$';

    // Format dollars with thousands separator
    std::string dollarStr = std::to_string(dollars);
    std::string withCommas;
    int count = 0;
    for (auto it = dollarStr.rbegin(); it != dollarStr.rend(); ++it)
    {
        if (count && count % 3 == 0)
        {
            withCommas.insert(0, 1, ',');
        }
        withCommas.insert(0, 1, *it);
        ++count;
    }

    os << withCommas;

    if (withCents)
    {
        os << '.' << std::setw(2) << std::setfill('0') << cents;
    }

    return os;
}

}  // namespace Gen

//----------------------------------------------------------------
