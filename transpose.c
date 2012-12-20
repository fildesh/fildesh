
/** Simple utility to transpose based on a delimiter.**/

#include "cx/syscx.h"
#include "cx/fileb.h"
#include "cx/table.h"

int main(int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    DeclTableT( cstr_row, TableT(cstr) );
    DeclTable( cstr_row, mat );
    XFileB* xf = stdin_XFileB ();
    OFileB* of = stdout_OFileB ();
    const char* line;
    const char* delim;
    uint coli = 0;
    DeclTable( uint, widths );

    if (argi+1 != argc)
        failout_sysCx ("Need exactly one argument.");
    delim = argv[argi++];

    mayflush_XFileB (xf, Yes);

    for (line = getline_XFileB (xf);
         line;
         line = getline_XFileB (xf))
    {
        const char* field;
        uint rowi = 0;
        XFileB olay = olay_XFileB (xf, IdxEltTable( xf->buf, line ));

        for (field = getlined_XFileB (&olay, delim);
             field;
             field = getlined_XFileB (&olay, delim))
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

            printf_OFileB (of, "%*s%s%s",
                           widths.s[j],
                           (field ? field : ""),
                           (j + 1 < row->sz ? " " : ""),
                           (j + 1 < row->sz ? delim : ""),
                           (j + 1 < row->sz ? " " : ""));
            free (field);
        }
        oput_char_OFileB (of, '\n');
        LoseTable( *row );
    }

    LoseTable( mat );
    LoseTable( widths );
    lose_sysCx ();
    return 0;
}


