// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#ifndef INCLUDE_RANDOM_SEED_GENERATOR_HPP_
#define INCLUDE_RANDOM_SEED_GENERATOR_HPP_

#include <cstddef>
// include target-specific intrinsics required for good random seeding
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace hash_finder_lib::random_seed_generator {

#ifdef __always_inline
    __always_inline
#else
    inline
#endif
    ::std::uintmax_t good_random_seed() { return __rdtsc(); }
} // namespace hash_finder_lib

#endif // INCLUDE_RANDOM_SEED_GENERATOR_HPP_
