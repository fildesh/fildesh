/**
 * \file syscx.h
 **/
#ifndef sysCx_H_
#define sysCx_H_

#include "def.h"

void
init_sysCx();
void
push_losefn_sysCx (void (*f) (void*), void* x);
void
lose_sysCx ();

char*
mktmppath_sysCx(const char* hint);
void
tacenv_sysCx (const char* key, const char* val);

bool
mkdir_sysCx (const char* pathname);
Bool
randomize_sysCx(void* p, uint size);

#endif

