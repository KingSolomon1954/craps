//----------------------------------------------------------------
//
// File: ConfigManagerTest.cpp
//
//----------------------------------------------------------------

#include <controller/ConfigManager.h>

#include <string>
#include <iostream>
#include <doctest/doctest.h>

using namespace Ctrl;

//----------------------------------------------------------------

TEST_CASE("ConfigManager::ctor")
{
    int argc = 3;
    char* argv[] = {
        const_cast<char*>("RoyalCraps"),
        const_cast<char*>("-d"),
        const_cast<char*>("--con"),
        nullptr // NULL-terminated for safety in some tools
    };
    ConfigManager cfgMgr(argc, argv);
    cfgMgr.listLayers();
}

//----------------------------------------------------------------
