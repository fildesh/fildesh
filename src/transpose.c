
/** Simple utility to transpose based on a delimiter.**/

#include "cx/syscx.h"
#include "cx/xfile.h"
#include "cx/ofile.h"

int main(int argc, char** argv)
{
  int argi = init_sysCx (&argc, &argv);
  DeclTableT( cstr_row, TableT(cstr) );
  DeclTable( cstr_row, mat );
  XFile* xf = stdin_XFile ();
  OFile* of = stdout_OFile ();
  const char* line;
  const char* delim;
  uint coli = 0;
  DeclTable( uint, widths );

  if (argi+1 != argc)
    failout_sysCx ("Need exactly one argument.");
  delim = argv[argi++];

  mayflush_XFile (xf, Yes);

  for (line = getline_XFile (xf);
       line;
       line = getline_XFile (xf))
  {
    const char* field;
    uint rowi = 0;
    XFile olay[1];
    olay_txt_XFile (olay, xf, IdxEltTable( xf->buf, line ));

    for (field = getlined_XFile (olay, delim);
         field;
         field = getlined_XFile (olay, delim))
    {
      TableT(cstr)* row;
      {:if (rowi >= mat.sz)
        row = Grow1Table( mat );
        InitTable( *row );
      }
      {:else
        row = &mat.s[rowi];
      }

      {:while (coli >= row->sz)
        PushTable( *row, 0 );
      }

      row->s[coli] = dup_cstr (field);
      ++ rowi;
    }

    ++ coli;
  }

  {:while (widths.sz < coli)
    PushTable( widths, 0 );
  }

  {:for (i ; mat.sz)
    TableT(cstr)* row = &mat.s[i];
    {:for (j ; coli)
      char* field;
      {:if (j >= row->sz)
        PushTable( *row, 0 );
      }
      field = row->s[j];

      {:if (field)
        uint w = strlen (field);
        {:while (w > 0 && strchr (WhiteSpaceChars, field[w-1]))
          field[--w] = '\0';
        }
        if (w > widths.s[j])
          widths.s[j] = w;
      }
    }
  }

  {:for (i ; mat.sz)
    TableT(cstr)* row = &mat.s[i];
    {:for (j ; row->sz)
      char* field = row->s[j];

      printf_OFile (of, "%*s%s%s",
                    widths.s[j],
                    (field ? field : ""),
                    (j + 1 < row->sz ? " " : ""),
                    (j + 1 < row->sz ? delim : ""),
                    (j + 1 < row->sz ? " " : ""));
      free (field);
    }
    oput_char_OFile (of, '\n');
    LoseTable( *row );
  }

  LoseTable( mat );
  LoseTable( widths );
  lose_sysCx ();
  return 0;
}


