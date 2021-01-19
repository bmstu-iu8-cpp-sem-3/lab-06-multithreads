#include <header.hpp>

int main(int argc, char *argv[]) {
    size_t thread_count = argc < 2 ? std::thread::hardware_concurrency() : std::stoul(argv[1]);
    output_file = argc < 3 ? "../hash/hashes.json" : argv[2];
    init();
    srand(time(NULL));
    std::signal(SIGINT, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::vector<std::thread> threads_vec;
    threads_vec.reserve(thread_count);

    for (size_t i = 0; i < thread_count; i++) {
        threads_vec.emplace_back(find_hash);
    }
    for (std::thread &th : threads_vec) {
        if (th.joinable()) {
            th.join();
        }
    }
}