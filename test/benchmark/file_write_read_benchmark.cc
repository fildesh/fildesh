#include <benchmark/benchmark.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include <fildesh/istream.hh>
#include <fildesh/ostream.hh>
extern "C" {
#include "include/fildesh/fildesh_compat_file.h"
}

#define THIS_BENCHMARK_RANGE \
  Args({1<<18})  /* 1.7 MiB file */

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
    write_seq_file(filename.c_str(), state.range(0));
  }
  fildesh_compat_file_rm(filename.c_str());
}
// Register the function as a benchmark
BENCHMARK(BM_FileWriteIntegers_FildeshOF)->THIS_BENCHMARK_RANGE;


static void BM_FileWriteIntegers_fildesh_ofstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("WriteIntegers_fildesh_ofstream", state.range(0));
  for (auto _ : state) {
    fildesh::ofstream out(filename.c_str());
		for (int i = 0; i < state.range(0); ++i) {
      out << i << '\n';
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
// Register the function as a benchmark
BENCHMARK(BM_FileWriteIntegers_fildesh_ofstream)->THIS_BENCHMARK_RANGE;


static void BM_FileWriteIntegers_std_ofstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("WriteIntegers_std_ofstream", state.range(0));
  for (auto _ : state) {
    std::ios_base::openmode mode = (
        std::ios_base::out | std::ios_base::trunc);
    std::ofstream out(filename.c_str(), mode);
		for (int i = 0; i < state.range(0); ++i) {
      out << i << '\n';
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
// Register the function as a benchmark
BENCHMARK(BM_FileWriteIntegers_std_ofstream)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_FileWriteIntegers_fprintf)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_FileReadIntegers_FildeshXF)->THIS_BENCHMARK_RANGE;


static void BM_FileReadIntegers_fildesh_ifstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadIntegers_fildesh_ifstream", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    fildesh::ifstream in(filename.c_str());
		for (int i = 0; i < state.range(0); ++i) {
      int x = -1;
      in >> x;
      assert(x == i);
      benchmark::DoNotOptimize(x);
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadIntegers_fildesh_ifstream)->THIS_BENCHMARK_RANGE;


static void BM_FileReadIntegers_std_ifstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadIntegers_std_ifstream", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    std::ifstream in(filename.c_str());
		for (int i = 0; i < state.range(0); ++i) {
      int x = -1;
      in >> x;
      assert(x == i);
      benchmark::DoNotOptimize(x);
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadIntegers_std_ifstream)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_FileReadIntegers_fscanf)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_FileReadLines_FildeshXF)->THIS_BENCHMARK_RANGE;


static void BM_FileReadLines_fildesh_ifstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadLines_fildesh_ifstream", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    fildesh::ifstream in(filename.c_str());
    std::string line;
		for (int i = 1; i < state.range(0); ++i) {
      std::getline(in, line, '\n');
      assert(!line.empty());
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadLines_fildesh_ifstream)->THIS_BENCHMARK_RANGE;


static void BM_FileReadLines_std_ifstream(benchmark::State& state) {
  const std::string& filename = temporary_file_name("ReadLines_std_ifstream", state.range(0));
  write_seq_file(filename, state.range(0));
  for (auto _ : state) {
    std::ifstream in(filename.c_str());
    std::string line;
		for (int i = 0; i < state.range(0); ++i) {
      std::getline(in, line, '\n');
      assert(!line.empty());
		}
  }
  fildesh_compat_file_rm(filename.c_str());
}
BENCHMARK(BM_FileReadLines_std_ifstream)->THIS_BENCHMARK_RANGE;


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
BENCHMARK(BM_FileReadLines_fgets)->THIS_BENCHMARK_RANGE;


// Run the benchmark
BENCHMARK_MAIN();
