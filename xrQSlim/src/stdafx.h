#ifndef stdafxH
#define stdafxH

#if !defined(__GNUC__)
#  pragma once
#endif

#include <xrCore.h>

#if defined(_DEBUG) && defined(_MSC_VER)
// STL makes Visual C++ complain about identifiers longer than 255
// characters.  Unfortunately, this may limit the debugability of
// code that uses STL.
#pragma warning (disable : 4786)
#endif

inline bool streq(const char *a, const char *b) { return !xr_strcmp(a,b); }

#pragma comment( lib, "x:\\xrCore.lib"	)

#endif
