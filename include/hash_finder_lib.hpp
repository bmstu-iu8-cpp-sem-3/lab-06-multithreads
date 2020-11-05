// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#ifndef INCLUDE_HASH_FINDER_LIB_HPP_
#define INCLUDE_HASH_FINDER_LIB_HPP_

#include <picosha2.h>

#include <hash_finder_lib.hpp>
#include <string>

namespace hash_finder_lib {

    bool ends_with(std::string const& source, std::string const& suffix);
} // namespace hash_finder_lib

#endif // INCLUDE_HASH_FINDER_LIB_HPP_
