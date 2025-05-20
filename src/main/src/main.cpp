//----------------------------------------------------------------
//
// File: main.cpp
//
//----------------------------------------------------------------

#include <iostream>
#include <controller/Globals.h>
#include <gen/BuildInfo.h>

//----------------------------------------------------------------

int
main(int argc, char* argv[])
{
    Gen::BuildInfo bld(Gbl::appNameScreen);
    std::cout << bld.shortInfo() << std::endl;
//  std::cout << bld.fullInfo() << std::endl;
    
    (void) argc; (void) argv;
//  Ctrl::CrapsGame craps(argc, argv);
    return 0;
}

//----------------------------------------------------------------
