
#include "fildesh.h"
#include "fildesh_compat_errno.h"
#include "fildesh_compat_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LineJoin LineJoin;
struct LineJoin
{
  const char* field;
  const char* lookup_line;  /* From small file.*/
  const char* stream_line;  /* From large file.*/
};

static void print_usage()
{
#define f(s)  fputs(s "\n", stderr)
  f("Usage: ujoin -x-lut LUT [OPTION]*");
  f("  -x-lut LUT -- Lookup table file where each line has a key and value");
  f("      separated by some delimiter.");
  f("  -x STREAM -- File formatted like LUT but with different values.");
  f("      If unspecified, stdin is used.");
  f("  -o FILE -- Write lines of LUT (in the order they appear in LUT) that");
  f("      were matched by a line of STREAM. The corresponding STREAM value");
  f("      is also appended to the end of the line after a 2nd delimiter.");
  f("      If FILE is unspecified, stdout is used.");
  f("  -o-not-found FILE -- Write STREAM lines that could not find a match.");
  f("  -o-conflicts FILE -- Write STREAM lines that duplicated a match.");
  f("  -n -- Nix the join field from the main output file.");
  f("  -l -- Write the STREAM value before the LUT value.");
  f("  -d DELIM -- Use a delimiter other than tab.");
  f("  -blank -- Accept any blank space character as the delimiter.");
  f("  -p DFLT -- Print unmatched lines with this default LUT value.");
#undef f
}

static int cmp_pLineJoin(const void* plhs, const void* prhs) {
  const LineJoin* lhs = *(const LineJoin* const*)plhs;
  const LineJoin* rhs = *(const LineJoin* const*)prhs;
  return strcmp(lhs->field, rhs->field);
}

  static LineJoin**
new_LineJoinMap(LineJoin** joins, size_t join_count)
{
  LineJoin** map = (LineJoin**) malloc(join_count*sizeof(LineJoin*));
  memcpy(map, joins, join_count*sizeof(map[0]));
  qsort(map, join_count, sizeof(map[0]), cmp_pLineJoin);
  return map;
}

  static LineJoin*
lookup_LineJoinMap(LineJoin** map, size_t join_count, const char* field) {
  LineJoin join;
  LineJoin* key;
  void* ret;
  join.field = field;
  key = &join;
  ret = bsearch(&key, map, join_count, sizeof(map[0]), cmp_pLineJoin);
  if (!ret) {return NULL;}
  return *(LineJoin**) ret;
}

  static LineJoin**
setup_lookup_table(FildeshX* in, const char* delim,
                   size_t* ret_join_count, FildeshAlloc* alloc)
{
  LineJoin** joins = NULL;
  size_t join_count = 0;
  fildesh_lgsize_t join_lgcount = 0;
  const unsigned delim_sz = delim ? strlen(delim) : 0;
  char* s;

  for (s = getline_FildeshX(in);
       s;
       s = getline_FildeshX(in))
  {
    LineJoin* join;

    /* Disregard a trailing empty line.*/
    if (s[0] == '\0')  break;

    s = strdup_FildeshAlloc(alloc, s);
    join = fildesh_allocate(LineJoin, 1, alloc);
    *(LineJoin**) grow_FildeshA_((void**)&joins, &join_count, &join_lgcount,
                                 sizeof(LineJoin*), 1)
      = join;
    join->field = s;
    join->lookup_line = NULL;
    join->stream_line = NULL;
    if (delim) {
      s = strstr (s, delim);
    } else {
      s = &s[strcspn (s, fildesh_compat_string_blank_bytes)];
    }

    if (!s || s[0] == '\0') {
      s = NULL;
    }

    if (s) {
      s[0] = '\0';
      if (delim)  s = &s[delim_sz];
      else        s = &s[1 + strspn (&s[1], fildesh_compat_string_blank_bytes)];
    }
    join->lookup_line = s;
  }

  *ret_join_count = join_count;
  return joins;
}

  static void
compare_lines(FildeshX* in, LineJoin** joins, size_t join_count,
              const char* delim, FildeshO* nomatch_out, FildeshO* dupmatch_out,
              FildeshAlloc* alloc)
{
  LineJoin** map = new_LineJoinMap(joins, join_count);
  const unsigned delim_sz = delim ? strlen(delim) : 0;
  unsigned line_no = 0;
  char* line;

  for (line = getline_FildeshX(in);
       line;
       line = getline_FildeshX(in))
  {
    char* field = line;
    char* payload;
    char nixed_char = '\0';
    LineJoin* join = NULL;

    ++ line_no;

    if (delim) {
      payload = strstr(line, delim);
    }
    else {
      payload = &line[strcspn(line, fildesh_compat_string_blank_bytes)];
    }

    if (!payload || payload[0] == '\0')
      payload = NULL;

    if (payload)
    {
      nixed_char = payload[0];
      payload[0] = '\0';
    }

    join = lookup_LineJoinMap(map, join_count, field);

    if (!join)
    {
      if (nomatch_out)
      {
        if (payload)  payload[0] = nixed_char;
        puts_FildeshO(nomatch_out, line);
        putc_FildeshO(nomatch_out, '\n');
      }
    }
    else if (join->stream_line)
    {
      if (payload)  payload[0] = nixed_char;
      if (dupmatch_out)
      {
        puts_FildeshO(dupmatch_out, line);
        putc_FildeshO(dupmatch_out, '\n');
      }
      else
      {
        fildesh_log_warningf( "Already a match for: %s", line );
      }
    }
    else if (!payload)
    {
      join->stream_line = join->field;
    }
    else
    {
      if (delim) {
        payload = &payload[delim_sz];
      }
      else {
        payload = &payload[1 + strspn(&payload[1],
                                      fildesh_compat_string_blank_bytes)];
      }

      join->stream_line = strdup_FildeshAlloc(alloc, payload);
    }
  }
  free(map);
}


  int
fildesh_builtin_ujoin_main(
    unsigned argc, char** argv,
    FildeshX** inputv, FildeshO** outputv)
{
  const char* delim = "\t";
  const char* dflt_record = NULL;
  bool keep_join_field = true;
  bool stream_on_left = false;
  FildeshX* lookup_in = NULL;
  FildeshX* stream_in = NULL;
  FildeshO* nomatch_out = NULL;
  FildeshO* dupmatch_out = NULL;
  FildeshO* out = NULL;
  unsigned argi;
  LineJoin** joins = NULL;
  size_t join_count = 0;
  FildeshAlloc* alloc = NULL;
  int exstatus = 0;
  unsigned i;

  for (argi = 1; argi < argc && exstatus == 0; ++argi) {
    if (0 == strcmp(argv[argi], "-x-lut")) {
      lookup_in = open_arg_FildeshXF(++argi, argv, inputv);
      if (!lookup_in) {
        fildesh_compat_errno_trace();
        fildesh_log_errorf("Cannot open %s", argv[argi]);
        exstatus = 66;
      }
    }
    else if (0 == strcmp(argv[argi], "-x")) {
      stream_in = open_arg_FildeshXF(++argi, argv, inputv);
      if (!stream_in) {
        fildesh_compat_errno_trace();
        fildesh_log_errorf("Cannot open %s", argv[argi]);
        exstatus = 66;
      }
    }
    else if (0 == strcmp(argv[argi], "-o")) {
      out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!out) {
        fildesh_log_error("Output (-o) needs an argument.");
        exstatus = 73;
      }
    }
    else if (0 == strcmp(argv[argi], "-n")) {
      keep_join_field = false;
    }
    else if (0 == strcmp(argv[argi], "-l")) {
      stream_on_left = true;
    }
    else if (0 == strcmp(argv[argi], "-blank")) {
      delim = NULL;
    }
    else if (0 == strcmp(argv[argi], "-d")) {
      delim = argv[++argi];
      if (!delim || !delim[0]) {
        fildesh_log_error("Delimiter (-d) needs an argument.");
        exstatus = 64;
      }
    }
    else if (0 == strcmp(argv[argi], "-p")) {
      dflt_record = argv[++argi];
      if (!dflt_record) {
        fildesh_log_error("Need argument for default record (-p).");
        exstatus = 64;
      }
    }
    else if (0 == strcmp(argv[argi], "-o-not-found")) {
      nomatch_out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!nomatch_out) {
        fildesh_log_error("Need argument for nomatch file (-o-not-found).");
        exstatus = 73;
      }
    }
    else if (0 == strcmp(argv[argi], "-o-conflicts")) {
      dupmatch_out = open_arg_FildeshOF(++argi, argv, outputv);
      if (!dupmatch_out) {
        fildesh_log_error("Need argument for dupmatch file (-o-conflicts).");
        exstatus = 73;
      }
    }
    else {
      fildesh_log_errorf("Unknown argument: %s", argv[argi]);
      exstatus = 64;
    }
  }

  if (exstatus == 0 && !lookup_in) {
    fildesh_log_error("Please provide a -x-lut file.");
    print_usage();
    exstatus = 64;
  }

  if (exstatus == 0 && !stream_in) {
    stream_in = open_arg_FildeshXF(0, argv, inputv);
    if (!stream_in) {
      fildesh_log_error("Cannot open stdin.");
      exstatus = 66;
    }
  }

  if (exstatus == 0 && !out) {
    out = open_arg_FildeshOF(0, argv, outputv);
    if (!out) {
      fildesh_log_error("Cannot open stdout.");
      exstatus = 73;
    }
  }

  if (exstatus == 0) {
    alloc = open_FildeshAlloc();
    joins = setup_lookup_table(lookup_in, delim, &join_count, alloc);
  }
  close_FildeshX(lookup_in);

  if (exstatus == 0) {
    compare_lines(stream_in, joins, join_count,
                  delim, nomatch_out, dupmatch_out, alloc);
  }
  close_FildeshX(stream_in);
  close_FildeshO(nomatch_out);
  close_FildeshO(dupmatch_out);

  if (!delim)  delim = "\t";
  for (i = 0; i < join_count && exstatus == 0; ++i) {
    LineJoin* join = joins[i];
    const char* stream_line = join->stream_line;
    if (!stream_line && dflt_record)
      stream_line = dflt_record;
    if (stream_line)
    {
      bool tab = false;
      if (keep_join_field)
      {
        puts_FildeshO(out, join->field);
        tab = true;
      }

      if (stream_on_left && stream_line != join->field) {
        if (tab) {puts_FildeshO(out, delim);}
        tab = true;
        puts_FildeshO(out, stream_line);
      }

      if (join->lookup_line)
      {
        if (tab) {puts_FildeshO(out, delim);}
        tab = true;
        puts_FildeshO(out, join->lookup_line);
      }

      if (!stream_on_left && stream_line != join->field) {
        if (tab) {puts_FildeshO(out, delim);}
        tab = true;
        puts_FildeshO(out, stream_line);
      }
      putc_FildeshO(out, '\n');
    }
  }
  close_FildeshO(out);

  if (joins) {free(joins);}
  close_FildeshAlloc(alloc);
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
  int
main(int argc, char** argv)
{
  return fildesh_builtin_ujoin_main((unsigned)argc, argv, NULL, NULL);
}
#endif
