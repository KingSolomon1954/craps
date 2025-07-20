//----------------------------------------------------------------
//
// File: Money.h
//
// This is close to being usable, but it is not finished.
// Needs more work for comparison operators against
// standard C++ types. Need to move the associated friend 
// operators into unamed namespace instead of in namespace Gen.
//
// So this doesn't yet work:
// 
// Money m{500};
// if (600 > m)  // This compiles but doesn't work.
// if (m < 600)  // This compiles and works fine.
// Compiler doesn't see our friend operator<(long long, Money);
//
//----------------------------------------------------------------

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <yaml-cpp/yaml.h>

namespace Gen {

// Enum for formatting flags
enum class MoneyFormat : uint8_t
{
    ShowDollar = 1 << 0,
    ShowCents  = 1 << 1,
    ShowComma  = 1 << 2
};

// Per-stream storage for flags
inline long& getMoneyFormat(std::ios_base& ios)
{
    static int index = std::ios_base::xalloc();
    return ios.iword(index);  // returns long&
}

// Manipulators
inline std::ios_base& showDollar(std::ios_base& os)
{
    getMoneyFormat(os) |= static_cast<long>(MoneyFormat::ShowDollar);
    return os;
}

inline std::ios_base& noDollar(std::ios_base& os)
{
    getMoneyFormat(os) &= ~static_cast<long>(MoneyFormat::ShowDollar);
    return os;
}

inline std::ios_base& showCents(std::ios_base& os)
{
    getMoneyFormat(os) |= static_cast<long>(MoneyFormat::ShowCents);
    return os;
}

inline std::ios_base& noCents(std::ios_base& os)
{
    getMoneyFormat(os) &= ~static_cast<long>(MoneyFormat::ShowCents);
    return os;
}

inline std::ios_base& showComma(std::ios_base& os)
{
    getMoneyFormat(os) |= static_cast<long>(MoneyFormat::ShowComma);
    return os;
}

inline std::ios_base& noComma(std::ios_base& os)
{
    getMoneyFormat(os) &= ~static_cast<long>(MoneyFormat::ShowComma);
    return os;
}

//----------------------------------------
// Money class
//----------------------------------------

class Money
{
public:
    /// @name Lifecycle
    /// @{
    Money() = default;
    explicit Money(long long cents);
    static Money fromDollars     (double dollars);
    static Money fromWholeDollars(long long dollars);
    static Money fromString      (const std::string& str);  // throws

    // Assignment operators
    Money& operator=(long long cents);  // Use integer literals to disambiguate
    Money& operator=(double dollars);
    Money& operator=(const std::string& str); // Parse "$12.34", throws
    /// @}

    /// @name Modifiers
    /// @{
    Money  operator+ (const Money& other) const;
    Money  operator- (const Money& other) const;
    Money& operator+=(const Money& other);
    Money& operator-=(const Money& other);
    /// @}

    /// @name Observers
    /// @{
    long long toCents()        const;
    long long toWholeDollars() const;
    double    toDouble()       const;
    // Compiler creates relational operators
    bool operator== (const Money&) const = default;
    auto operator<=>(const Money&) const = default;

    // Support comparisons with built-in types
    std::strong_ordering operator<=>(long long dollars) const;
    bool operator==(long long dollars) const;

    // Symmetric friend versions:
    friend std::strong_ordering operator<=>(long long dollars, const Money& m);
    friend bool operator==(long long dollars, const Money& m);
    friend bool operator< (long long lhs, const Money& rhs);
    friend bool operator> (long long lhs, const Money& rhs);
    friend bool operator<=(long long lhs, const Money& rhs);
    friend bool operator>=(long long lhs, const Money& rhs);
    /// @}

    /// @name Strings
    /// @{
    std::string toString()      const;  // "$1,234.56"
    std::string toStringPlain() const;  // "1,234"  no '$', no cents, yes commas
    std::string toStringRaw()   const;  // "123456"
    std::string toStringDebug() const;  // Money[cents=123456, formatted=$1,234.56]

    // Full formatting control e.g.
    // m.toString(showDollar, showCents, noComma)
    std::string toString(std::ios_base& (*fmt1)(std::ios_base&),
                         std::ios_base& (*fmt2)(std::ios_base&),
                         std::ios_base& (*fmt3)(std::ios_base&)) const;
    /// @}

    /// @name YAML
    /// @{
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    /// @}

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Money& m);

private:
    long long cents_ = 0;
};

/*-----------------------------------------------------------*//**

@class Money

@brief Manipulates Money.

Has these capabilities:

@li Internal storage in int64_t cents_
@li Whole-dollar convenience functions
@li YAML serialization
@li Flexible operator<< formatting using custom I/O manipulators:

        Show/hide dollar sign: showDollar, noDollar

        Show/hide cents: showCents, noCents

Eample usages:

@code
Money bet = Money::fromDollars(25.50);
Money win = Money::fromWholeDollars(100);

// Default format
std::cout << bet << "\n";                // $25.50

// No cents
std::cout << showDollar << noCents << bet << "\n";  // $25

// No dollar sign
std::cout << noDollar << showCents << win << "\n";  // 100.00

// Raw cents
std::cout << "Stored as: " << bet.toCents() << " cents\n";  // 2550

// YAML
YAML::Node node = win.toYAML();
Money restored;
restored.fromYAML(node);
std::cout << "Restored: " << restored << "\n";
@endcode

*/

}  // namespace Gen

//----------------------------------------------------------------
