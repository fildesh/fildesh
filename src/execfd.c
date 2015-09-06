
#include "utilace.h"
#include "cx/fileb.h"
#include "cx/bittable.h"

#include <sys/stat.h>

static
  void
show_usage_and_exit ()
{
  OFile* of = stderr_OFile ();
#define f(s)  oput_cstr_OFile (of, s); oput_char_OFile (of, '\n');

  printf_OFile (of, "Usage: %s [-exe name] [argi]* -- [arg|fd]*\n",
                exename_of_sysCx ());
  f( "  Each /argi/ is an index which has a file descriptor in it." );
  f( "  Index zero is the executable name." );
#undef f
  failout_sysCx ("Bad args...");
}

/** Read from the file descriptor /in/ and write to file /name/.
 * If the input stream contains no data, the file will not be
 * written (or overwritten).
 **/
static
  void
pipe_to_file (fd_t in, const char* name)
{
  FILE* out = 0;

  while (true)
  {
    jint sz;
    byte buf[BUFSIZ];

    sz = read_sysCx (in, buf, BUFSIZ);

    if (sz <= 0)  break;
    if (!out)  out = fopen (name, "wb");

    fwrite (buf, 1, sz, out);
  }

  closefd_sysCx (in);
  if (out)  fclose (out);
}


static
  char*
readin_fd (fd_t in, bool scrap_newline)
{
  const uint n_per_chunk = 8192;
  DeclTable( char, buf );
  uint off = 0;

  while (1)
  {
    jint n;
    n = off + n_per_chunk + 1;
    if (buf.sz < (size_t) n)
      GrowTable( buf, n - buf.sz );

    n = read_sysCx (in, &buf.s[off], n_per_chunk * sizeof(char));
    if (n < 0)
      failout_sysCx ("Problem reading file descriptor!");
    if (n == 0)  break;
    off += n;
  }
  closefd_sysCx (in);

  MPopTable( buf, buf.sz - off + 1 );
  buf.s[off] = '\0';
  if (scrap_newline && off > 0 && buf.s[off-1] == '\n')
  {
    buf.s[--off] = '\0';
    if (off > 0 && buf.s[off-1] == '\r')
      buf.s[--off] = '\0';
  }
  return buf.s;
}

LaceUtilMain(execfd)
{
  int off = 0;
  BitTable bt = cons2_BitTable (argc, 0);
  char* exe = 0;

  while (!eql_cstr (argv[argi], "--"))
  {
    uint idx = 0;
    if (!argv[argi])
    {
      show_usage_and_exit ();
    }
    else if (eql_cstr (argv[argi], "-exe"))
    {
      exe = argv[++argi];
    }
    else if (xget_uint_cstr (&idx, argv[argi]))
    {
      set1_BitTable (bt, idx);
    }
    else
    {
      DBog1( "Cannot parse index from arg: %s", argv[argi] );
      show_usage_and_exit ();
    }
    ++ argi;
  }
  off = ++ argi;

  while (argv[argi])
  {
    if (test_BitTable (bt, argi-off))
    {
      uint fd = 0;
      if (!xget_uint_cstr (&fd, argv[argi]))
      {
        DBog1( "Cannot parse fd from arg: %s", argv[argi] );
        show_usage_and_exit ();
      }
      else if (argi == off)
      {
        if (!exe)
          failout_sysCx ("Need to provide -exe argument.");

        pipe_to_file (fd, exe);
        argv[argi] = exe;
        chmodu_sysCx (argv[argi], true, true, true);
      }
      else
      {
        argv[argi] = readin_fd (fd, true);
        push_losefn1_sysCx ((void (*) (void*)) free, argv[argi]);
      }
    }
    ++ argi;
  }

  lose_BitTable (&bt);
  {
    int ret = lace_util_main (off, argc, argv);
    if (ret >= 0)
      return ret;
  }
  execvp_sysCx (&argv[off]);

  lose_sysCx ();
  return 1;
}

