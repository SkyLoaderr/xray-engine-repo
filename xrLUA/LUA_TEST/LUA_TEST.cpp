// LUA_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "xrCore.h"
#pragma comment(lib,"x:\\xrCore.lib")

// Lua
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#pragma comment(lib,"x:\\xrLUA.lib")

// Lua-bind
#pragma warning(disable:4244)
#include <luabind\\luabind.hpp>
#pragma warning(default:4244)

using namespace luabind;

void Log(LPCSTR S)
{
	printf("%s",S);
}

namespace Game {
	enum EWeatherType {
		eWeatherTypeSun = u32(0),
		eWeatherTypeClouds,
		eWeatherTypeRain,
		eWeatherTypeDummy = u32(-1),
	};

	u32 get_time()
	{
		static int	g_iCurrentTime = 0;
		return		(g_iCurrentTime++);
	}
	
	EWeatherType get_weather()
	{
		return(eWeatherTypeSun);
	}

	u32 get_psi_plant_deactivation_time()
	{
		static int	g_iCurrentTime = 0;
		return		(g_iCurrentTime++);
	}
};

//class _vector {
//public:
//
//};
//
//class CItemObject {
//public:
//	Active();
//	Visible(); // only for NPC
//	Condition();
//	get_parent();
//	get_mass();
//	get_cost();
//	position();
//};
//
//class CAliveObject {
//public:
//	rank();
//	health();
//	activeweapon();
//	equipment();
//	asleep();
//	zombied();
//	checkifobjectvisible();
//
//};
//
int ifSuspendThread(lua_State *tpLuaVirtualMachine)
{
	return lua_yield(tpLuaVirtualMachine, lua_gettop(tpLuaVirtualMachine));
}

void export2lua		(lua_State *tpLuaVirtualMachine)
{
	open			(tpLuaVirtualMachine);
	
	module(tpLuaVirtualMachine,"Game")
	[
		// declarations
		def("get_time",							Game::get_time)
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
//		
//		namespace_("Level")
//		[
//			// declarations
//			def("get_weather",					Level::get_weather)
//			def("get_object_by_name",			Level::get_object_by_name),
//		]
//
	];

//	module(tpLuaVirtualMachine)
//	[
//		class_<CScriptObject>("CGameObject")
//			.def(),
//	];
//	
//
	module(tpLuaVirtualMachine)
	[
		class_<Fvector >("Fvector")
		.def_readwrite("x", &Fvector::x)
		.def_readwrite("y", &Fvector::y)
		.def_readwrite("z", &Fvector::z)
		.def(constructor<>())
		.def("set",							(void (Fvector::*)(float,float,float))(Fvector::set))
		.def("set",							(void (Fvector::*)(const Fvector &))(Fvector::set))
		.def("add",							(void (Fvector::*)(float))(Fvector::add))
		.def("add",							(void (Fvector::*)(const Fvector &))(Fvector::add))
		.def("add",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::add))
		.def("add",							(void (Fvector::*)(const Fvector &, float))(Fvector::add))
		.def("sub",							(void (Fvector::*)(float))(Fvector::sub))
		.def("sub",							(void (Fvector::*)(const Fvector &))(Fvector::sub))
		.def("sub",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::sub))
		.def("sub",							(void (Fvector::*)(const Fvector &, float))(Fvector::sub))
		.def("mul",							(void (Fvector::*)(float))(Fvector::mul))
		.def("mul",							(void (Fvector::*)(const Fvector &))(Fvector::mul))
		.def("mul",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mul))
		.def("mul",							(void (Fvector::*)(const Fvector &, float))(Fvector::mul))
		.def("div",							(void (Fvector::*)(float))(Fvector::div))
		.def("div",							(void (Fvector::*)(const Fvector &))(Fvector::div))
		.def("div",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::div))
		.def("div",							(void (Fvector::*)(const Fvector &, float))(Fvector::div))
		.def("invert",						(void (Fvector::*)())(Fvector::invert))
		.def("invert",						(void (Fvector::*)(const Fvector &))(Fvector::invert))
		.def("min",							(void (Fvector::*)(const Fvector &))(Fvector::min))
		.def("min",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::min))
		.def("max",							(void (Fvector::*)(const Fvector &))(Fvector::max))
		.def("max",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::max))
		.def("abs",							&Fvector::abs)
		.def("similar",						&Fvector::similar)
		.def("set_length",					&Fvector::set_length)
		.def("align",						&Fvector::align)
		.def("squeeze",						&Fvector::squeeze)
		.def("clamp",						(void (Fvector::*)(const Fvector &))(Fvector::clamp))
		.def("clamp",						(void (Fvector::*)(const Fvector &, const Fvector))(Fvector::clamp))
		.def("inertion",					&Fvector::inertion)
		.def("average",						(void (Fvector::*)(const Fvector &))(Fvector::average))
		.def("average",						(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::average))
		.def("lerp",						&Fvector::lerp)
		.def("mad",							(void (Fvector::*)(const Fvector &, float))(Fvector::mad))
		.def("mad",							(void (Fvector::*)(const Fvector &, const Fvector &, float))(Fvector::mad))
		.def("mad",							(void (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mad))
		.def("mad",							(void (Fvector::*)(const Fvector &, const Fvector &, const Fvector &))(Fvector::mad))
		.def("square_magnitude",			&Fvector::square_magnitude)
		.def("magnitude",					&Fvector::magnitude)
		.def("normalize",					(float (Fvector::*)())(Fvector::normalize))
		.def("normalize",					(void (Fvector::*)(const Fvector &))(Fvector::normalize))
		.def("normalize_safe",				(void (Fvector::*)())(Fvector::normalize_safe))
		.def("normalize_safe",				(void (Fvector::*)(const Fvector &))(Fvector::normalize_safe))
		.def("random_dir",					(void (Fvector::*)(CRandom &))(Fvector::random_dir))
		.def("random_dir",					(void (Fvector::*)(const Fvector &, float, CRandom &))(Fvector::random_dir))
		.def("random_point",				(void (Fvector::*)(const Fvector &, CRandom &))(Fvector::random_point))
		.def("random_point",				(void (Fvector::*)(float, CRandom &))(Fvector::random_point))
		.def("dotproduct",					&Fvector::dotproduct)
		.def("crossproduct",				&Fvector::crossproduct)
		.def("distance_to_xz",				&Fvector::distance_to_xz)
		.def("distance_to_sqr",				&Fvector::distance_to_sqr)
		.def("distance_to",					&Fvector::distance_to)
		//			.def("from_bary",					(void (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(Fvector::from_bary))
		.def("from_bary",					(void (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fvector::from_bary))
		//			.def("from_bary4",					&Fvector::from_bary4)
		.def("mknormal_non_normalized",		&Fvector::mknormal_non_normalized)
		.def("mknormal",					&Fvector::mknormal)
		.def("setHP",						&Fvector::setHP)
		.def("getHP",						&Fvector::getHP)
		.def("reflect",						&Fvector::reflect)
		.def("slide",						&Fvector::slide)
		.def("generate_orthonormal_basis",	&Fvector::generate_orthonormal_basis)
	];

	lua_register	(tpLuaVirtualMachine,	"wait",				ifSuspendThread);
	function		(tpLuaVirtualMachine,"log",(void (*)(LPCSTR))(Log));
}

// main
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	lua_State*	luaVM	= lua_open();
	if (NULL == luaVM)	{ printf("Error Initializing lua\n"); return -1; }

	// initialize lua standard library functions 
	luaopen_base	(luaVM); 
	luaopen_table	(luaVM);
	luaopen_io		(luaVM);
	luaopen_string	(luaVM);
	luaopen_math	(luaVM);
	luaopen_debug	(luaVM);

	// export fvector
	export2lua		(luaVM);

	// do some stuff
	lua_dofile		(luaVM, "x:\\test1.lua");

	// close lua
	lua_close		(luaVM);
	return 0;
}
