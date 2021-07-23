/**
 * \file syscx.h
 **/
#ifndef sysCx_H_
#define sysCx_H_

const char*
exename_of_sysCx ();
void
init_sysCx();
void
push_losefn_sysCx (void (*f) (void*), void* x);
void
lose_sysCx ();

/* synhax.h - failout_sysCx() */

#if defined(_MSC_VER)
#elif defined(__APPLE__)
#define LACE_POSIX_SOURCE
#else
#define LACE_POSIX_SOURCE
/* TODO: Figure out the correct POSIX_SOURCE to use!*/
#ifndef POSIX_SOURCE
#define POSIX_SOURCE
#endif
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
/* Needed by setenv().*/
#define _POSIX_C_SOURCE 200112L
#endif
#endif

#ifdef LACE_POSIX_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#else
#include <fcntl.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#ifdef _MSC_VER
typedef intptr_t pid_t;
#endif
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

typedef int fd_t;

#include "def.h"

bool
kill_please_sysCx(pid_t pid);

char*
mktmppath_sysCx(const char* hint);
void
setenv_sysCx (const char* key, const char* val);
void
tacenv_sysCx (const char* key, const char* val);

bool
mkdir_sysCx (const char* pathname);
bool
rmdir_sysCx (const char* pathname);
Bool
randomize_sysCx(void* p, uint size);

#endif

