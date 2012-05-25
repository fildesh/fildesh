
#ifndef SYS_CX_H_
#define SYS_CX_H_

void
init_sys_cx ();
void
push_losefn_sys_cx (void (*f) ());
void
push_losefn1_sys_cx (void (*f) (void*), void* x);
void
lose_sys_cx ();

    /* synhax.h - dbglog_printf3() */
    /* fileb.h - stdin_FileB () */
    /* fileb.h - stdout_FileB () */
    /* fileb.h - stderr_FileB () */

#ifdef IncludeC
#include "sys-cx.c"
#endif
#endif

