//----------------------------------------------------------------
//
// File: main.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsGame.h>
#include <exception>
#include <iostream>

//----------------------------------------------------------------

int
main(int argc, char* argv[])
{
    try
    {
        Ctrl::CrapsGame craps(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception: Unknown exception\n";
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------
