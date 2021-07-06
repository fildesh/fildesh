
#include "lace.h"
#include "lace_compat_errno.h"
#include "cx/alphatab.h"
#include "cx/associa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LineJoin LineJoin;
struct LineJoin
{
  AlphaTab field;
  char* lookup_line;  /* From small file.*/
  char* stream_line;  /* From large file.*/
};

DeclTableT( LineJoin, LineJoin );

  void
init_LineJoin (LineJoin* join)
{
  join->field = dflt_AlphaTab ();
  join->lookup_line = 0;
  join->stream_line = 0;
}

  void
lose_LineJoin (LineJoin* join)
{
  if (join->stream_line && join->stream_line != join->field.s)
    free (join->stream_line);
  lose_AlphaTab (&join->field);
}

  static void
show_usage_and_exit ()
{
#define f(s)  fputs(s "\n", stderr)
  f("Usage: ujoin SMALL LARGE [OPTION]*");
  f("    SMALL is a file used for lookup.");
  f("    LARGE can be a stream, which tries to match the fields in SMALL.");
  f("    -x  Nix the join field.");
  f("    -l  Output from LARGE on the left.");
  f("    -ws  Use any whitespace as the delimiter.");
  f("    -d DELIM  Use a delimiter other than tab.");
  f("    -p DFLT  Default record to use when it is empty.");
  f("    -nomatch FILE  Put lines whose fields could not be matched here.");
  f("    -dupmatch FILE  Put fields who got matched here.");
#undef f
  exit(64);
}

  static TableT(LineJoin)
setup_lookup_table(LaceX* in, const char* delim)
{
  DeclTable( LineJoin, table );
  const uint delim_sz = delim ? strlen (delim) : 0;
  char* s;

  for (s = getline_LaceX(in);
       s;
       s = getline_LaceX(in))
  {
    LineJoin* join;

    /* Disregard a trailing empty line.*/
    if (s[0] == '\0')  break;

    join = Grow1Table( table );
    init_LineJoin (join);
    join->field = cons1_AlphaTab (s);
    s = cstr_AlphaTab (&join->field);
    if (delim)
      s = strstr (s, delim);
    else
      s = &s[strcspn (s, WhiteSpaceChars)];

    if (!s || s[0] == '\0')
      s = 0;

    if (s)
    {
      join->field.sz = IdxEltTable( join->field, s );
      s[0] = 0;
      if (delim)  s = &s[delim_sz];
      else        s = &s[1 + strspn (&s[1], WhiteSpaceChars)];
    }
    join->lookup_line = s;
  }

  PackTable( table );
  return table;
}

  static void
compare_lines(LaceX* in, Associa* map, const char* delim,
              LaceO* nomatch_out, LaceO* dupmatch_out)
{
  const uint delim_sz = delim ? strlen (delim) : 0;
  uint line_no = 0;
  char* line;

  for (line = getline_LaceX(in);
       line;
       line = getline_LaceX(in))
  {
    char* field = line;
    char* payload;
    char nixed_char = '\0';
    LineJoin* join = 0;

    ++ line_no;

    if (delim)  payload = strstr (line, delim);
    else        payload = &line[strcspn (line, WhiteSpaceChars)];

    if (!payload || payload[0] == '\0')
      payload = 0;

    if (payload)
    {
      nixed_char = payload[0];
      payload[0] = '\0';
    }

    {
      AlphaTab ts = dflt1_AlphaTab (field);
      Assoc* assoc = lookup_Associa (map, &ts);
      join = (assoc ? *(LineJoin**) val_of_Assoc (map, assoc) : 0);
    }

    if (!join)
    {
      if (nomatch_out)
      {
        if (payload)  payload[0] = nixed_char;
        puts_LaceO(nomatch_out, line);
        putc_LaceO(nomatch_out, '\n');
      }
    }
    else if (join->stream_line)
    {
      if (payload)  payload[0] = nixed_char;
      if (dupmatch_out)
      {
        puts_LaceO(dupmatch_out, line);
        putc_LaceO(dupmatch_out, '\n');
      }
      else
      {
        DBog1( "Already a match for: %s", line );
      }
    }
    else if (!payload)
    {
      join->stream_line = join->field.s;
    }
    else
    {
      if (delim)
        payload = &payload[delim_sz];
      else
        payload = &payload[1 + strspn (&payload[1], WhiteSpaceChars)];

      join->stream_line = dup_cstr (payload);
    }
  }
}


  int
main_ujoin(unsigned argc, char** argv)
{
  const char* delim = "\t";
  const char* dflt_record = 0;
  bool keep_join_field = true;
  bool stream_on_left = false;
  LaceX* lookup_in = NULL;
  LaceX* stream_in = NULL;
  LaceO* nomatch_out = NULL;
  LaceO* dupmatch_out = NULL;
  LaceO* out = NULL;
  unsigned argi = 1;
  TableT(LineJoin) table;
  Associa map[1];
  uint i;

  InitAssocia( AlphaTab, LineJoin*, *map, cmp_AlphaTab );

  if (argi + 2 <= argc) {
    lookup_in = open_LaceXF(argv[argi++]);
    stream_in = open_LaceXF(argv[argi++]);
  }
  if (!lookup_in || !stream_in) {
    lace_compat_errno_trace();
    show_usage_and_exit();
  }

  while (argi < argc)
  {
    const char* arg = argv[argi];
    ++ argi;
    if (0 == strcmp (arg, "-h"))
    {
      show_usage_and_exit ();
    }
    else if (0 == strcmp (arg, "-o"))
    {
      out = open_LaceOF(argv[argi++]);
      if (!out) {
        lace_log_error("Output (-o) needs an argument.");
        return 64;
      }
    }
    else if (0 == strcmp (arg, "-x"))
    {
      keep_join_field = false;
    }
    else if (0 == strcmp (arg, "-l"))
    {
      stream_on_left = true;
    }
    else if (0 == strcmp (arg, "-ws"))
    {
      delim = 0;
    }
    else if (0 == strcmp (arg, "-d"))
    {
      delim = argv[argi++];
      if (!delim) {
        lace_log_error("Delimiter (-d) needs an argument.");
        return 64;
      }
      Claim2( strlen (delim) ,>, 0 );
    }
    else if (0 == strcmp (arg, "-p"))
    {
      dflt_record = argv[argi++];
      if (!dflt_record) {
        lace_log_error("Need argument for default record (-p).");
        return 64;
      }
    }
    else if (0 == strcmp (arg, "-nomatch"))
    {
      nomatch_out = open_LaceOF(argv[argi++]);
      if (!nomatch_out) {
        lace_log_error("Need argument for nomatch file (-nomatch).");
        return 64;
      }
    }
    else if (0 == strcmp (arg, "-dupmatch"))
    {
      dupmatch_out = open_LaceOF(argv[argi++]);
      if (!dupmatch_out) {
        lace_log_error("Need argument for dupmatch file (-dupmatch).");
        return 64;
      }
    }
    else
    {
      lace_log_errorf("Unknown argument: %s", arg);
      return 64;
    }
  }

  table = setup_lookup_table(lookup_in, delim);
  close_LaceX(lookup_in);

  UFor( i, table.sz ) {
    LineJoin* join = &table.s[i];
    insert_Associa (map, &join->field, &join);
  }

  compare_lines(stream_in, map, delim, nomatch_out, dupmatch_out);
  close_LaceX(stream_in);
  if (nomatch_out) {close_LaceO(nomatch_out);}
  if (dupmatch_out) {close_LaceO(dupmatch_out);}

  if (!out) {
    out = open_LaceOF("-");
    if (!out) {
      lace_log_error("Cannot open stdout.");
      return 1;
    }
  }

  if (!delim)  delim = "\t";
  UFor( i, table.sz ) {
    LineJoin* join = &table.s[i];
    const char* stream_line = join->stream_line;
    if (!stream_line && dflt_record)
      stream_line = dflt_record;
    if (stream_line)
    {
      bool tab = false;
      if (keep_join_field)
      {
        puts_LaceO(out, join->field.s);
        tab = true;
      }

      if (stream_on_left && stream_line != join->field.s)
      {
        if (tab) {puts_LaceO(out, delim);}
        tab = true;
        puts_LaceO(out, stream_line);
      }

      if (join->lookup_line)
      {
        if (tab) {puts_LaceO(out, delim);}
        tab = true;
        puts_LaceO(out, join->lookup_line);
      }

      if (!stream_on_left && stream_line != join->field.s)
      {
        if (tab) {puts_LaceO(out, delim);}
        tab = true;
        puts_LaceO(out, stream_line);
      }
      putc_LaceO(out, '\n');
    }
    lose_LineJoin (join);
  }
  close_LaceO(out);

  LoseTable( table );
  lose_Associa (map);
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int istat = main_ujoin(argc, argv);
  return istat;
}
#endif
