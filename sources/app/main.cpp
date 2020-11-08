// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <picosha2.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <atomic>
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
#include <fstream>
#include <hash_finder_lib.hpp>
#include <string>
#include <thread>
#include <vector>

namespace hash_finder_app {
    static void setup_logging();
} // namespace hash_finder_app

int main(int const arguments_count, char const* arguments[]) {
    hash_finder_lib::string_utils::ends_with("foo", "o");

    { // this is requires as the program uses static context to handle signals
        static ::std::atomic_bool single_caller_guard = false;
        bool expectation = false;
        if (!single_caller_guard.compare_exchange_strong(expectation, true))
            throw ::std::runtime_error("Attempt to call main() multiple times");
    }

    ::hash_finder_app::setup_logging();

    ::std::size_t m;
    ::std::optional<::std::string> reportFileName;
    if (arguments_count >= 2) {
        try {
            m = ::std::stoul(arguments[1]);
        } catch (::std::invalid_argument const& e) {
            BOOST_LOG_TRIVIAL(error) << "Invalid m: expected positive number but got \"" << arguments[1] << "\""
                                     << ::std::endl;
            return 1;
        } catch (::std::out_of_range const& e) {
            BOOST_LOG_TRIVIAL(error) << "Invalid m: value " << arguments[1] << " is too big" << ::std::endl;
            return 1;
        }
        if (m == 0) {
            ::std::cerr << "Invalid m: expected positive number but got 0" << ::std::endl;
            return 1;
        }

        if (arguments_count >= 3) reportFileName = arguments[2];
    } else {
        m = ::std::thread::hardware_concurrency();
        if (m == 0) m = 1; // minimal value
    }

    assert(m > 0);
    BOOST_LOG_TRIVIAL(debug) << "Using " << m << " workers" << ::std::endl;

    static ::std::atomic_bool shutdown = false;
    {
        __sighandler_t interruptionHandler = [](int const signal) {
            BOOST_LOG_TRIVIAL(info) << "Shutting down due to signal <" << signal << '>' << std::endl;
            shutdown = true;
        };
        sysv_signal(SIGINT, interruptionHandler);
        sysv_signal(SIGSTOP, interruptionHandler);
        sysv_signal(SIGTERM, interruptionHandler);
    }

    auto const* shutdownPointer = &shutdown;

    ::std::vector<::hash_finder_lib::worker::Result> results;
    {
        ::std::mutex resultsMutex;

        ::hash_finder_lib::worker::Callback callback
            = [shutdownPointer, &results, &resultsMutex](::hash_finder_lib::worker::Result const& result) {
                  auto const data = ::picosha2::bytes_to_hex_string(result.data),
                             hash = ::picosha2::bytes_to_hex_string(result.hash);

                  if (::hash_finder_lib::string_utils::ends_with(hash, "0000")) {
                      BOOST_LOG_TRIVIAL(info) << "sha256(" << data << ") = " << hash << ::std::endl;

                      // handle the value matching the task
                      ::std::lock_guard const lock(resultsMutex);
                      results.push_back(result);
                  } else {
                      BOOST_LOG_TRIVIAL(trace) << "sha256(" << data << ") = " << hash << ::std::endl;
                  }

                  return !*shutdownPointer;
              };

        ::std::vector<::std::thread> workers;
        workers.reserve(m);
        for (size_t workerId = 1; workerId <= m; ++workerId) {
            auto const seed = ::hash_finder_lib::seed_generator::good_seed();
            BOOST_LOG_TRIVIAL(debug) << "Worker " << workerId << " uses seed " << seed << ::std::endl;
            workers.push_back(::hash_finder_lib::worker::create_async_worker(seed, callback));
        }
        for (auto& worker : workers) worker.join();
    }

    if (reportFileName) {
        BOOST_LOG_TRIVIAL(info) << "Generating report file \"" << *reportFileName << "\"" << ::std::endl;
        ::std::ofstream output(*reportFileName);
        ::hash_finder_lib::report_generator::generate_report(output, results);
    }

    return 0;
}

namespace hash_finder_app {

    void setup_logging() {
        namespace logging = ::boost::log;
        namespace sources = logging::sources;
        namespace expressions = logging::expressions;
        namespace keywords = logging::keywords;
        namespace sinks = logging::sinks;

        logging::add_common_attributes();

        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);

        auto const format = expressions::stream << '['
                                                << expressions::format_date_time<::boost::posix_time::ptime>(
                                                       "TimeStamp", "%Y-%m-%d %H:%M:%S")
                                                << ']' << " (" << logging::trivial::severity
                                                << "): " << expressions::smessage;

        logging::add_console_log()->set_formatter(format);
        logging::add_file_log(keywords::file_name = "logs/file_%5N.log", keywords::rotation_size = 5ul << 20u,
                              keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0),
                              keywords::format = format);
    }
} // namespace hash_finder_app
