// Copyright [2020] <Alyona Dorodnyaya>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <PicoSHA2/picosha2.h>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <chrono>
#include <csignal>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>

namespace log = boost::log;
namespace sources = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using nlohmann::json;

using backend_type = sinks::text_file_backend;
using sink_type = sinks::synchronous_sink<backend_type>;
const std::string g_format = "[%TimeStamp%] [%Severity%] : %Message%";
std::string outputFile;

json data;
std::mutex mut;
const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";

// generate data
std::vector<unsigned char> RandGenerator() {
  std::vector<unsigned char> vec;
  vec.reserve(4);
  for (size_t i = 0; i < 4; i++) {
    vec.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
  }
  return vec;
}

// add in json
void AddToJson(const size_t &timestamp, const std::string &hash,
               const std::string &prototype) {
  json object;
  object["data"] = prototype;
  object["hash"] = hash;
  object["timestamp"] = timestamp;
  mut.lock();
  data.push_back(object);
  mut.unlock();
}

[[noreturn]] void FindHash() {
  for (;;) {
    std::vector<unsigned char> temp = RandGenerator();
    std::string randomString;
    for (const auto &elem : temp) {
      randomString += elem;
    }

    std::string hash = picosha2::hash256_hex_string(temp);

    // for all hash
    BOOST_LOG_TRIVIAL(trace) << randomString << ' ' << hash << ' '
                             << std::this_thread::get_id() << '\n';

    // for <{XXX}000> hash
    if (hash.substr(60, 4) == "0000") {
      BOOST_LOG_TRIVIAL(info) << " FOUND! " << randomString << " " << hash
                              << ' ' << log::aux::this_thread::get_id() << '\n';
      AddToJson(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count(),
                hash, randomString);
    }
  }
}

void init() {
  boost::shared_ptr<log::core> core = log::core::get();

  log::add_common_attributes();
  boost::shared_ptr<backend_type> backend =
      boost::make_shared<sinks::text_file_backend>(
          keywords::file_name = "file_%5N.log",
          keywords::rotation_size = 5 * 1024 * 1024,
          keywords::max_size = 1024 * 1024,
          keywords::min_free_space = 1000 * 1024 * 1024,
          keywords::max_files = 10,
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(1, 0, 0));

  boost::shared_ptr<sink_type> sink(new sink_type(backend));
  sink->set_formatter(log::parse_formatter(g_format));
  core->add_sink(sink);
  log::add_file_log(backend);
  log::add_console_log(std::cout, keywords::format = g_format);

  log::core::get()->set_filter(log::trivial::severity >= log::trivial::trace);
}

void signalHandler(int) {
  std::ofstream out(outputFile);
  out << data;
  out.close();
  std::cout << "File successfully created\n";
  exit(0);
}

#endif  // INCLUDE_HEADER_HPP_
