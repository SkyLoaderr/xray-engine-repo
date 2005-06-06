#include "stdafx.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
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