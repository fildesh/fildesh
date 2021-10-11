
/** Simple utility to transpose based on a delimiter.**/

#include "fildesh.h"
#include "lace_compat_string.h"
#include "cx/alphatab.h"
#include "cx/table.h"

  int
lace_builtin_transpose_main(unsigned argc, char** argv,
                            FildeshX** inputs, FildeshO** outputs)
{
  DeclTableT( cstr_row, TableT(cstr) );
  DeclTable( cstr_row, mat );
  FildeshX* in;
  FildeshO* out;
  FildeshX slice;
  const char* const delim = argv[1];
  DeclTable( uint, row_widths );
  unsigned i;
  unsigned ncols = 0;

  if (argc != 2 || !delim) {
    fildesh_log_error("Need exactly one argument.");
    return 64;
  }

  in = open_arg_FildeshXF(0, argv, inputs);
  for (slice = sliceline_FildeshX(in);
       slice.at;
       slice = sliceline_FildeshX(in))
  {
    const char* field;
    size_t max_width = 0;
    TableT(cstr)* row = Grow1Table( mat );
    InitTable(*row);

    skipchrs_FildeshX(&slice, " ");
    for (field = gets_FildeshX(&slice, delim);
         field;
         field = gets_FildeshX(&slice, delim))
    {
      size_t width = strlen(field);
      if (width > max_width) {
        max_width = width;
      }
      PushTable( *row, lace_compat_string_duplicate(field) );
      if (row->sz > ncols) {
        ncols = row->sz;
      }
      skipchrs_FildeshX(&slice, " ");
    }
    if (row_widths.sz > 0) {
      max_width += 1;  /* 1 extra space after delim in the output.*/
    }
    PushTable( row_widths, max_width );
  }
  close_FildeshX(in);

  out = open_arg_FildeshOF(0, argv, outputs);
  for (i = 0; i < ncols; ++i) {
    unsigned j;
    for (j = 0; j < mat.sz; ++j) {
      char* field = (i < mat.s[j].sz ? mat.s[j].s[i] : NULL);
      size_t field_width = field ? strlen(field) : 0;
      size_t width_needed;
      assert(field_width <= row_widths.s[j]);
      if (field || j + 1 < mat.sz) {
        for (width_needed = row_widths.s[j] - field_width;
             width_needed > 0;
             width_needed -= 1)
        {
          putc_FildeshO(out, ' ');
        }
      }

      if (field) {
        puts_FildeshO(out, field);
        free(field);
      }
      if (j + 1 < mat.sz) {
        puts_FildeshO(out, delim);
      }
    }
    putc_FildeshO(out, '\n');
  }
  close_FildeshO(out);

  LoseTable( row_widths );
  for (i = 0; i < mat.sz; ++i) {
    LoseTable( mat.s[i] );
  }
  LoseTable( mat );
  return 0;
}

int main_transpose(unsigned argc, char** argv) {
  return lace_builtin_transpose_main(argc, argv, NULL, NULL);
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int istat = main_transpose(argc, argv);
  return istat;
}
#endif
