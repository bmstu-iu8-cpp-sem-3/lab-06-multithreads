// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <boost/log/keywords/file_name.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/keywords/rotation_size.hpp>
#include <boost/log/keywords/time_based_rotation.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <cstddef>
#include <hash_finder_lib.hpp>
#include <iostream>
#include <string>
#include <thread>

namespace logging = ::boost::log;

static void setup_logging(::boost::shared_ptr<::logging::core> const& logging_core);

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

    setup_logging(::logging::core::get());

    ::std::cout << "testme" << ::std::endl;
    BOOST_LOG_TRIVIAL(info) << "Hello world";
}

void setup_logging(::boost::shared_ptr<::logging::core> const& logging_core) {
    namespace keywords = ::logging::keywords;
    namespace sinks = ::logging::sinks;

    logging_core->add_sink(boost::make_shared<sinks::asynchronous_sink<sinks::text_file_backend>>(
        boost::make_shared<sinks::text_file_backend>(
            keywords::file_name = "logs/file_%5N.log", keywords::rotation_size = 5ul << 20u,
            keywords::format = "[%TimeStamp%]: %Message%",
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0))));
}
