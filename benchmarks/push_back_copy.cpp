//
// Copyright Miro Knejp 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//

#include "bench-utils.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace bench_utils;

namespace
{
  auto alloc = bumb_allocator<char>(std::size_t(8) * 1024 * 1024 * 1024);

  auto const max_bytes_tests = {
    std::int64_t(64),
    std::int64_t(128),
    std::int64_t(256),
    std::int64_t(512),
    std::int64_t(1) * 1024,
    std::int64_t(4) * 1024,
    std::int64_t(16) * 1024,
    std::int64_t(64) * 1024,
    std::int64_t(128) * 1024,
    std::int64_t(512) * 1024,
    std::int64_t(1) * 1024 * 1024,
    std::int64_t(4) * 1024 * 1024,
    std::int64_t(16) * 1024 * 1024,
    std::int64_t(64) * 1024 * 1024,
    std::int64_t(128) * 1024 * 1024,
    std::int64_t(512) * 1024 * 1024,
    std::int64_t(1) * 1024 * 1024 * 1024,
    std::int64_t(2) * 1024 * 1024 * 1024,
    // std::int64_t(4) * 1024 * 1024 * 1024,
  };

  template<typename T>
  auto configure_runs(benchmark::internal::Benchmark* b)
  {
    b->Unit(benchmark::kNanosecond);
    for(auto max_bytes: max_bytes_tests)
    {
      if(max_bytes / sizeof(T) > 0)
      {
        b->Arg(max_bytes);
      }
    }
  }

}

template<typename T>
static auto push_back_copy_only(benchmark::State& state, T x)
{
  auto const max_size = static_cast<std::size_t>(state.range(0));
  auto const n = max_size / sizeof(T);

  for(auto _: state)
  {
    (void)_;
    alloc.reset();
    auto v = std::vector<T, bumb_allocator<T>>(bumb_allocator<T>(alloc));
    std::fill_n(std::back_inserter(v), n, x);
    benchmark::DoNotOptimize(v.data());
    benchmark::ClobberMemory();
  }
}

// trivially copyable types
BENCHMARK_CAPTURE(push_back_copy_only, int, 12345)->Apply(configure_runs<int>);

BENCHMARK_CAPTURE(push_back_copy_only, bigval, bigval{1, 2, 3, 4, 5, 6, 7, 8, 9, 0})->Apply(configure_runs<bigval>);

// std::string with small string optimization
BENCHMARK_CAPTURE(push_back_copy_only, std::string, std::string("abcd"))->Apply(configure_runs<std::string>);
