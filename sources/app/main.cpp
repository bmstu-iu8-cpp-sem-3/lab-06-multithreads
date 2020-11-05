// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/keywords/file_name.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/keywords/rotation_size.hpp>
#include <boost/log/keywords/time_based_rotation.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <cstddef>
#include <hash_finder_lib.hpp>
#include <iostream>
#include <string>
#include <thread>

static void setup_logging();

int main(int const arguments_count, char const* arguments[]) {
    ::std::size_t m;
    if (arguments_count > 2) {
        try {
            m = ::std::stoul(arguments[1]);
        } catch (::std::invalid_argument const& e) {
            ::std::cerr << "Invalid m: expected positive number but got \"" << arguments[1] << "\"" << std::endl;
            return 1;
        } catch (::std::out_of_range const& e) {
            ::std::cerr << "Invalid m: value " << arguments[1] << " is too big" << std::endl;
            return 1;
        }
        if (m == 0) {
            ::std::cerr << "Invalid m: expected positive number but got 0" << std::endl;
            return 1;
        }
    } else {
        m = ::std::thread::hardware_concurrency();
        if (m == 0) m = 1; // minimal value
    }

    assert(m > 0);

    setup_logging();
}

void setup_logging() {
    namespace logging = ::boost::log;
    namespace sources = logging::sources;
    namespace expressions = logging::expressions;
    namespace keywords = logging::keywords;
    namespace sinks = logging::sinks;

    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);

    auto const format = expressions::stream
                        << "["
                        << expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
                        << "]"
                        << " (" << logging::trivial::severity << "): " << expressions::smessage;

    logging::add_console_log()->set_formatter(format);
    logging::add_file_log(keywords::file_name = "logs/file_%5N.log", keywords::rotation_size = 5ul << 20u,
                          keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0),
                          keywords::format = format);
}
