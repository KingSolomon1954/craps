//----------------------------------------------------------------
//
// File: EnumBetNameTest.cpp
//
//----------------------------------------------------------------

#include "EnumBetName.h"

#include <sstream>
#include <doctest/doctest.h>

//----------------------------------------------------------------

TEST_CASE("EnumBetNameTest: toString")
{
    std::string s;
    BetName val;

    val = BetName::Pass;
    s = EnumBetName::toString(val);
    CHECK(s == "Pass");

    val = BetName::Come;
    s = EnumBetName::toString(val);
    CHECK(s == "Come");

    val = BetName::DontPass;
    s = EnumBetName::toString(val);
    CHECK(s == "DontPass");

    val = BetName::DontCome;
    s = EnumBetName::toString(val);
    CHECK(s == "DontCome");

    val = BetName::Place;
    s = EnumBetName::toString(val);
    CHECK(s == "Place");

    val = BetName::Lay;
    s = EnumBetName::toString(val);
    CHECK(s == "Lay");

    val = BetName::Buy;
    s = EnumBetName::toString(val);
    CHECK(s == "Buy");

    val = BetName::Hardway;
    s = EnumBetName::toString(val);
    CHECK(s == "Hardway");

    val = BetName::Field;
    s = EnumBetName::toString(val);
    CHECK(s == "Field");

    val = BetName::AnyCraps;
    s = EnumBetName::toString(val);
    CHECK(s == "AnyCraps");

    val = BetName::AnySeven;
    s = EnumBetName::toString(val);
    CHECK(s == "AnySeven");

    val = BetName::CandE;
    s = EnumBetName::toString(val);
    CHECK(s == "C&E");

    val = BetName::Horn;
    s = EnumBetName::toString(val);
    CHECK(s == "Horn");
}

//----------------------------------------------------------------

TEST_CASE("EnumBetNameTest: fromString")
{
    std::string s;

    s = "pass";
    CHECK(BetName::Pass == EnumBetName::fromString(s));

    s = "COME";
    CHECK(BetName::Come == EnumBetName::fromString(s));

    s = "doNTpass";
    CHECK(BetName::DontPass == EnumBetName::fromString(s));

    s = "dontcome";
    CHECK(BetName::DontCome == EnumBetName::fromString(s));

    s = "Place";
    CHECK(BetName::Place == EnumBetName::fromString(s));

    s = "Lay";
    CHECK(BetName::Lay == EnumBetName::fromString(s));

    s = "Buy";
    CHECK(BetName::Buy == EnumBetName::fromString(s));

    s = "Hardway";
    CHECK(BetName::Hardway == EnumBetName::fromString(s));

    s = "field";
    CHECK(BetName::Field == EnumBetName::fromString(s));

    s = "anyCRAPS";
    CHECK(BetName::AnyCraps == EnumBetName::fromString(s));

    s = "ANYseven";
    CHECK(BetName::AnySeven == EnumBetName::fromString(s));

    s = "C&e";
    CHECK(BetName::CandE == EnumBetName::fromString(s));

    s = "Horn";
    CHECK(BetName::Horn == EnumBetName::fromString(s));

    CHECK_THROWS_AS((void)EnumBetName::fromString("XYZ"),
                    std::invalid_argument);
}

//---------------------------------------------------------------

TEST_CASE("EnumBetNameTest: isConvertibleFrom")
{
    std::string s;

    s = "pass";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "COME";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "DontPass";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "DontCome";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "Place";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "Lay";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "Buy";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "hardway";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "fielD";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "anycraps";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "anyseven";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "C&E";
    CHECK(EnumBetName::isConvertibleFrom(s));

    s = "horn";
    CHECK(EnumBetName::isConvertibleFrom(s));

    // uh oh, this should fail but doesn't
    s = "the rain in spain";
    CHECK(!EnumBetName::isConvertibleFrom(s));
}

//---------------------------------------------------------------

TEST_CASE("EnumBetNameTest: fromUnsigned")
{
    CHECK(BetName::Pass      == EnumBetName::fromUnsigned(1u));
    CHECK(BetName::Come      == EnumBetName::fromUnsigned(2u));
    CHECK(BetName::DontPass  == EnumBetName::fromUnsigned(3u));
    CHECK(BetName::DontCome  == EnumBetName::fromUnsigned(4u));
    CHECK(BetName::Place     == EnumBetName::fromUnsigned(5u));
    CHECK(BetName::Lay       == EnumBetName::fromUnsigned(6u));
    CHECK(BetName::Buy       == EnumBetName::fromUnsigned(7u));
    CHECK(BetName::Hardway   == EnumBetName::fromUnsigned(8u));
    CHECK(BetName::Field     == EnumBetName::fromUnsigned(9u));
    CHECK(BetName::AnyCraps  == EnumBetName::fromUnsigned(10u));
    CHECK(BetName::AnySeven  == EnumBetName::fromUnsigned(11u));
    CHECK(BetName::CandE     == EnumBetName::fromUnsigned(12u));
    CHECK(BetName::Horn      == EnumBetName::fromUnsigned(13u));
    CHECK(BetName::Invalid   == EnumBetName::fromUnsigned(99u));
}

//---------------------------------------------------------------

TEST_CASE("EnumBetNameTest: listAsString")
{
    std::string s = "Pass Come DontPass DontCome Place Lay Buy Hardway "
                    "Field AnyCraps AnySeven C&E Horn";
    CHECK(s == EnumBetName::listAsString());
}

//---------------------------------------------------------------

TEST_CASE("EnumBetNameTest: ostreamOperator")
{
    BetName bn = BetName::DontCome;

    std::stringstream ss;
    ss << bn;

    CHECK(ss.str() == "DontCome");
}

//----------------------------------------------------------------
