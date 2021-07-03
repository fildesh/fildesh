
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>

int lace_tool_spawn_main(int argc, char** argv);

static int spawnit(const char* arg1, ...) {
  va_list argp;
  unsigned argc = 10;
  const char* argv[11];
  unsigned i;

  argv[0] = "spawn";
  argv[1] = arg1;
  if (!argv[1]) {
    argc = 1;
  } else {
    va_start(argp, arg1);
    for (i = 2; i <= argc && argv[i-1]; ++i) {
      argv[i] = va_arg(argp, const char*);
    }
    va_end(argp);
    argc = i-1;
  }
  assert(!argv[argc]);
  return lace_tool_spawn_main(argc, (char**)argv);
}

int main(int argc, const char** argv) {
  const char* success_exe;
  const char* failure_exe;

  assert(argc == 3);
  success_exe = argv[1];
  failure_exe = argv[2];

  /* Error with no args.*/
  assert(64 == spawnit(NULL));

  /* Tricky way to mimic `true` and `false` commands.*/
  assert(0 == spawnit("!", "!", NULL));
  assert(64 == spawnit("!", NULL));
  /* But we have stand-in `success` and `failure` commands for this test.*/

  /* Expected success.*/
  assert(0 == spawnit(success_exe, NULL));
  /* Status is propagated from `failure`.*/
  assert(1 == spawnit(failure_exe, NULL));

  /* Unexpected success.*/
  assert(65 == spawnit("!", success_exe, NULL));
  /* Expected failure.*/
  assert(0 == spawnit("!", failure_exe, NULL));

  /* The current directory is not a valid executable.*/
  assert(126 == spawnit(".", NULL));
  /* Treat as "unexpected success" when negated because we lack information.*/
  assert(65 == spawnit("!", ".", NULL));

  return 0;
}
