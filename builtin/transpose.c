
/** Simple utility to transpose based on a delimiter.**/

#include "fildesh.h"
#include "fildesh_compat_string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct TransposeLine TransposeLine;
struct TransposeLine {
  char** fields;
  size_t field_count;
  size_t max_width;
};

  static void
parse_row(
    FildeshX* row_in, TransposeLine* row,
    char*** fields_buf, fildesh_lgsize_t* field_lgcount,
    const unsigned char* delim, bool pad, FildeshAlloc* alloc)
{
  FildeshX field;
  const size_t delim_size = strlen((char*)delim);
  row->max_width = 0;
  row->field_count = 0;

  if (pad) {skipchrs_FildeshX(row_in, fildesh_compat_string_blank_bytes);}
  for (field = until_bytestring_FildeshX(row_in, delim, delim_size);
       field.at;
       field = until_bytestring_FildeshX(row_in, delim, delim_size))
  {
    if (field.size > row->max_width) {
      row->max_width = field.size;
    }
    *(char**) grow_FildeshA_(
        (void**)fields_buf, &row->field_count, field_lgcount,
        sizeof(char*), 1)
      = strdup_FildeshX(&field, alloc);

    skipstr_FildeshX(row_in, (const char*)delim);
    if (pad) {skipchrs_FildeshX(row_in, fildesh_compat_string_blank_bytes);}
  }
  row->fields = fildesh_allocate(char*, row->field_count, alloc);
  memcpy(row->fields, *fields_buf, row->field_count * sizeof(char*));
}

  int
fildesh_builtin_transpose_main(unsigned argc, char** argv,
                               FildeshX** inputs, FildeshO** outputs)
{
  FildeshAlloc* alloc = NULL;
  FildeshX* in = NULL;
  FildeshO* out = NULL;
  FildeshX slice;
  bool pad = false;
  const char* delim = NULL;
  unsigned i;
  unsigned ncols = 0;
  size_t line_count = 0;
  TransposeLine* mat = NULL;
  char** fields_buf = NULL;
  fildesh_lgsize_t line_lgcount = 0;
  fildesh_lgsize_t field_lgcount = 0;
  int exstatus = 0;

  for (i = 1; i < argc && exstatus == 0; ++i) {
    if (0 == strcmp(argv[i], "-d")) {
      delim = argv[++i];
      if (!delim || !delim[0]) {
        fildesh_log_error("Delimiter (-d) needs an argument.");
        exstatus = 64;
      }
    }
    else if (0 == strcmp(argv[i], "-blank")) {
      pad = true;
    }
    else {
      fildesh_log_errorf("Unknown argument: %s", argv[i]);
      exstatus = 64;
    }
  }

  if (exstatus == 0 && !delim && !pad) {
    delim = "\t";
  }

  if (exstatus == 0 && !delim) {
    fildesh_log_error("Please specify a delimiter to go with blank space.");
    exstatus = 64;
  }

  if (exstatus == 0) {
    alloc = open_FildeshAlloc();
  }

  in = open_arg_FildeshXF(0, argv, inputs);
  for (slice = sliceline_FildeshX(in);
       slice.at && exstatus == 0;
       slice = sliceline_FildeshX(in))
  {
    TransposeLine* row = (TransposeLine*)
      grow_FildeshA_((void**) &mat, &line_count, &line_lgcount,
                     sizeof(TransposeLine), 1);
    parse_row(&slice, row, &fields_buf, &field_lgcount,
              (const unsigned char*)delim, pad, alloc);
    if (line_count > 1) {
      row->max_width += 1;  /* 1 extra space after delim in the output.*/
    }
    if (row->field_count > ncols) {
      ncols = row->field_count;
    }
  }
  close_FildeshX(in);

  if (fields_buf) {free(fields_buf);}

  out = open_arg_FildeshOF(0, argv, outputs);
  for (i = 0; i < ncols && exstatus == 0; ++i) {
    size_t j;
    for (j = 0; j < line_count; ++j) {
      char* field = (i < mat[j].field_count ? mat[j].fields[i] : NULL);
      size_t field_width = field ? strlen(field) : 0;
      size_t width_needed;
      assert(field_width <= mat[j].max_width);
      if (pad && (field || j + 1 < line_count)) {
        for (width_needed = mat[j].max_width - field_width;
             width_needed > 0;
             width_needed -= 1)
        {
          putc_FildeshO(out, ' ');
        }
      }

      if (field) {
        puts_FildeshO(out, field);
      }
      if (j + 1 < line_count) {
        puts_FildeshO(out, delim);
      }
    }
    putc_FildeshO(out, '\n');
  }
  close_FildeshO(out);

  if (mat) {free(mat);}
  close_FildeshAlloc(alloc);
  return exstatus;
}

#if !defined(FILDESH_BUILTIN_LIBRARY) && !defined(UNIT_TESTING)
  int
main(int argc, char** argv)
{
  return fildesh_builtin_transpose_main((unsigned)argc, argv, NULL, NULL);
}
#endif
