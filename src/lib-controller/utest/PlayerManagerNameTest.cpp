//----------------------------------------------------------------
//
// File: PlayerManagerNameTest.cpp
//
// The rules for player names and short descriptions.
//
//----------------------------------------------------------------

#include <controller/PlayerManager.h>
#include <string>
#include <doctest/doctest.h>

using Ctrl::PlayerManager;

//----------------------------------------------------------------

TEST_CASE("PlayerManager::checkName: length and characters")
{
    std::string why;
    CHECK(PlayerManager::checkName("Howie", why));
    CHECK(PlayerManager::checkName("HisRoyalMajesty", why));          // 15
    CHECK(PlayerManager::checkName(std::string(16, 'x'), why));

    CHECK_FALSE(PlayerManager::checkName("", why));
    CHECK_FALSE(PlayerManager::checkName(std::string(17, 'x'), why));
    CHECK_FALSE(PlayerManager::checkName("Caf\xC3\xA9", why));        // UTF-8
    CHECK_FALSE(PlayerManager::checkName("tab\there", why));
    CHECK_FALSE(why.empty());
}

//----------------------------------------------------------------

TEST_CASE("PlayerManager::checkDescription: may be empty, is limited")
{
    std::string why;
    CHECK(PlayerManager::checkDescription("", why));
    CHECK(PlayerManager::checkDescription("Left-handed dice thrower", why));
    CHECK(PlayerManager::checkDescription(
        std::string(PlayerManager::MaxDescriptionLength, 'x'), why));

    why.clear();
    CHECK_FALSE(PlayerManager::checkDescription(
        std::string(PlayerManager::MaxDescriptionLength + 1, 'x'), why));
    CHECK_FALSE(why.empty());

    CHECK_FALSE(PlayerManager::checkDescription("bell\a", why));
    CHECK_FALSE(PlayerManager::checkDescription("Caf\xC3\xA9", why));
}

//----------------------------------------------------------------
