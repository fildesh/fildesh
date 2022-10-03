#include <benchmark/benchmark.h>
#include <cstdlib>
#include <vector>
#include "fildesh.h"


static void BM_PushPop_FildeshA(benchmark::State& state) {
  // Perform setup here
  for (auto _ : state) {
		unsigned char* at = NULL;
		size_t count = 0;
		fildesh_lgsize_t allocated_lgcount = 0;
		for (int i = 0; i < state.range(0); ++i) {
			*static_cast<unsigned char*>(
          grow_FildeshA_((void**)(&at), &count, &allocated_lgcount, 1, 1))
        = static_cast<unsigned char>(i & 0xFF);
		}
		for (int i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(at[count-1]);
      mpop_FildeshA_((void**)&at, &count, &allocated_lgcount, 1, 1);
		}
    if (at) {free(at);}
  }
}
// Register the function as a benchmark
BENCHMARK(BM_PushPop_FildeshA)
  ->RangeMultiplier(2)->Range(1<<10, 1<<18);//->Complexity(benchmark::oN);


static void BM_PushPop_StdVector(benchmark::State& state) {
  // Perform setup here
  for (auto _ : state) {
    std::vector<unsigned char> at;
		for (int i = 0; i < state.range(0); ++i) {
      at.push_back(static_cast<unsigned char>(i & 0xFF));
		}
		for (int i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(at[at.size()-1]);
      at.pop_back();
		}
  }
}
// Register the function as a benchmark
BENCHMARK(BM_PushPop_StdVector)
  ->RangeMultiplier(2)->Range(1<<10, 1<<18);//->Complexity(benchmark::oN);

// Run the benchmark
BENCHMARK_MAIN();

