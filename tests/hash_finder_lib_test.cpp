// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <gtest/gtest.h>

#include <hash_finder_lib.hpp>

namespace hash_finder_lib {

    TEST(HashFinderLib, ends_with) {
        ASSERT_TRUE(::hash_finder_lib::ends_with("456123", "123"));
        ASSERT_FALSE(::hash_finder_lib::ends_with("123456", "123"));
        ASSERT_FALSE(::hash_finder_lib::ends_with("123", "123456"));
        ASSERT_FALSE(::hash_finder_lib::ends_with("123", "456123"));
    }
}
