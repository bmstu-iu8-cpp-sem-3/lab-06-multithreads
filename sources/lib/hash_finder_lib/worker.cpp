// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <picosha2.h>

#include <hash_finder_lib/worker.hpp>
#include <random>

namespace hash_finder_lib::worker {

    static constexpr ::std::size_t DATA_LENGTH = 9;

    template <typename R, typename D>
    inline static Result next_result(R& random, D& distribution);

    ::std::thread create_async_worker(::std::uintmax_t const seed, Callback const& callback) {
        return ::std::thread([seed, &callback]() {
            ::std::default_random_engine random(seed);
            ::std::uniform_int_distribution<::std::uint8_t> distribution;

            Result result;
            do {
                result = next_result(random, distribution);
            } while (callback(result));
        });
    }

    template <typename R, typename D>
    inline static ::std::vector<::std::uint8_t> random_data(R& random, D& distribution);

    template <typename R, typename D>
    Result next_result(R& random, D& distribution) {
        Result result{random_data(random, distribution), {}, {}};
        // resize (zero-fill) target buffer so that its length corresponds to digest one
        result.hash.resize(::picosha2::k_digest_size);
        ::picosha2::hash256(result.data, result.hash);
        result.timestamp = ::std::chrono::duration_cast<::std::chrono::seconds>(
            ::std::chrono::system_clock::now().time_since_epoch());

        return result;
    }

    template <typename R, typename D>
    ::std::vector<::std::uint8_t> random_data(R& random, D& distribution) {
        ::std::vector<::std::uint8_t> data;
        data.reserve(DATA_LENGTH);
        for (size_t i = 0; i < DATA_LENGTH; ++i) data.push_back(distribution(random));

        return data;
    }
} // namespace hash_finder_lib::worker
