// Copyright 2020 Petr Portnov <gh@progrm-jarvis.ru>

#include <cstddef>
#include <hash_finder_lib.hpp>
#include <iostream>
#include <string>
#include <thread>

int main(int const arguments_count, char const* arguments[]) {
    ::std::size_t m;
    if (arguments_count > 2) {
        try {
            m = ::std::stoul(arguments[1]);
        } catch (::std::invalid_argument const& e) {
            ::std::cerr << "Invalid m: expected positive number but got \"" << arguments[1] << "\"" << std::endl;
            return 1;
        } catch (::std::out_of_range const& e) {
            ::std::cerr << "Invalid m: value " << arguments[1] << " is too big" << std::endl;
            return 1;
        }
        if (m == 0) {
            ::std::cerr << "Invalid m: expected positive number but got 0" << std::endl;
            return 1;
        }
    } else {
        m = ::std::thread::hardware_concurrency();
        if (m == 0) m = 1; // minimal value
    }

    assert(m > 0);
}
