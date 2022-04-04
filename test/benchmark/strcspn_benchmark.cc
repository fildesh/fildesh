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

static std::string make_needle() {
  return std::string(":Rj!<T");
}

#define THIS_BENCHMARK_RANGE RangeMultiplier(32)->Range(1<<2, 1<<14)
/* #define THIS_BENCHMARK_RANGE Arg(2)->Arg(4)->Arg(8)->Arg(16)->Arg(32)->Arg(1<<14)->Arg(1<<15) */
/* #define THIS_BENCHMARK_RANGE Range(1<<14, 1<<14) */

static void BM_strcspn_mascii(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  const std::string needle = make_needle();
  const FildeshMascii mascii = charset_FildeshMascii(needle.c_str(), needle.size());
  for (auto _ : state) {
    int idx = find_FildeshMascii(&mascii, haystack.c_str(), haystack.size());
    benchmark::DoNotOptimize(idx);
    assert(idx == state.range(0)-1);
  }
}
BENCHMARK(BM_strcspn_mascii)->THIS_BENCHMARK_RANGE;

static unsigned benchfn_switch(const char* haystack, const unsigned n) {
  for (unsigned i = 0; i < n; ++i) {
    switch (haystack[i]) {
      case ':':
      case 'R':
      case 'j':
      case '!':
      case '<':
      case 'T':
        return i;
        break;
      default:
        break;
    }
  }
  return n;
}

static void BM_strcspn_switch(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  for (auto _ : state) {
    unsigned idx = benchfn_switch(haystack.c_str(), haystack.size());
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strcspn_switch)->THIS_BENCHMARK_RANGE;

static void BM_strcspn_StdFindFirstOf(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  const std::string needle = make_needle();
  for (auto _ : state) {
    auto idx = haystack.find_first_of(needle);
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strcspn_StdFindFirstOf)->THIS_BENCHMARK_RANGE;

static void BM_strcspn_strcspn(benchmark::State& state) {
  const std::string haystack = make_haystack(state.range(0));
  const std::string needle = make_needle();
  for (auto _ : state) {
    size_t idx = strcspn(haystack.c_str(), needle.c_str());
    benchmark::DoNotOptimize(idx);
    assert(idx == (unsigned)state.range(0)-1);
  }
}
BENCHMARK(BM_strcspn_strcspn)->THIS_BENCHMARK_RANGE;

// Run the benchmark
BENCHMARK_MAIN();

