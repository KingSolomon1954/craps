//----------------------------------------------------------------
//
// File: PlayerIdGenerator.cpp
//
//----------------------------------------------------------------

#include <fstream>
#include "PlayerIdGenerator.h"

using namespace App;

// constexpr std::string PlayerIdGenerator::idFile = "player_id.txt";

//----------------------------------------------------------------

unsigned
PlayerIdGenerator::generatePlayerId()
{
    std::ifstream inFile(idFile);
    int lastId = 0;

    if (inFile.is_open())
    {
        inFile >> lastId;
        inFile.close();
    }

    int newId = lastId + 1;

    std::ofstream outFile(idFile, std::ios::trunc);
    if (outFile.is_open())
    {
        outFile << newId;
        outFile.close();
    }

    return newId;
}

//----------------------------------------------------------------
