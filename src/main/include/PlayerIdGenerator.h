//----------------------------------------------------------------
//
// File: PlayerIdGenerator.h
//
//----------------------------------------------------------------

#pragma once

#include <string>

namespace App {

class PlayerIdGenerator
{
public:
    static unsigned generatePlayerId();

private:
    static constexpr std::string idFile = "player_id.txt";
};

} // namespace App


//----------------------------------------------------------------
