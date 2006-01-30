///////////////////////////////////////////////////////////////
// StalkerOutfit.cpp
// StalkerOutfit - �������� ������ ��������
///////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "StalkerOutfit.h"

CStalkerOutfit::CStalkerOutfit()
{
}

CStalkerOutfit::~CStalkerOutfit() 
{
}

#include "script_space.h"

using namespace luabind;

void CStalkerOutfit::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CStalkerOutfit,CGameObject>("CStalkerOutfit")
			.def(constructor<>())
	];
}
