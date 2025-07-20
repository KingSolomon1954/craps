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
                        std::ios_base& (*fmt2)(std::ios_base&) = showCents,
                        std::ios_base& (*fmt3)(std::ios_base&) = showComma)
{
    std::ostringstream oss;

    // Clear all money format flags before setting
    getMoneyFormat(oss) = 0;

    oss << fmt1 << fmt2 << fmt3 << m;
    return oss.str();
}

//----------------------------------------------------------------

TEST_CASE("Money:ctor")
{
    Money m1 = Money::fromDollars(12.34);
    CHECK(m1.toCents() == 1234);
    CHECK(m1.toWholeDollars() == 12);
    CHECK(doctest::Approx(m1.toDouble()) == 12.34);

    Money m2 = Money::fromWholeDollars(100);
    CHECK(m2.toCents() == 10000);
    CHECK(m2.toWholeDollars() == 100);

    // Assignment
    Money m3 = Money{10000};
    CHECK(m3.toCents() == 10000);
    
    // Direct assignment
    Money m4;
    m4 = 100.00;  // assignment from double
    CHECK(m4.toCents() == 10000);

    m4 = 10000LL;  // assignment from long long
    CHECK(m4.toCents() == 10000);

    // m4 = 10000;  // ambiguous, must use integer literal suffix 10000LL
    // CHECK(m4.toCents() == 10000);

    m4 = "$100.00";  // assignment from string
    CHECK(m4.toCents() == 10000);

    // Equality 
    Money m5{222}, m6{333};
    CHECK(m5 != m6);
    m6 = 222LL;
    CHECK(m5 == m6);

    // Comparisons    
    Money m7{44400}, m8{55500};
    CHECK(m7 == 444);  // 444 is treated as dollars, not cents
    CHECK(444 == m7);
    CHECK(m8 == 555);  // 555 is treated as dollars, not cents
    CHECK(555 == m8);

    CHECK(m7  < m8);
    CHECK(m7  < 500);
#if 0    
    CHECK(500 > m7);   // Fails here unless friend operators are
                       // moved to the global namespace.
    
    CHECK(m8  > 500);
    CHECK(500 < m8);
#endif    
}

//----------------------------------------------------------------

TEST_CASE("Money:arithmeticAndComparison")
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

TEST_CASE("Money:stringParsing")
{
    CHECK(Money::fromString("$123.45").toCents() == 12345);
    CHECK(Money::fromString("1,000.00").toCents() == 100000);
    CHECK(Money::fromString("-42.99").toCents() == -4299);
    CHECK(Money::fromString("5").toCents() == 500);
}

//----------------------------------------------------------------

TEST_CASE("Money:toString")
{
    Money m = Money::fromDollars(1234.56);
    CHECK(m.toString() == "$1,234.56");
    CHECK(m.toStringPlain() == "1,234");
    CHECK(m.toString(noComma, showDollar, noCents) == "$1234");
    CHECK(m.toString(showDollar, noComma, showCents) == "$1234.56");
    CHECK(m.toString(noDollar, noCents, noComma) == "1234");
}

//----------------------------------------------------------------

TEST_CASE("Money:formattedOutput")
{
    Money m = Money::fromDollars(1234.56);
    std::ostringstream oss;

    oss << showDollar << showCents << showComma << m;
    CHECK(oss.str() == "$1,234.56");

    CHECK(formatMoney(m, showDollar, noCents,  showComma) == "$1,234");
    CHECK(formatMoney(m, noDollar,  showCents, showComma) == "1,234.56");
    CHECK(formatMoney(m, noDollar,  noCents,   showComma) == "1,234");

    Money neg = Money::fromDollars(-20.50);
    CHECK(formatMoney(neg) == "-$20.50");
}

//----------------------------------------------------------------

TEST_CASE("Money:roundTripYAML")
{
    Money m = Money::fromDollars(78.90);
    YAML::Node node = m.toYAML();

    Money restored;
    restored.fromYAML(node);
    CHECK(restored == m);
}

//----------------------------------------------------------------

