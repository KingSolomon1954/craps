//----------------------------------------------------------------
//
// File: MoneyUtilsTest.cpp
//
//----------------------------------------------------------------

#include <gen/MoneyUtils.h>
#include <doctest/doctest.h>

using namespace Gen;

//----------------------------------------------------------------

TEST_CASE("MoneyUtils:toStringBasicFormatting")
{
    Money value = 123456;

    CHECK(MoneyUtils::toString(value)               == "$123,456");
    CHECK(MoneyUtils::toString(value, true, false)  == "$123456");
    CHECK(MoneyUtils::toString(value, false, false) == "123456");
    CHECK(MoneyUtils::toString(value, false, true)  == "123,456");
    CHECK(MoneyUtils::toStringPlain(value)          == "123456");
    CHECK(MoneyUtils::toStringNoDollar(value)       == "123,456");
    CHECK(MoneyUtils::toStringNoCommas(value)       == "$123456");

    Money m1 = 1;
    CHECK(MoneyUtils::toStringPlain(m1)             == "1");
    CHECK(MoneyUtils::toStringNoDollar(m1)          == "1");
    CHECK(MoneyUtils::toStringNoCommas(m1)          == "$1");
}

//----------------------------------------------------------------

TEST_CASE("MoneyUtils:fromStringParsing")
{
    CHECK(MoneyUtils::fromString("$123,456") == 123456);
    CHECK(MoneyUtils::fromString("123456") == 123456);
    CHECK(MoneyUtils::fromString("$123456") == 123456);
    CHECK(MoneyUtils::fromString("123,456") == 123456);
    CHECK(MoneyUtils::fromString("1") == 1);

    CHECK_THROWS_AS(MoneyUtils::fromString("abc"), std::invalid_argument);
    CHECK_THROWS_AS(MoneyUtils::fromString("$123abc"), std::invalid_argument);
}

//----------------------------------------------------------------

TEST_CASE("MoneyUtils:RoundTripYAML")
{
    Money original = 123456;

    YAML::Node node = MoneyUtils::toYAML(original);
    Money parsed = MoneyUtils::fromYAML(node);

    CHECK(original == parsed);
}

//----------------------------------------------------------------
