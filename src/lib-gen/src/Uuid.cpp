//----------------------------------------------------------------
//
// File: Uuid.cpp
//
//----------------------------------------------------------------

#include "gen/Uuid.h"
#include <random>
#include <uuid.h>

using namespace Gen;

/*-----------------------------------------------------------*//**

Return a UUID based on random number generator.

@return
    UUID as a string.
*/
std::string
Gen::generateUuid()
{
    std::random_device rd;
    auto seed_data = std::array<int, 6> {};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::ranlux48_base generator(seq);

    uuids::basic_uuid_random_generator<std::ranlux48_base> gen(&generator);
    uuids::uuid const id = gen();

    return uuids::to_string(id);
}

//----------------------------------------------------------------
