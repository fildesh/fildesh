
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


#ifdef _MSC_VER
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
#ifdef _MSC_VER
#define restrict
#else
#define restrict __restrict
#endif
#endif

#define qual_inline static inline

#ifdef _MSC_VER
# define __FUNC__ __FUNCTION__
#else
# define __FUNC__ __func__
#endif


#include "synhax.h"

#endif

