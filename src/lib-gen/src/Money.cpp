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

Money::Money(long long cents)
    : cents_(cents)
{
    // empty
}

//----------------------------------------------------------------
//
// Don't implement. Various concerns about precision, conversions,
// ambiguity with conversions, assignments and constructors.
//
// Money::Money(double dollars)
//     : cents_(static_cast<long long>(std::round(dollars * 100.00)))
// {
//     // empty
// }

//----------------------------------------------------------------

Money&
Money::operator=(long long cents)
{
    cents_ = cents;
    return *this;
}

//----------------------------------------------------------------

Money&
Money::operator=(double dollars)
{
    *this = Money::fromDollars(dollars);
    return *this;
}

//----------------------------------------------------------------

Money&
Money::operator=(const std::string& str)
{
    *this = fromString(str);
    return *this;
}

//----------------------------------------------------------------

Money
Money::fromDollars(double dollars)
{
    return Money(static_cast<long long>(std::round(dollars * 100.00)));
}

//----------------------------------------------------------------

Money
Money::fromWholeDollars(long long dollars)
{
    return Money(dollars * 100);
}

//----------------------------------------------------------------

long long
Money::toCents() const
{
    return cents_;
}

//----------------------------------------------------------------

long long
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

//----------------------------------------------------------------
//
// Convert to string default format: e.g. "$1,234.56"
//
std::string
Money::toString() const
{
    return toString(showDollar, showCents, showComma);
}

//----------------------------------------------------------------
//
// Convert to string plain format: e.g. "1,234"
//
std::string
Money::toStringPlain() const
{
    return toString(noDollar, noCents, showComma);
}

/*-----------------------------------------------------------*//**

Convert to string with formatting options.

Have to supply all three options.

Example usage:

@code

Money m = Money::fromDollars(1234.56);

std::cout << m.toString();                                  // "$1,234.56"
std::cout << m.toString(noDollar,   showCents, showComma);  // "1,234.56"
std::cout << m.toString(showDollar, noCents,   showComma);  // "$1,234"
@endcode
*/
std::string
Money::toString(std::ios_base& (*fmt1)(std::ios_base&),
                std::ios_base& (*fmt2)(std::ios_base&),
                std::ios_base& (*fmt3)(std::ios_base&)) const
{
    std::ostringstream oss;
    getMoneyFormat(oss) = 0;              // Reset formatting flags
    oss << fmt1 << fmt2 << fmt3 << *this; // Apply format and stream
    return oss.str();
}

//----------------------------------------------------------------

std::string
Money::toStringRaw() const
{
    return std::to_string(cents_);
}

//----------------------------------------------------------------

std::string
Money::toStringDebug() const
{
    std::ostringstream oss;
    oss << "Money[cents=" << cents_ << ", formatted=" << toString() << "]";
    return oss.str();
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
    if (!std::regex_match(clean, pattern))
    {
        std::string diag("Invalid money format: \"" + str + "\"");
        throw std::invalid_argument(diag);
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
        cents_ = node.as<long long>();
    }
    else
    {
        throw std::runtime_error("Money: expected scalar YAML node");
    }
}

//----------------------------------------------------------------

Money
Money::operator+(const Money& other) const
{
    return Money(cents_ + other.cents_);
}

//----------------------------------------------------------------

Money
Money::operator-(const Money& other) const
{
    return Money(cents_ - other.cents_);
}

//----------------------------------------------------------------

Money&
Money::operator+=(const Money& other)
{
    cents_ += other.cents_;
    return *this;
}

//----------------------------------------------------------------

Money&
Money::operator-=(const Money& other)
{
    cents_ -= other.cents_;
    return *this;
}

//----------------------------------------------------------------

std::strong_ordering
Money::operator<=>(long long dollars) const
{
    return cents_ <=> (dollars * 100);
}

//----------------------------------------------------------------

bool
Money::operator==(long long dollars) const
{
    return cents_ == (dollars * 100);
}

//----------------------------------------------------------------

namespace Gen {

std::strong_ordering
operator<=>(long long dollars, const Money& m)
{
    return m <=> dollars;
}
    
}  // namespace Gen

//----------------------------------------------------------------

namespace Gen {

bool
operator==(long long dollars, const Money& m)
{
    return m == dollars;
}

}  // namespace Gen

namespace Gen {

bool operator< (long long lhs, const Money& rhs) {
    return lhs <=> rhs == std::strong_ordering::less;
}

bool operator> (long long lhs, const Money& rhs) {
std::cout << "Howie lhs:" << lhs << "\n";
    return lhs <=> rhs == std::strong_ordering::greater;
}

bool operator<=(long long lhs, const Money& rhs) {
    return lhs <=> rhs != std::strong_ordering::greater;
}

bool operator>=(long long lhs, const Money& rhs) {
    return lhs <=> rhs != std::strong_ordering::less;
}

}  // namespace Gen



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
    bool withComma  = flags & static_cast<long>(MoneyFormat::ShowComma);

    auto absCents = std::abs(m.cents_);
    auto dollars  = absCents / 100;
    auto cents    = absCents % 100;

    if (m.cents_ < 0) os << '-';
    if (withDollar)   os << '$';

    // Format dollars with thousands separator
    std::string dollarStr = std::to_string(dollars);
    if (withComma)
    {
        std::string withCommas;
        int count = 0;
        for (auto it = dollarStr.rbegin(); it != dollarStr.rend(); ++it)
        {
            if (count && count % 3 == 0)
                withCommas.insert(0, 1, ',');
            withCommas.insert(0, 1, *it);
            ++count;
        }
        os << withCommas;
    }
    else
    {
        os << dollarStr;
    }

    if (withCents)
    {
        os << '.' << std::setw(2) << std::setfill('0') << cents;
    }

    return os;
}

}  // namespace Gen

//----------------------------------------------------------------
