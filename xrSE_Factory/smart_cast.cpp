////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.cpp
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef PURE_DYNAMIC_CAST
#	ifdef XRGAME_EXPORTS
#		include	"gameobject.h"
#		include "actor.h"
#	endif
#endif

#undef   SMART_CAST_H
#include "smart_cast.h"