#include <benchmark/benchmark.h>
#include <map>
#include <vector>
#include "fildesh.h"

extern "C" {
  void* make_LaceLegacyIntIntMap();
  void set_LaceLegacyIntIntMap(void*, int, int);
  int* get_LaceLegacyIntIntMap(void*, int);
  void remove_LaceLegacyIntIntMap(void*, int);
  void free_LaceLegacyIntIntMap(void*);
}


static inline int calculate_key(int i, int mul, int off, int count) {
  return (i*mul + off) % count;
}

static inline int calculate_value(int i, int mul, int off, int count) {
  return (i*mul + off);
}


static void BM_MapAddIntegers_LaceLegacy(benchmark::State& state) {
  const int mul = state.range(0);
  const int off = state.range(1);
  const int count = state.range(2);
  assert(mul < count);
  assert(off < count);

  for (auto _ : state) {
    void* map = make_LaceLegacyIntIntMap();
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int v = calculate_value(i, mul, off, count);
      set_LaceLegacyIntIntMap(map, k, v);
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int* v = get_LaceLegacyIntIntMap(map, k);
      assert(v);
      benchmark::DoNotOptimize(v);
    }
    free_LaceLegacyIntIntMap(map);
  }
}
BENCHMARK(BM_MapAddIntegers_LaceLegacy)
  ->Args({100, 3, 200})
  ->Args({0, 7, 5000})
  ->Args({0, 27273, 50000})
  ;


static void BM_MapAddIntegers_StdMap(benchmark::State& state) {
  const int mul = state.range(0);
  const int off = state.range(1);
  const int count = state.range(2);
  assert(mul < count);
  assert(off < count);

  for (auto _ : state) {
    std::map<int,int> map;
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int v = calculate_value(i, mul, off, count);
      map[k] = v;
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      std::map<int,int>::const_iterator v = map.find(k);
      assert(v != map.end());
      benchmark::DoNotOptimize(*v);
    }
  }
}
BENCHMARK(BM_MapAddIntegers_StdMap)
  ->Args({100, 3, 200})
  ->Args({0, 7, 5000})
  ->Args({0, 27273, 50000})
  ;


static void BM_MapAddIntegers_Array(benchmark::State& state) {
  const int mul = state.range(0);
  const int off = state.range(1);
  const int count = state.range(2);
  assert(mul < count);
  assert(off < count);

  for (auto _ : state) {
    int* map = new int[count];
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int v = calculate_value(i, mul, off, count);
      map[k] = v;
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      benchmark::DoNotOptimize(map[k]);
    }
    delete[] map;
  }
}
BENCHMARK(BM_MapAddIntegers_Array)
  ->Args({100, 3, 200})
  ->Args({0, 7, 5000})
  ->Args({0, 27273, 50000})
  ;


static void BM_MapAddIntegers_Nop(benchmark::State& state) {
  const int mul = state.range(0);
  const int off = state.range(1);
  const int count = state.range(2);
  assert(mul < count);
  assert(off < count);

  for (auto _ : state) {
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int v = calculate_value(i, mul, off, count);
      benchmark::DoNotOptimize(k);
      benchmark::DoNotOptimize(v);
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      benchmark::DoNotOptimize(k);
    }
  }
}
BENCHMARK(BM_MapAddIntegers_Nop)
  ->Args({100, 3, 200})
  ->Args({0, 7, 5000})
  ->Args({0, 27273, 50000})
  ;


// Run the benchmark
BENCHMARK_MAIN();
