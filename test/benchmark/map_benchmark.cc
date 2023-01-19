#include <benchmark/benchmark.h>
#include <map>
#include <vector>
#include <fildesh/fildesh.h>


#define LINEAR_BENCHMARK_RANGE \
  Args({100, 3, 200}) \
  ->Args({0, 7, 5000})

#define THIS_BENCHMARK_RANGE \
  LINEAR_BENCHMARK_RANGE \
  ->Args({0, 27273, 50000}) \
  ->Args({0, 3, 50000})


static inline int calculate_key(int i, int off, int mul, int count) {
  return (off + i*mul) % count;
}

static inline int calculate_value(int i, int off, int mul, int count) {
  return (off + i*mul);
}


static inline
  void
MapAddIntegers_FildeshKV_common(
    FildeshKV* map, const int off, const int mul, const int count)
{
  for (int i = 0; i < count; ++i) {
    const int k = calculate_key(i, off, mul, count);
    const int v = calculate_value(i, off, mul, count);
    const FildeshKV_id_t id = ensure_FildeshKV(map, &k, sizeof(k));
    assign_at_FildeshKV(map, id, &v, sizeof(v));
  }
  for (int i = 0; i < count; ++i) {
    const int k = calculate_key(i, off, mul, count);
    const int* v = (int*) lookup_value_FildeshKV(map, &k, sizeof(k));
    assert(v);
    assert(*v == calculate_value(i, off, mul, count));
    benchmark::DoNotOptimize(*v);
  }
  close_FildeshKV(map);
}


static void BM_MapAddIntegers_FildeshKV_SINGLE_LIST(benchmark::State& state) {
  const int off = state.range(0);
  const int mul = state.range(1);
  const int count = state.range(2);
  assert(off < count);
  assert(mul < count);

  for (auto _ : state) {
    FildeshKV map[1] = {DEFAULT_FildeshKV_SINGLE_LIST};
    MapAddIntegers_FildeshKV_common(map, off, mul, count);
  }
}
BENCHMARK(BM_MapAddIntegers_FildeshKV_SINGLE_LIST)->LINEAR_BENCHMARK_RANGE;


static void BM_MapAddIntegers_StdMap(benchmark::State& state) {
  const int off = state.range(0);
  const int mul = state.range(1);
  const int count = state.range(2);
  assert(off < count);
  assert(mul < count);

  for (auto _ : state) {
    std::map<int,int> map;
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, off, mul, count);
      const int v = calculate_value(i, off, mul, count);
      map[k] = v;
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, off, mul, count);
      std::map<int,int>::const_iterator v = map.find(k);
      assert(v != map.end());
      benchmark::DoNotOptimize(*v);
    }
  }
}
BENCHMARK(BM_MapAddIntegers_StdMap)->THIS_BENCHMARK_RANGE;


static void BM_MapAddIntegers_Array(benchmark::State& state) {
  const int off = state.range(0);
  const int mul = state.range(1);
  const int count = state.range(2);
  assert(off < count);
  assert(mul < count);

  for (auto _ : state) {
    int* map = new int[count];
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, off, mul, count);
      const int v = calculate_value(i, off, mul, count);
      map[k] = v;
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, off, mul, count);
      benchmark::DoNotOptimize(map[k]);
    }
    delete[] map;
  }
}
BENCHMARK(BM_MapAddIntegers_Array)->THIS_BENCHMARK_RANGE;


static void BM_MapAddIntegers_Nop(benchmark::State& state) {
  const int off = state.range(0);
  const int mul = state.range(1);
  const int count = state.range(2);
  assert(off < count);
  assert(mul < count);

  for (auto _ : state) {
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, off, mul, count);
      const int v = calculate_value(i, off, mul, count);
      benchmark::DoNotOptimize(k);
      benchmark::DoNotOptimize(v);
    }
		for (int i = 0; i < count; ++i) {
      const int k = calculate_key(i, off, mul, count);
      benchmark::DoNotOptimize(k);
    }
  }
}
BENCHMARK(BM_MapAddIntegers_Nop)->THIS_BENCHMARK_RANGE;


// Run the benchmark
BENCHMARK_MAIN();
