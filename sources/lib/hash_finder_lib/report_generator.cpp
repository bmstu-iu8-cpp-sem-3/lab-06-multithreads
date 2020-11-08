// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <picosha2.h>

#include <hash_finder_lib/report_generator.hpp>

namespace hash_finder_lib::report_generator {

    static void write_needle_(::std::ostream& output, ::hash_finder_lib::worker::Result const& result) {
        output << "  {\n    \"timestamp\": " << result.timestamp.count()
               << ",\n    \"hash\": \"" << ::picosha2::bytes_to_hex_string(result.hash)
               << "\",\n    \"data\": \"" << ::picosha2::bytes_to_hex_string(result.data) << "\"\n  }";
    }

    void generate_report(::std::ostream& output, std::vector<::hash_finder_lib::worker::Result> const& results) {
        output << "[\n";

        {
            auto iterator = results.cbegin();
            auto const end = results.cend();
            if (iterator != end) {
                write_needle_(output, *iterator);
                while (++iterator != end) write_needle_(output << ",\n", *iterator);
            }
        }
        if (!results.empty()) {}

        output << "\n]";
    }
} // namespace hash_finder_lib::report_generator
