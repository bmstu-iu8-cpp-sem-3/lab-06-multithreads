// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <gtest/gtest.h>

#include <hash_finder_lib/string_utils.hpp>

namespace hash_finder_lib::string_utils {

    TEST(HashFinderLib, ends_with) {
        ASSERT_TRUE(ends_with("456123", "123"));
        ASSERT_FALSE(ends_with("123456", "123"));
        ASSERT_FALSE(ends_with("123", "123456"));
        ASSERT_FALSE(ends_with("123", "456123"));
    }
}
