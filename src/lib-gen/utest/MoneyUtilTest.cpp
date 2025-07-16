//----------------------------------------------------------------
//
// File: MoneyUtilTest.cpp
//
//----------------------------------------------------------------

#include <gen/MoneyUtil.h>
#include <doctest/doctest.h>

using namespace Gen;

//----------------------------------------------------------------

TEST_CASE("MoneyUtil::toString basic formatting")
{
    Money value = 123456;

    CHECK(MoneyUtil::toString(value)               == "$123,456");
    CHECK(MoneyUtil::toString(value, true, false)  == "$123456");
    CHECK(MoneyUtil::toString(value, false, false) == "123456");
    CHECK(MoneyUtil::toString(value, false, true)  == "123,456");
    CHECK(MoneyUtil::toStringPlain(value)          == "123456");
    CHECK(MoneyUtil::toStringNoDollar(value)       == "123,456");
    CHECK(MoneyUtil::toStringNoCommas(value)       == "$123456");

    Money m1 = 1;
    CHECK(MoneyUtil::toStringPlain(m1)             == "1");
    CHECK(MoneyUtil::toStringNoDollar(m1)          == "1");
    CHECK(MoneyUtil::toStringNoCommas(m1)          == "$1");
}

//----------------------------------------------------------------

TEST_CASE("MoneyUtil::fromString parsing")
{
    CHECK(MoneyUtil::fromString("$123,456") == 123456);
    CHECK(MoneyUtil::fromString("123456") == 123456);
    CHECK(MoneyUtil::fromString("$123456") == 123456);
    CHECK(MoneyUtil::fromString("123,456") == 123456);
    CHECK(MoneyUtil::fromString("1") == 1);

    CHECK_THROWS_AS(MoneyUtil::fromString("abc"), std::invalid_argument);
    CHECK_THROWS_AS(MoneyUtil::fromString("$123abc"), std::invalid_argument);
}

//----------------------------------------------------------------

TEST_CASE("MoneyUtil::YAML round-trip")
{
    Money original = 123456;

    YAML::Node node = MoneyUtil::toYAML(original);
    Money parsed = MoneyUtil::fromYAML(node);

    CHECK(original == parsed);
}

//----------------------------------------------------------------
