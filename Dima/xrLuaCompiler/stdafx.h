////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.h
//	Created 	: 14.10.2003
//  Modified 	: 14.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header
////////////////////////////////////////////////////////////////////////////

#pragma once

#define _XRAY_LUA_COMPILER_

#pragma warning(disable:4995)
#include "..\stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#include "clsid_game.h"

// #pragma warning(disable:4201)
#include "..\igame_persistent.h"
#include "..\igame_level.h"
#include "..\xr_object.h"
#include "..\sound.h"
#include "..\xr_input.h"
#include "..\x_ray.h"
#include "..\std_classes.h"
#include "..\GameFont.h"

#include "..\NET_Server.h"
#include "..\NET_Client.h"

#include "..\render.h"
#include "..\fmesh.h"
#include "..\BodyInstance.h"


#include "ode_include.h"

#include "xr_level_controller.h"
#include "Level.h"
#include "GameMtlLib.h"
