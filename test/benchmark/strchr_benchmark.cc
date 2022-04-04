#include <benchmark/benchmark.h>
#include <cstring>
#include "src/mascii.h"

static std::string make_haystack(unsigned n) {
  std::string s = std::string(n, 'x');
  for (unsigned i = 0; i < s.size(); ++i) {
    char c = 32+3*(i%32);
    s[i] = c;
  }
  s[s.size()-1] = '!';
  return s;
}

#define THIS_BENCHMARK_RANGE RangeMultiplier(32)->Range(1<<2, 1<<14)
/* #define THIS_BENCHMARK_RANGE Range(1<<14, 1<<14) */

static void BM_strchr_mascii(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  const FildeshMascii mascii = charset_FildeshMascii("!", 1);
  for (auto _ : state) {
    int idx = find_FildeshMascii(&mascii, haystack.c_str(), haystack.size());
    benchmark::DoNotOptimize(idx);
    assert(idx == state.range(0)-1);
  }
}
BENCHMARK(BM_strchr_mascii)->THIS_BENCHMARK_RANGE;

static unsigned benchfn_if1(const char* haystack, const unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    if (haystack[i] == '!') {
      return i;
    }
  }
  return n;
}

static void BM_strchr_if1(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  for (auto _ : state) {
    unsigned idx = benchfn_if1(haystack.c_str(), haystack.size());
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strchr_if1)->THIS_BENCHMARK_RANGE;

static void BM_strchr_StdFind(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  for (auto _ : state) {
    auto idx = haystack.find('!');
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strchr_StdFind)->THIS_BENCHMARK_RANGE;

static void BM_strchr_strchr(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  for (auto _ : state) {
    const char* s = strchr(haystack.c_str(), '!');
    assert(s);
    size_t idx = s - haystack.c_str();
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strchr_strchr)->THIS_BENCHMARK_RANGE;

static void BM_strchr_memchr(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  for (auto _ : state) {
    const char* s = (const char*) memchr(haystack.c_str(), '!', haystack.size());
    assert(s);
    size_t idx = s - haystack.c_str();
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strchr_memchr)->THIS_BENCHMARK_RANGE;

// Run the benchmark
BENCHMARK_MAIN();

