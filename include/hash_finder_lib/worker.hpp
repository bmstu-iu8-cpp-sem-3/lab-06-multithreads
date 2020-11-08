// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#ifndef INCLUDE_HASH_FINDER_LIB_WORKER_HPP_
#define INCLUDE_HASH_FINDER_LIB_WORKER_HPP_

#include <chrono>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>

namespace hash_finder_lib::worker {

    struct Result {
        ::std::vector<::std::uint8_t> data, hash;
        ::std::chrono::seconds timestamp;
    };

    using Callback = ::std::function<bool(Result)>;

    ::std::thread create_async_worker(::std::uintmax_t seed, Callback const&);
} // namespace hash_finder_lib::worker

#endif // INCLUDE_HASH_FINDER_LIB_WORKER_HPP_
