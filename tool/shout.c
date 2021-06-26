/** Write to a file (1st argument) with content (2nd+ arguments).
 *
 * The content is separated by spaces and ends with a newline.
 * Keep small; built often.
 **/
#include <stdio.h>

#ifndef LACE_TOOL_LIBRARY
#define lace_tool_shout_main main
#endif
int lace_tool_shout_main(int argc, char** argv) {
  FILE* f = stdout;
  int argi;
  if (argc < 3 || !argv[1] || !argv[2]) {
    return 64;  /* EX_USAGE: Command line usage error.*/
  }
  if (argv[1][0] != '-' || argv[1][1] != '\0') {
    f = fopen(argv[1], "wb");
    if (!f)  return 73;  /* EX_CANTCREAT: Cannot open output file.*/
  }

  for (argi = 2; argi < argc && argv[argi]; ++argi) {
    if (argi > 2 && 0 > fputc(' ', f)) {
      return 74;  /* EX_IOERR: A write failed.*/
    }
    if (0 > fputs(argv[argi], f))  return 74;
  }
  if (0 > fputc('\n', f))  return 74;
  if (0 != fclose(f))  return 74;
  return 0;  /* Success!*/
}
