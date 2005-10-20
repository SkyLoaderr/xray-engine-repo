#include "stdafx.h"
#include "UIMapInfo.h"
#include "../script_space.h"

using namespace luabind;

void CUIMapInfo::script_register(lua_State *L){
	module(L)
	[
		class_<CUIMapInfo, CUIWindow>("CUIMapInfo")
		.def(				constructor<>())
		.def("Init",		&CUIMapInfo::Init)
		.def("InitMap",		&CUIMapInfo::InitMap)
	];

}