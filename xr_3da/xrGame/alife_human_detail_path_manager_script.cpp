////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human detail path manager class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_detail_path_manager.h"
#include "script_space.h"

using namespace luabind;

void CALifeHumanDetailPathManager::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CALifeHumanDetailPathManager>("CALifeHumanDetailPathManager")
	];
}
