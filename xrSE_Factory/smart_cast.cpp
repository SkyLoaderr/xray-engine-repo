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
#		include "custommonster.h"
#		include "../ispatial.h"
#		include "../fbasicvisual.h"
#		include "inventory_item.h"
#		include "ui/uiwindow.h"
#		include "customzone.h"
#		include "weapon.h"
#	endif
#	include "xrServer_Objects_ALife_Items.h"
#endif

#undef   SMART_CAST_H
#include "smart_cast.h"