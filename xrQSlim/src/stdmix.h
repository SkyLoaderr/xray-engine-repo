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
//
//

const double FEQ_EPS = 1e-6;
const double FEQ_EPS2 = 1e-12;

inline bool  FEQ(double a, double b, double e=FEQ_EPS)  {return _abs(a-b)<e;}
inline bool FEQ2(double a, double b, double e=FEQ_EPS2) {return _abs(a-b)<e;}

inline bool streq(const char *a, const char *b) { return !strcmp(a,b); }

// STDMIX_INCLUDED
#endif
