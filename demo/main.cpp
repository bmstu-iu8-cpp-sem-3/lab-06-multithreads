#include <header.hpp>

// first arg = threadCount, second arg = outputFile
int main(int argc, char *argv[]) {
  size_t threadCount =
      argc < 2 ? std::thread::hardware_concurrency() : std::stoul(argv[1]);
  outputFile = argc < 3 ? "../include/Hashes.json" : argv[2];
  init();
  srand(time(NULL));
  std::signal(SIGINT, signalHandler);

  std::vector<std::thread> threads;
  threads.reserve(threadCount);

  for (size_t i = 0; i < threadCount; i++) {
    threads.emplace_back(FindHash);
  }
  for (std::thread &th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }
}