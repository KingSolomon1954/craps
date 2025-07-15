//----------------------------------------------------------------
//
// File: MoneyTest.cpp
//
//----------------------------------------------------------------

#include <gen/Money.h>

#include <doctest/doctest.h>
#include <sstream>
#include <yaml-cpp/yaml.h>

using namespace Gen;
using namespace std;


//----------------------------------------------------------------

// Helper to parse and reformat money for tests
std::string formatMoney(const Money& m,
                        std::ios_base& (*fmt1)(std::ios_base&) = showDollar,
                        std::ios_base& (*fmt2)(std::ios_base&) = showCents)
{
    std::ostringstream oss;

    // Clear all money format flags before setting
    getMoneyFormat(oss) = 0;

    oss << fmt1 << fmt2 << m;
    return oss.str();
}

//----------------------------------------------------------------

TEST_CASE("Money::ctor")
{
    Money m1 = Money::fromDollars(12.34);
    CHECK(m1.toCents() == 1234);
    CHECK(m1.toWholeDollars() == 12);
    CHECK(doctest::Approx(m1.toDouble()) == 12.34);

    Money m2 = Money::fromWholeDollars(100);
    CHECK(m2.toCents() == 10000);
    CHECK(m2.toWholeDollars() == 100);
}

//----------------------------------------------------------------

TEST_CASE("Money::arithmetic and comparison")
{
    Money a = Money::fromDollars(50.25);
    Money b = Money::fromDollars(10.75);

    CHECK((a + b).toCents() == 6100);
    CHECK((a - b).toCents() == 3950);

    Money c = a;
    c += b;
    CHECK(c.toCents() == 6100);

    Money d = a;
    d -= b;
    CHECK(d.toCents() == 3950);

    CHECK(b < a);
    CHECK(a == Money::fromDollars(50.25));
}

//----------------------------------------------------------------

TEST_CASE("Money::string parsing")
{
    CHECK(Money::fromString("$123.45").toCents() == 12345);
    CHECK(Money::fromString("1,000.00").toCents() == 100000);
    CHECK(Money::fromString("-42.99").toCents() == -4299);
    CHECK(Money::fromString("5").toCents() == 500);
}

//----------------------------------------------------------------

TEST_CASE("Money::formatted output")
{
    Money m = Money::fromDollars(1234.56);
    std::ostringstream oss;

    oss << showDollar << showCents << m;
    CHECK(oss.str() == "$1,234.56");

    CHECK(formatMoney(m, showDollar, noCents) == "$1,234");
    CHECK(formatMoney(m, noDollar, showCents) == "1,234.56");
    CHECK(formatMoney(m, noDollar, noCents)   == "1,234");

    Money neg = Money::fromDollars(-20.50);
    CHECK(formatMoney(neg) == "-$20.50");
}

//----------------------------------------------------------------

TEST_CASE("Money::YAML roundtrip")
{
    Money m = Money::fromDollars(78.90);
    YAML::Node node = m.toYAML();

    Money restored;
    restored.fromYAML(node);
    CHECK(restored == m);
}

//----------------------------------------------------------------

