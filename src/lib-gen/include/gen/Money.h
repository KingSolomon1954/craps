#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <yaml-cpp/yaml.h>

namespace Gen {

//----------------------------------------
// Enum for formatting flags
//----------------------------------------

enum class MoneyFormat : uint8_t
{
    ShowDollar = 1 << 0,
    ShowCents  = 1 << 1
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

//----------------------------------------
// Money class
//----------------------------------------

class Money
{
public:
    /// @name Lifecycle
    /// @{
    Money() = default;
    explicit Money(int64_t cents);
    static Money fromDollars(double dollars);
    static Money fromWholeDollars(int64_t dollars);
    static Money fromString(const std::string& str);
    /// @}

    /// @name Modifiers
    /// @{
    Money operator+(const Money& other) const { return Money(cents_ + other.cents_); }
    Money operator-(const Money& other) const { return Money(cents_ - other.cents_); }
    Money& operator+=(const Money& other) { cents_ += other.cents_; return *this; }
    Money& operator-=(const Money& other) { cents_ -= other.cents_; return *this; }
    /// @}

    /// @name Observers
    /// @{
    int64_t toCents() const;
    int64_t toWholeDollars() const;
    double toDouble() const;
    auto operator<=>(const Money&) const = default;  // Comparison
    /// @}

    /// @name YAML
    /// @{
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    /// @}
    
    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Money& m);
    
private:
    int64_t cents_ = 0;
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
