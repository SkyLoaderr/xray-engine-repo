#ifndef STDMIX_INCLUDED // -*- C++ -*-
#define STDMIX_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

#include "xrCore.h"

/************************************************************************

  Standard base include file for the MIX library.  This defines various
  common stuff that is used elsewhere.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: stdmix.h,v 1.21.2.1 2002/01/31 18:38:37 garland Exp $

 ************************************************************************/


/* Define this if your compiler doesn't support the new C++ 'bool' type */
#define HAVE_BOOL 1

/* Define if your system supports rint() */
/* #define HAVE_RINT 1 */

/* Define if your system supports getrusage() */
/* #undef HAVE_GETRUSAGE */

/* Define if your system does not support getrusage() but supports times() */
/* #undef HAVE_TIMES */

/* Define if your system supports random() as opposed to just rand() */
/* #undef HAVE_RANDOM */

/* Define if the STL hash_map template is available */
/* #undef HAVE_HASH_MAP */

/* Define if the STL valarray  template is available */
#define HAVE_VALARRAY 1

/* Define if the FLTK GUI toolkit is available */
#define HAVE_LIBFLTK 1

/* Define if Sam Leffler's libtiff is available */
#define HAVE_LIBTIFF 1

/* Define if libpng is available */
#define HAVE_LIBPNG 1

/* Define if the Independent JPEG Group's libjpeg is available */
#define HAVE_LIBJPEG 1

/* ***** OpenGL configuration section ***** */

/* Define to the name of OpenGL implementation (e.g., "OpenGL" or "Mesa") */
#define HAVE_OPENGL "OpenGL"

/* Define if glPolygonOffsetEXT is available */
/* #undef HAVE_POLYOFFSET_EXT */

/* Define if glPolygonOffset is available */
#define HAVE_POLYOFFSET 1

/* Define if <GL/glext.h> header is available */
#define HAVE_GL_GLEXT_H 1

/* Define if <GL/glxext.h> header is available */
/* #undef HAVE_GL_GLXEXT_H */

/* Define if <GL/wglext.h> header is available */
#define HAVE_GL_WGLEXT_H 1

/* Visual C++ gets confused by too many casting and [] operators */
#define HAVE_CASTING_LIMITS 1

#if defined(_DEBUG) && defined(_MSC_VER)
// STL makes Visual C++ complain about identifiers longer than 255
// characters.  Unfortunately, this may limit the debugability of
// code that uses STL.
#pragma warning (disable : 4786)

#include <cstdlib>
#include <cmath>
#include <climits>
#include <iostream>
#endif

////////////////////////////////////////////////////////////////////////
//
// Standards notwithstanding, not all platforms define exactly the
// same things in their header files.  We try to compensate here.
//

#if !defined(HAVE_BOOL)
typedef int bool;
const bool false = 0;
const bool true = 1;
#endif

// Microsoft doesn't define std::min() & std::max() because it conflicts with
// <windef.h>, and their _MIN/_MAX macro workarounds don't add the std::
// namespace qualification.  These macros provide a uniform way of getting
// around this problem.
//
#ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288419716939937510582097494459
#endif

#if !defined(HUGE) && defined(HUGE_VAL)
#  define HUGE HUGE_VAL
#endif

#if !defined(HAVE_RINT)
inline double rint(double x) { return floor(x + 0.5); }
#endif

////////////////////////////////////////////////////////////////////////
//
//
//

#if defined(HAVE_RANDOM)
inline double random1() { return (double)random() / (double)LONG_MAX; }
inline char   random_byte() { return (char)(random() & 0xff); }
#else
inline double random1() { return (double)rand() / (double)RAND_MAX; }
inline char   random_byte() { return (char)(rand() & 0xff); }
#endif

const double FEQ_EPS = 1e-6;
const double FEQ_EPS2 = 1e-12;

inline bool  FEQ(double a, double b, double e=FEQ_EPS)  {return _abs(a-b)<e;}
inline bool FEQ2(double a, double b, double e=FEQ_EPS2) {return _abs(a-b)<e;}


////////////////////////////////////////////////////////////////////////
//
//
//

#define TIMING(t, cmd) { t=get_cpu_time(); cmd; t=get_cpu_time() - t; }
extern double get_cpu_time();





















using namespace std;
#include <string.h>

#ifdef UINT_MAX
#  define MXID_NIL UINT_MAX
#else
#  define MXID_NIL 0xffffffffU
#endif

#ifndef MIX_NO_AXIS_NAMES
enum Axis {X=0, Y=1, Z=2, W=3};
#endif

inline bool streq(const char *a, const char *b) { return !strcmp(a,b); }

////////////////////////////////////////////////////////////////////////
//
// Optimization control, debugging, and error reporting facilities
//

//
// Safety levels:
//
//       -2 Reckless
//       -1 Optimized
//        0 Normal
//        1 Cautious
//        2 Paranoid
//
#ifndef SAFETY
// Default safety policy is to take the middle of the road
#define SAFETY 0
#endif

#include "mixmsg.h"

#define fatal_error(s) mxmsg_signal(MXMSG_FATAL, s, NULL, __FILE__, __LINE__)

#ifdef assert
#  undef assert
#endif

#if SAFETY >= 0
#  define assert(i) (i)?((void)NULL):mxmsg_signal(MXMSG_ASSERT, # i, \
						  NULL, __FILE__, __LINE__)
#  define CAREFUL(x) x
#else
#  define assert(i)
#  define CAREFUL(x)
#endif

#if SAFETY==2
#  define SanityCheck(t) assert(t)
#  define PARANOID(x) x
#else
#  define SanityCheck(t)
#  define PARANOID(x)
#endif

#if SAFETY > 0
#  define AssertBound(t)  assert(t)
#  define PRECAUTION(x) x
#else
#  define AssertBound(t)
#  define PRECAUTION(x)
#endif

// STDMIX_INCLUDED
#endif
