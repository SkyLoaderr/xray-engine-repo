////////////////////////////////////////////////////////////////////////////
//	Module 		: particles_object_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Particles object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "particlesobject.h"
#include "script_space.h"

using namespace luabind;

void CParticlesObject::script_register(lua_State *L)
{
/*	module(L)
	[
		class_<CParticlesObject>("particles")
			.def(								constructor<LPCSTR,bool>())
			.def("position",					&CParticlesObject::Position)
			.def("play_at_pos",					&CParticlesObject::play_at_pos)
			.def("stop",						&CParticlesObject::Stop)
	];*/
}