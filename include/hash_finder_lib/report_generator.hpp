// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#ifndef INCLUDE_HASH_FINDER_LIB_REPORT_GENERATOR_HPP_
#define INCLUDE_HASH_FINDER_LIB_REPORT_GENERATOR_HPP_

#include <hash_finder_lib/worker.hpp>
#include <ostream>
#include <vector>

namespace hash_finder_lib::report_generator {

    void generate_report(::std::ostream& output, std::vector<::hash_finder_lib::worker::Result> const& results);
}

#endif // INCLUDE_HASH_FINDER_LIB_REPORT_GENERATOR_HPP_
