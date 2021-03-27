
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
  _execvp(argv[1], &argv[1]);
#else
  execvp(argv[1], &argv[1]);
#endif
  return 1;
}

