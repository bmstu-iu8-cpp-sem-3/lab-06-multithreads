#include <header.hpp>
#include <iostream>
#include <thread>



int main(int argc, char* argv[]) {
    if (argc > 1){
      BrutFSha(std::atoi(argv[1]));
    } else{
      BrutFSha{std::thread::hardware_concurrency()};
    }
}