//----------------------------------------------------------------
//
// File: ErrorPass.cpp
//
//----------------------------------------------------------------

#include "gen/ErrorPass.h"
#include "gen/StringUtils.h"

using namespace Gen;

void
ErrorPass::prepend(const std::string& s)
{
    StringUtils::prepend(diag, s);
}

void
ErrorPass::append(const std::string& s)
{
    diag += s;
}
