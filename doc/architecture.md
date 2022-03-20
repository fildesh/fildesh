# Fildesh Architecture

Remarks on where and why things are in this repository.

## Layout



### Spawn tool
The `spawn` tool (at `tool/spawn.c`) is meant to be a minimal, robust, and self-contained example of how to spawn another program.
There are several reasons for this:
* Test sanity.
  * `spawn` is rigorously tested to be sure that it fails when it should.
  * The small code size makes it possible to audit certain behaviors (like `spawn !` exits in failure).
* Test efficiency.
  * Many of our Bazel tests rely on a `spawn_test()` macro that is a simple wrapper for a `cc_test()`.
    This means that `spawn.c` is recompiled a lot, which is only reasonable due to how simple and small it is.
* Code coverage.
  * When one program spawns another, any code that they both rely on tends to be omitted from the child process's coverage report.
    Having all `spawn` code in one file (that nothing else references) prevents this.
* Documentation.
  * Shows the core OS functions that the rest of the codebase uses to spawn new processes.
  * Shows how Windows escaping works.
* Debugging.
  * Spawning processes is harder than it should be.
    Having a minimal example of it has been invaluable.

### Compat directory
A bunch of functions that wrap functionality provided by the operating system.

### Tool directory
Utilities for testing.

### Src directory
Code for the fildesh library and program itself.

### Builtin directory
Code for the various fildesh builtins.
They all have a function signature that includes input & output files that are being passed from & to other builtins in the same process.
These input & output file arrays correspond with `argv` and should be accessed with special functions.
```c
int function_name(unsigned argc, char** argv, FildeshX** inputv, FildeshO** outputv) {
  FildeshX* in = open_arg_FildeshXF(1, argv, inputv);  /* Assuming argv[1] is an input file.*/
  FildeshO* out = open_arg_FildeshOF(2, argv, outputv);  /* Assuming argv[2] is an output file.*/
  int exstatus = 0;
  /* ... */
  close_FildeshX(in);
  close_FildeshO(out);
  return exstatus;
}
```

