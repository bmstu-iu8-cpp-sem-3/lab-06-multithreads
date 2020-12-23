// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <picosha2.h>


class BrutFSha {
 private:
  unsigned int n_threads;

 public:
  BrutFSha(const unsigned int n_t) : n_threads(n_t) {}

  void work_threads(){
    std::vector<std::thread> vec_thr;
    for (unsigned int i = 0; i < n_threads; i++){
      vec_thr.push_back(std::thread(reaserch_sha));
    }

    //Прекращение работы по ctrl+c
  }

  void research_sha(){
    thread_local std::mt19937 g(std::random_device{}());
    while(true){
      std::string rand_str = std::to_string(g());
      const std::string hash
          = picosha2::hash256_hex_string(rand_str.begin(), rand_str.end());
    }
  }


};


#endif // INCLUDE_HEADER_HPP_
