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

#endif

