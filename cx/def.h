
#ifndef DEF_H_
#define DEF_H_
#include <float.h>
#include <limits.h>
#include <stddef.h>

typedef int Bit;
enum Trit { Nil = 0, Yes = 1, May = 2 };
typedef enum Trit Trit;

    /** Define bool.**/
#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

typedef unsigned char byte;
#define NBitsInByte 8
#define NBits_byte 8

typedef unsigned int uint;
#define Max_uint UINT_MAX

#if 0
typedef double real;
#define Max_real DBL_MAX
#define Min_real (-DBL_MAX)
#define Small_real DBL_MIN
#define Epsilon_real DBL_EPSILON
#define realPackSz 2
#define GL_REAL GL_DOUBLE

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef DistribCompute
#define MPI_real MPI_DOUBLE
#endif

#else
typedef float real;
#define Max_real FLT_MAX
#define Min_real (-FLT_MAX)
#define Small_real FLT_MIN
#define Epsilon_real FLT_EPSILON
#define realPackSz 4
#define GL_REAL GL_FLOAT

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifdef DistribCompute
#define MPI_real MPI_FLOAT
#endif

#endif


#ifdef _WIN32
    /* Disable warning: 'fopen' unsafe, use fopen_s instead */
    /* REF CMakeLists.txt: Define _CRT_SECURE_NO_WARNINGS */

    /* Disable: conditional expression is constant */
# pragma warning (disable : 4127)
    /* Disable: conversion from 'uint' to 'real' */
# pragma warning (disable : 4244)
    /* Disable: conversion from 'double' to 'float' */
# pragma warning (disable : 4305)
#endif


#if __STDC_VERSION__ < 199901L
#define inline __inline
#define restrict __restrict
#endif

#define Concatify(a,b) a ## b
#define ConcatifyDef(a,b)  Concatify(a,b)

#define ArraySz( a )  sizeof(a) / sizeof(*a)

#define CastUp( T, field, p ) \
    ((T*) ((ptrdiff_t) p - offsetof( T, field )))

#define IndexOf( T, a, e ) \
    (((ptrdiff_t) (e) - (ptrdiff_t) (a)) / sizeof (T))

#define BSfx( a, op, b, sfx )  (a)sfx op (b)sfx

#define UFor( i, bel )  for (i = 0; i < (bel); ++i)
#define BLoop( i, bel )  uint i; for (i = 0; i < (bel); ++i) {
#define BLose() }

#define Claim( x )  assert(x)
#define Claim2( a ,op, b )  assert((a) op (b))

#define AccepTok( line, tok ) \
    ((0 == strncmp ((line), (tok), strlen(tok))) \
     ? ((line) = &(line)[strlen(tok)]) \
     : 0)

#define DecloStack( T, x )  T onstack_##x; T* const restrict x = &onstack_##x

#endif

