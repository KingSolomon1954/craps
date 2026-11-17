//----------------------------------------------------------------
//
// File: PlayerManifestTest.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerManifest.h>
#include <stdexcept>
#include <string>
#include <yaml-cpp/yaml.h>
#include <doctest/doctest.h>

using Ctrl::PlayerKind;
using Ctrl::PlayerManifest;

namespace {

const char* TwoPlayers = R"(
players:
  - playerId: id-1
    kind: user
    playerName: Nathan
    shortDescription: Primary player
    fullDescription: Likes the pass line.
    fileName: nathan.yaml
  - playerId: id-2
    kind: buddy
    playerName: Arthur
    shortDescription: High roller
    fullDescription: Lots of bets.
    fileName: arthur.yaml
)";

PlayerManifest
loaded()
{
    PlayerManifest m;
    m.fromYAML(YAML::Load(TwoPlayers)["players"]);
    return m;
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("PlayerManifest: reads the kind of each player")
{
    auto m = loaded();
    REQUIRE(m.getPlayerList().size() == 2);
    CHECK((m.getPlayerList()[0].kind == PlayerKind::User));
    CHECK((m.getPlayerList()[1].kind == PlayerKind::Buddy));
}

//----------------------------------------------------------------

TEST_CASE("PlayerManifest: a missing or unknown kind is an error")
{
    PlayerManifest m;
    CHECK_THROWS_AS(m.fromYAML(YAML::Load(R"(
- playerId: id-1
  playerName: Nathan
  shortDescription: s
  fullDescription: f
  fileName: nathan.yaml
)")), std::runtime_error);

    CHECK_THROWS_AS(m.fromYAML(YAML::Load(R"(
- playerId: id-1
  kind: robot
  playerName: Nathan
  shortDescription: s
  fullDescription: f
  fileName: nathan.yaml
)")), std::runtime_error);
}

//----------------------------------------------------------------

TEST_CASE("PlayerManifest: toYAML and fromYAML round trip")
{
    auto a = loaded();
    PlayerManifest b;
    b.fromYAML(a.toYAML()["players"]);
    REQUIRE(b.getPlayerList().size() == 2);
    CHECK(b.getPlayerList()[1].playerName == "Arthur");
    CHECK((b.getPlayerList()[1].kind       == PlayerKind::Buddy));
    CHECK(b.getPlayerList()[0].fileName   == "nathan.yaml");
}

//----------------------------------------------------------------

TEST_CASE("PlayerManifest: find, add, rename and remove")
{
    auto m = loaded();

    CHECK(m.find("id-1") != nullptr);
    CHECK(m.find("nope") == nullptr);
    CHECK(m.findByName("nATHAN") != nullptr);       // ignores case
    CHECK(m.findByName("Nathaniel") == nullptr);
    CHECK(m.fileNameInUse("arthur.yaml"));
    CHECK_FALSE(m.fileNameInUse("howie.yaml"));

    PlayerManifest::PlayerDescription d;
    d.playerId = "id-3";
    d.kind = PlayerKind::User;
    d.playerName = "Howie";
    d.fileName = "howie.yaml";
    m.add(d);
    CHECK(m.getPlayerList().size() == 3);
    CHECK(m.getFileName("id-3") == "howie.yaml");

    CHECK(m.rename("id-3", "Howard"));
    CHECK(m.find("id-3")->playerName == "Howard");
    CHECK(m.getFileName("id-3") == "howie.yaml");   // file name stays
    CHECK_FALSE(m.rename("nope", "X"));

    CHECK(m.remove("id-3"));
    CHECK_FALSE(m.remove("id-3"));
    CHECK(m.getPlayerList().size() == 2);
}

//----------------------------------------------------------------

TEST_CASE("PlayerManifest: saving a manifest that was never loaded fails")
{
    PlayerManifest m;
    CHECK_THROWS_AS(m.saveToFile(), std::runtime_error);
}

//----------------------------------------------------------------
