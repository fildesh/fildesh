#include <stdio.h>

/** Check that a file (1st argument) has the expected content (2nd+ arguments).
 *
 * The file may contain extra newlines, spaces, tabs, and even carriage returns.
 * Keep small; built often.
 **/
int main(int argc, char** argv) {
  FILE* f = stdin;
  const char* s;
  int c, argi;
  if (argc < 3 || !argv[1] || !argv[2]) {
    return 64;  /* EX_USAGE: Command line usage error.*/
  }
  if (argv[1][0] != '-' || argv[1][1] != '\0') {
    f = fopen(argv[1], "rb");
    if (!f)  return 66;  /* EX_NOINPUT: Cannot open input file.*/
  }

  c = fgetc(f);
  argi = 2;
  s = argv[argi];
  while (c != EOF) {
    if ((char)c == *s && *s != '\0') {
      c = fgetc(f);
      s = &s[1];
    } else if (*s == '\0' && argi+1 < argc && argv[argi+1]) {
      argi += 1;
      s = argv[argi];
    } else if ((char)c == '\n' || (char)c == ' ' ||
               (char)c == '\t' || (char)c == '\r') {
      c = fgetc(f);
    } else {
      fclose(f);
      return 65;  /* EX_DATAERR: Input caused unexpected difference.*/
    }
  }
  fclose(f);
  if (*s == '\0' && argi+1 == argc) {
    return 0;  /* Everything matches!*/
  }
  return 74;  /* EX_IOERR: File is missing some expected content.*/
}
