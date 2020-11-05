// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>
#include <hash_finder_lib.hpp>

#include <algorithm>

bool hash_finder_lib::ends_with(std::string const& source, std::string const& suffix) {
    auto const sourceSize = source.size(), suffixSize = suffix.size();
    if (sourceSize < suffixSize) return false; // fast failure

    return ::std::equal(source.begin() + sourceSize - suffixSize, source.end(), suffix.begin());
}
