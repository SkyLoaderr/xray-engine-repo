////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#ifndef SMART_CAST_H
#define SMART_CAST_H

#define  TL_FAST_COMPILATION
#undef   STATIC_CHECK
#include <typelist.h>

//#define PURE_DYNAMIC_CAST
#define PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK

#ifdef PURE_DYNAMIC_CAST
#	define smart_cast dynamic_cast
#else
#	ifndef DECLARE_SPECIALIZATION
#		include "smart_cast_impl0.h"
#	else
#		include "smart_cast_impl2.h"
#		define DO_NOT_DECLARE_TYPE_LIST
#	endif

#	ifndef DO_NOT_DECLARE_TYPE_LIST
#		include "smart_cast_impl1.h"
#	endif
#endif

#endif //SMART_CAST_H