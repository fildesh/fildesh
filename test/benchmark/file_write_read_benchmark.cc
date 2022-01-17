#include <benchmark/benchmark.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

extern "C" {
#include "fildesh.h"
#include "fildesh_compat_file.h"
}

static std::string temporary_file_name(const std::string& basename, int n) {
  const char* output_directory = getenv("TEST_TMPDIR");
  assert(output_directory && "need a TEST_TMPDIR environment variable");
  return (
      std::string(output_directory) + "/" + basename +
      "_" + std::to_string(n) + ".txt");
}


static void write_seq_file(const std::string& filename, int n) {
  FildeshO* out = open_FildeshOF(filename.c_str());
  for (int i = 0; i < n; ++i) {
    print_int_FildeshO(out, i);
    putc_FildeshO(out, '\n');
  }
  close_FildeshO(out);
}


static void BM_FileWriteIntegers_FildeshOF(benchmark::State& state) {
  const std::string& filename = temporary_file_name("WriteIntegers_FildeshOF", state.range(0));
  for (auto _ : state) {
    write_seq_file(filename, state.range(0));
  }
  fildesh_compat_file_rm(filename.c_str());
}
// Register the function as a benchmark
BENCHMARK(BM_FileWriteIntegers_FildeshOF)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileWriteIntegers_ofstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("WriteIntegers_ofstream", state.range(0));
  for (auto _ : state) {
    std::ios_base::openmode mode = (
        std::ios_base::out | std::ios_base::trunc);
    std::ofstream out(filename, mode);
		for (int i = 0; i < state.range(0); ++i) {
      out << i << '\n';
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
// Register the function as a benchmark
BENCHMARK(BM_FileWriteIntegers_ofstream)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileWriteIntegers_fprintf(benchmark::State& state) {
  const std::string& filename = temporary_file_name("WriteIntegers_fprintf", state.range(0));
  for (auto _ : state) {
    FILE* out = fopen(filename.c_str(), "w");
		for (int i = 0; i < state.range(0); ++i) {
      fprintf(out, "%d\n", i);
		}
    fclose(out);
  }
  fildesh_compat_file_rm(filename.c_str());
}
// Register the function as a benchmark
BENCHMARK(BM_FileWriteIntegers_fprintf)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileReadIntegers_FildeshXF(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadIntegers_FildeshXF", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    FildeshX* in = open_FildeshXF(filename.c_str());
		for (int i = 0; i < state.range(0); ++i) {
      int x = -1;
      parse_int_FildeshX(in, &x);
      assert(x == i);
      benchmark::DoNotOptimize(x);
		}
    close_FildeshX(in);
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadIntegers_FildeshXF)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileReadIntegers_ifstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadIntegers_ifstream", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    std::ifstream in(filename);
		for (int i = 0; i < state.range(0); ++i) {
      int x = -1;
      in >> x;
      assert(x == i);
      benchmark::DoNotOptimize(x);
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadIntegers_ifstream)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileReadIntegers_fscanf(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadIntegers_fscanf", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    FILE* in = fopen(filename.c_str(), "r");
		for (int i = 0; i < state.range(0); ++i) {
      int x = -1;
      fscanf(in, "%d", &x);
      assert(x == i);
      benchmark::DoNotOptimize(x);
		}
    fclose(in);
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadIntegers_fscanf)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileReadLines_FildeshXF(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadLines_FildeshXF", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    FildeshX* in = open_FildeshXF(filename.c_str());
		for (int i = 0; i < state.range(0); ++i) {
      const char* line = getline_FildeshX(in);
      assert(line);
		}
    close_FildeshX(in);
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadLines_FildeshXF)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileReadLines_ifstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadLines_ifstream", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    std::ifstream in(filename);
    std::string line;
		for (int i = 0; i < state.range(0); ++i) {
      std::getline(in, line, '\n');
      assert(!line.empty());
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadLines_ifstream)
  /* 1.7 MiB file */
  ->Args({1<<18});


static void BM_FileReadLines_fgets(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadLines_fgets", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    FILE* in = fopen(filename.c_str(), "r");
		for (int i = 0; i < state.range(0); ++i) {
      char line[FILDESH_INT_BASE10_SIZE_MAX+2];
      line[0] = '\0';
      fgets(line, sizeof(line), in);
      assert(line[0] != '\0');
		}
    fclose(in);
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadLines_fgets)
  /* 1.7 MiB file */
  ->Args({1<<18});


// Run the benchmark
BENCHMARK_MAIN();

