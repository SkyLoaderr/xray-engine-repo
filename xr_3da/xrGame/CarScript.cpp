#include "stdafx.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "Car.h"
#include "script_space.h"
#include "Actor.h"
using namespace luabind;

void CCar::script_register(lua_State *L)
{
	module(L)
	[
		class_<CCar,CGameObject>("CCar")
			.def(constructor<>())
	];
}