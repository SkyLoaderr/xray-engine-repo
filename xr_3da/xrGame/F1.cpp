#include "stdafx.h"
#include "f1.h"

CF1::CF1(void) {
	m_weight					= .1f;
	m_slot						= 3;
	m_flags.set					(Fbelt, TRUE);
}

CF1::~CF1(void) {
}

#include "script_space.h"

using namespace luabind;

void CF1::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CF1,CGameObject>("CF1")
			.def(constructor<>())
	];
}
