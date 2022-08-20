/** Benchmark the Fildesh bump allocator.
 *
 * We only do small memory allocations that can be unaligned
 * because we're mimicing strings.
 **/
#include <benchmark/benchmark.h>
#include <cstring>
#include "fildesh.h"


// Mimic repeated use of unaligned strings.
#define THIS_BENCHMARK_RANGE \
  Args({16, 1}) \
  ->Args({3, 10000}) \
  ->Args({7, 10000}) \
  ->Args({16, 10000}) \
  ->Args({55, 10000}) \
  ->Args({201, 10000}) \
  ->Args({512, 10000})


static std::string make_original_string(unsigned nbytes) {
  std::string s = std::string(nbytes-1, 'x');
  for (unsigned i = 0; i < s.size(); ++i) {
    char c = 32+((nbytes+3*i)%32);
    s[i] = c;
  }
  return s;
}


#define ACCESS_ALL_COPIES do { \
  for (unsigned i = 0; i < count; ++i) { \
    benchmark::DoNotOptimize(copies[i][0]); \
  } \
} while (0)


static void BM_strdup_FildeshAlloc(benchmark::State& state) {
  const std::string original = make_original_string(state.range(0));
  const unsigned count = state.range(1);
  for (auto _ : state) {
    FildeshAlloc* alloc = open_FildeshAlloc();
    char** copies = fildesh_allocate(char*, count, alloc);
		for (unsigned i = 0; i < count; ++i) {
      copies[i] = strdup_FildeshAlloc(alloc, original.c_str());
    }
    ACCESS_ALL_COPIES;
    close_FildeshAlloc(alloc);
  }
}
BENCHMARK(BM_strdup_FildeshAlloc)->THIS_BENCHMARK_RANGE;


static void BM_strndup_FildeshAlloc(benchmark::State& state) {
  const std::string original = make_original_string(state.range(0));
  const unsigned count = state.range(1);
  FildeshO o_original[1] = {DEFAULT_FildeshO};
  puts_FildeshO(o_original, original.c_str());
  for (auto _ : state) {
    FildeshAlloc* alloc = open_FildeshAlloc();
    char** copies = fildesh_allocate(char*, count, alloc);
		for (unsigned i = 0; i < count; ++i) {
      copies[i] = strdup_FildeshO(o_original, alloc);
    }
    ACCESS_ALL_COPIES;
    close_FildeshAlloc(alloc);
  }
  close_FildeshO(o_original);
}
BENCHMARK(BM_strndup_FildeshAlloc)->THIS_BENCHMARK_RANGE;


static void BM_strndup_malloc(benchmark::State& state) {
  const std::string original = make_original_string(state.range(0));
  const unsigned count = state.range(1);
  for (auto _ : state) {
    char** copies = (char**) malloc(sizeof(char*) * count);
		for (unsigned i = 0; i < count; ++i) {
      copies[i] = (char*) malloc(original.size()+1);
      memcpy(copies[i], original.c_str(), original.size()+1);
    }
    ACCESS_ALL_COPIES;
		for (unsigned i = 0; i < count; ++i) {
      free(copies[i]);
    }
    free(copies);
  }
}
BENCHMARK(BM_strndup_malloc)->THIS_BENCHMARK_RANGE;


static void BM_strndup_StdVectorConstructor(benchmark::State& state) {
  const std::string original = make_original_string(state.range(0));
  const unsigned count = state.range(1);
  for (auto _ : state) {
    std::vector<std::string> copies(count, original);
    ACCESS_ALL_COPIES;
  }
}
BENCHMARK(BM_strndup_StdVectorConstructor)->THIS_BENCHMARK_RANGE;


// Run the benchmark
BENCHMARK_MAIN();
