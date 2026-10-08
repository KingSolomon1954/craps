//----------------------------------------------------------------
//
// File: utest-lib-craps.cpp
//
//----------------------------------------------------------------

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <gen/Logger.h>

int main(int argc, char** argv)
{
    Gen::Logger::instance().setOutputFile("/dev/null");
    Gen::Logger::instance().disableConsoleLogging();

    doctest::Context context(argc, argv);

    int result = context.run();

    return result;
}

//----------------------------------------------------------------

// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #include <doctest/doctest.h>

//----------------------------------------------------------------
