// Copyright 2020 Malenko Artem artem.malenko@yandex.ru


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
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <csignal>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

namespace loging = boost::log;
namespace sources = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using nlohmann::json;

using backend_type = sinks::text_file_backend;
using sink_type = sinks::synchronous_sink<backend_type>;
const std::string g_form = "[%TimeStamp%] [%Severity%] : %Message%";

std::string output_file;

json data;
std::mutex mut;
const char alpha_num[] = "0123456789abcdefghijklmnopqrstuvwxyz";

std::vector<unsigned char> generator_random() {
    std::vector<unsigned char> vector;
    vector.reserve(4);
    for (size_t i = 0; i < 4; i++) {
        vector.push_back(alpha_num[rand() % (sizeof(alpha_num) - 1)]);
    }
    return vector;
}

void add_in_Json(const size_t &timestamp, const std::string &hash, const std::string &prototype) {
    json object;
    object["data"] = prototype;
    object["hash"] = hash;
    object["timestamp"] = timestamp;
    mut.lock();
    data.push_back(object);
    mut.unlock();
}

[[noreturn]] void find_hash() {
    for (;;) {
        std::vector<unsigned char> temp = generator_random();
        std::string r_string;
        for (const auto &elem : temp) {
            r_string += elem;
        }

        std::string hash = picosha2::hash256_hex_string(temp);

        BOOST_LOG_TRIVIAL(trace) << r_string << ' ' << hash << ' '
                                 << std::this_thread::get_id() << '\n';

        if (hash.substr(60, 4) == "0000") {
            BOOST_LOG_TRIVIAL(info)
                    << " FOUND! " << r_string << " " << hash << ' '
                    << loging::aux::this_thread::get_id() << '\n';
            add_in_Json(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count(),
                        hash, r_string);
        }
    }
}

void init() {
    boost::shared_ptr <loging::core> core = loging::core::get();

    loging::add_common_attributes();
    boost::shared_ptr <backend_type> backend =
            boost::make_shared<sinks::text_file_backend>(
                    keywords::file_name = "file_%5N.log",
                    keywords::rotation_size = 5 * 1024 * 1024,
                    keywords::max_size = 1024 * 1024,
                    keywords::min_free_space = 1000 * 1024 * 1024,
                    keywords::max_files = 10,
                    keywords::time_based_rotation =
                            sinks::file::rotation_at_time_point(1, 0, 0));

    boost::shared_ptr <sink_type> sink(new sink_type(backend));
    sink->set_formatter(loging::parse_formatter(g_form));
    core->add_sink(sink);
    loging::add_file_log(backend);
    loging::add_console_log(std::cout, keywords::format = g_form);

    loging::core::get()->set_filter(loging::trivial::severity >=
                                    loging::trivial::trace);
}

void signal_handler(int) {
    std::ofstream out(output_file);
    out << "[\n";
    for (size_t i = 0; i < data.size(); i++) {
        json d = data.at(i);
        out << "    {" << std::endl;
        out << "        \"data\" : " << d["data"] << "," << std::endl;
        out << "        \"hash\" : " << d["hash"] << "," << std::endl;
        out << "        \"timestamp\": " << d["timestamp"] << std::endl;
        out << "    }";
        if (i != data.size() - 1) out << ",";
        out << "\n";
    }
    out << "]\n";
    out.close();
    std::cout << "File successfully created\n";
    exit(0);
}

#endif // INCLUDE_HEADER_HPP_
