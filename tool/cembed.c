/**
 * Embed file contents into C code.
 *
 * Usage example:
 *   cembed -o shaders.h phong.vert phong.frag diffuse.vert diffuse.frag
 *
 * Then you can #include "shaders.h" from within some function
 * and use following (static const) variables:
 *   unsigned int nfiles = 4;
 *   unsigned int files_nbytes[4] = { all file byte counts };
 *   unsigned char* files_bytes[4] = { all file contents null-terminated };
 *
 * The null terminators in {files_bytes} are for convenience
 * and do not affect byte counts given in {files_nbytes}.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;
typedef unsigned char byte;

#define BailOut(ret, msg) \
  do { \
    fprintf (err, "%s\n", msg); \
    fprintf (err, "Usage: %s -o out.h in1 [... inN]\n", argv[0]); \
    return ret; \
  } while (0)

int main (int argc, const char** argv)
{
  int argi = 1;
  const char nfilessym[] = "nfiles";
  const char arrsym[] = "files_bytes";
  const char lensym[] = "files_nbytes";
  const char arrtype[] = "unsigned char";
  const char lentype[] = "unsigned int";
  FILE* out = stdout;
  FILE* err = stderr;
  uint i, nfiles;
  uint* files_nbytes;

  if (argi < argc) {
    if (0 != strcmp(argv[argi], "-o")) {
      BailOut(1, "Please use the -o flag to be explicit about the output file.");
    }
    argi += 1;
  }

  if (argi+1 >= argc) {
    BailOut(1, "Need at least one output and input file.");
  }

  out = fopen (argv[argi], "wb");
  if (!out)
  {
    fprintf (err, "%s: Cannot write:%s\n", argv[0], argv[argi]);
    return 1;
  }

  ++ argi;

  nfiles = (argc - argi);
  files_nbytes = (uint*) malloc(nfiles*sizeof(uint));
  fprintf (out, "static const %s %s=%u;\n", lentype, nfilessym, nfiles);

  for (i = 0; i < nfiles; ++i) {
    FILE* in;
    byte buf[BUFSIZ];
    const uint buflen = BUFSIZ;
    uint n;

    in = fopen (argv[argi + i], "rb");
    if (!in) {
      fprintf (err, "%s: Cannot read:%s\n", argv[0], argv[argi + i]);
      return 1;
    }

    fprintf (out, "static const %s %s_%u[]={", arrtype, arrsym, i);
    files_nbytes[i] = 0;
    for (n = fread (buf, 1, buflen, in);
         n > 0;
         n = fread (buf, 1, buflen, in))
    {
      uint j;
      files_nbytes[i] += n;
      for (j = 0; j < n; ++j) {
        fprintf (out, "%u,", buf[j]);
      }
    }

    fputs ("0};\n", out);

    fclose (in);
  }

  fprintf (out, "static const %s %s[]={", lentype, lensym);
  for (i = 0; i < nfiles; ++i) {
    if (i > 0)  fputc (',', out);
    fprintf (out, "%u", files_nbytes[i]);
  }
  fputs ("};", out);

  fputc ('\n', out);

  fprintf (out, "static const %s* const %s[]={", arrtype, arrsym);
  for (i = 0; i < nfiles; ++i) {
    if (i > 0)  fputc (',', out);
    fprintf (out, "%s_%u", arrsym, i);
  }
  fputs ("};\n", out);

  free (files_nbytes);
  fclose (out);
  return 0;
}

