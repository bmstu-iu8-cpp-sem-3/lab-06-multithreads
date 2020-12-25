// Copyright 2020 Your Name <your_email>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <csignal>
#include <fstream>
#include <hasher.hpp>
#include <iostream>
#include <thread>

void init(){
  auto traceFile = boost::log::add_file_log(
      boost::log::keywords::file_name = "research_%5N.log",
      boost::log::keywords::rotation_size = 5 * 1024 * 1024,
      boost::log::keywords::format = "[%TimeStamp%]: %Message%");
  traceFile->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::trace);
  //trace for console
  auto traceConsole = boost::log::add_console_log(
        std::cout,
      boost::log::keywords::format = "[%TimeStamp%]: %Message%");
  traceConsole->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::trace);
  auto infoConsole = boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = "[%TimeStamp%]: %Message%");
  infoConsole->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::info);
  boost::log::add_common_attributes();
  //exit
  signal(SIGINT, BrutFSha::sign_exit);
}

int main(int argc, char* argv[]) {
    init();
    std::size_t count;
    if (argc > 1){
      count =std::atoi(argv[1]);
    } else{
      count = std::thread::hardware_concurrency();
    }
    BrutFSha b(count);
    b.work_threads();
}