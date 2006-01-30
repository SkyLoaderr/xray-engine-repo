#include "stdafx.h"
#include "rgd5.h"

CRGD5::CRGD5(void)
{
	m_flags.set				(Fbelt, TRUE);
	m_weight				= .1f;
	m_slot					= GRENADE_SLOT;
}

CRGD5::~CRGD5(void)
{
}

#include "script_space.h"

using namespace luabind;

void CRGD5::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CRGD5,CGameObject>("CRGD5")
			.def(constructor<>())
	];
}
