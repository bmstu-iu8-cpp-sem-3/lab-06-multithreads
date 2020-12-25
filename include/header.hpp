// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <picosha2.h>
#include <mutex>
#include <nlohmann/json.hpp>

bool is_correct_ssh(const std::string& s);

using json = nlohmann::json;

class BrutFSha {
 private:
  unsigned int n_threads;
  [[noreturn]] void research_sha();
  std::size_t time();
  std::mutex Mutex;

 public:
  BrutFSha(const unsigned int n_t) : n_threads(n_t) {}
  void work_threads();
  static json data;

  static void sign_exit(int){
    std::cout << "Exit from program" <<std::endl;
    std::ofstream file("info_array.txt");
    file << data;
    exit(0);
  }
};

#endif // INCLUDE_HEADER_HPP_
