//----------------------------------------------------------------
//
// File: PlayerPicker.cpp
//
//----------------------------------------------------------------

#include <fstream>
#include <string>
#include "PlayerPicker.h"

using namespace App;

//----------------------------------------------------------------

PlayerPicker::PlayerPicker()
{
    std::string idFile("someFile");
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
}

//----------------------------------------------------------------
