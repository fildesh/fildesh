#include <benchmark/benchmark.h>
#include <map>
#include <vector>
#include "fildesh.h"


#define THIS_BENCHMARK_RANGE \
  Args({100, 3, 200}) \
  ->Args({0, 7, 5000}) \
  ->Args({0, 27273, 50000})


extern "C" {
  void* make_FildeshLegacyIntIntMap();
  void set_FildeshLegacyIntIntMap(void*, int, int);
  int* get_FildeshLegacyIntIntMap(void*, int);
  void remove_FildeshLegacyIntIntMap(void*, int);
  void free_FildeshLegacyIntIntMap(void*);
}


static inline int calculate_key(int i, int mul, int off, int count) {
  return (i*mul + off) % count;
}

static inline int calculate_value(int i, int mul, int off, int count) {
  return (i*mul + off);
}


static void BM_MapAddIntegers_FildeshLegacy(benchmark::State& state) {
  const int mul = state.range(0);
  const int off = state.range(1);
  const int count = state.range(2);
  assert(mul < count);
  assert(off < count);

  for (auto _ : state) {
    void* map = make_FildeshLegacyIntIntMap();
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int v = calculate_value(i, mul, off, count);
      set_FildeshLegacyIntIntMap(map, k, v);
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int* v = get_FildeshLegacyIntIntMap(map, k);
      assert(v);
      benchmark::DoNotOptimize(v);
    }
    free_FildeshLegacyIntIntMap(map);
  }
}
BENCHMARK(BM_MapAddIntegers_FildeshLegacy)->THIS_BENCHMARK_RANGE;


static void BM_MapAddIntegers_FildeshKV_SINGLE_LIST(benchmark::State& state) {
  const int mul = state.range(0);
  const int off = state.range(1);
  const int count = state.range(2);
  assert(mul < count);
  assert(off < count);

  for (auto _ : state) {
    FildeshKV map[1] = {DEFAULT_FildeshKV_SINGLE_LIST};
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int v = calculate_value(i, mul, off, count);
      const FildeshKV_id_t id = ensure_FildeshKV(map, &k, sizeof(k));
      assign_at_FildeshKV(map, id, &v, sizeof(v));
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, mul, off, count);
      const int* v = (int*) lookup_value_FildeshKV(map, &k, sizeof(k));
      assert(v);
      benchmark::DoNotOptimize(v);
    }
    close_FildeshKV(map);
  }
}
BENCHMARK(BM_MapAddIntegers_FildeshKV_SINGLE_LIST)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_MapAddIntegers_StdMap)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_MapAddIntegers_Array)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_MapAddIntegers_Nop)->THIS_BENCHMARK_RANGE;


// Run the benchmark
BENCHMARK_MAIN();
