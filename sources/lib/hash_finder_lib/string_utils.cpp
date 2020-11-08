// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <algorithm>
#include <hash_finder_lib/string_utils.hpp>

namespace hash_finder_lib::string_utils {

    bool ends_with(std::string const& source, std::string const& suffix) {
        auto const sourceSize = source.size(), suffixSize = suffix.size();
        if (sourceSize < suffixSize) return false; // fast failure

        return ::std::equal(source.begin() + sourceSize - suffixSize, source.end(), suffix.begin());
    }
} // namespace hash_finder_lib::string_utils
