////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_lua_extension.h"

using namespace Script;

void Script::vfPrintOutput(CLuaVirtualMachine *tpLuaVirtualMachine, LPCSTR caScriptFileName)
{
	for (int i=0; ; i++)
		if (lua_isstring(tpLuaVirtualMachine,i)) {
			if (!i)
				Msg("* [LUA] Output from %s",caScriptFileName);
			Msg		("* [LUA] %s",lua_tostring(tpLuaVirtualMachine,i));
		}
		else
			return;
}

int Script::ifSuspendThread(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	return lua_yield(tpLuaVirtualMachine, lua_gettop(tpLuaVirtualMachine));
}

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

double get_time()
{
	return((double)Level().GetGameTime());
}

void LuaLog(LPCSTR S)
{
	Msg			("* [LUA] %s",S);
}

void Script::vfExportToLua(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	open			(tpLuaVirtualMachine);

	lua_register	(tpLuaVirtualMachine,	"wait",							ifSuspendThread);
	function		(tpLuaVirtualMachine,	"log",	(void (*) (LPCSTR))		(LuaLog));

	module(tpLuaVirtualMachine)
	[
		class_<Fvector>("Fvector")
			.def_readwrite("x", &Fvector::x)
			.def_readwrite("y", &Fvector::y)
			.def_readwrite("z", &Fvector::z)
			.def(constructor<>())
			.def("set",							(Fvector & (Fvector::*)(float,float,float))(Fvector::set))
			.def("set",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::set))
			.def("add",							(Fvector & (Fvector::*)(float))(Fvector::add))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::add))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::add))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::add))
			.def("sub",							(Fvector & (Fvector::*)(float))(Fvector::sub))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::sub))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::sub))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::sub))
			.def("mul",							(Fvector & (Fvector::*)(float))(Fvector::mul))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::mul))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mul))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mul))
			.def("div",							(Fvector & (Fvector::*)(float))(Fvector::div))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::div))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::div))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::div))
			.def("invert",						(Fvector & (Fvector::*)())(Fvector::invert))
			.def("invert",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::invert))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::min))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::min))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::max))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::max))
			.def("abs",							&Fvector::abs)
			.def("similar",						&Fvector::similar)
			.def("set_length",					&Fvector::set_length)
			.def("align",						&Fvector::align)
			.def("squeeze",						&Fvector::squeeze)
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::clamp))
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &, const Fvector))(Fvector::clamp))
			.def("inertion",					&Fvector::inertion)
			.def("average",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::average))
			.def("average",						(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::average))
			.def("lerp",						&Fvector::lerp)
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mad))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, float))(Fvector::mad))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mad))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &))(Fvector::mad))
			.def("square_magnitude",			&Fvector::square_magnitude)
			.def("magnitude",					&Fvector::magnitude)
			.def("magnitude",					&Fvector::normalize_magn)
			.def("normalize",					(Fvector & (Fvector::*)())(Fvector::normalize))
			.def("normalize",					(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize))
			.def("normalize_safe",				(Fvector & (Fvector::*)())(Fvector::normalize_safe))
			.def("normalize_safe",				(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize_safe))
			.def("random_dir",					(Fvector & (Fvector::*)(CRandom &))(Fvector::random_dir))
			.def("random_dir",					(Fvector & (Fvector::*)(const Fvector &, float, CRandom &))(Fvector::random_dir))
			.def("random_point",				(Fvector & (Fvector::*)(const Fvector &, CRandom &))(Fvector::random_point))
			.def("random_point",				(Fvector & (Fvector::*)(float, CRandom &))(Fvector::random_point))
			.def("dotproduct",					&Fvector::dotproduct)
			.def("crossproduct",				&Fvector::crossproduct)
			.def("distance_to_xz",				&Fvector::distance_to_xz)
			.def("distance_to_sqr",				&Fvector::distance_to_sqr)
			.def("distance_to",					&Fvector::distance_to)
//			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(Fvector::from_bary))
			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fvector::from_bary))
//			.def("from_bary4",					&Fvector::from_bary4)
			.def("mknormal_non_normalized",		&Fvector::mknormal_non_normalized)
			.def("mknormal",					&Fvector::mknormal)
			.def("setHP",						&Fvector::setHP)
			.def("getHP",						&Fvector::getHP)
			.def("reflect",						&Fvector::reflect)
			.def("slide",						&Fvector::slide)
			.def("generate_orthonormal_basis",	&Fvector::generate_orthonormal_basis)
	];
	
	module(tpLuaVirtualMachine,"Game")
	[
		// declarations
		def("get_time",							get_time)
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
		
//		namespace_("Level")
//		[
//			// declarations
//			def("get_weather",					Level::get_weather)
//			def("get_object_by_name",			Level::get_object_by_name),
//		]

	];
	
	string256		S,S1;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);
	LPCSTR			caScriptString = l_tpIniFile->r_string("common","script");

	u32				N = _GetItemCount(caScriptString);
	string16		I;
	for (u32 i=0; i<N; i++) {
		FS.update_path(S,"$game_scripts$",strconcat(S1,_GetItem(caScriptString,i,I),".script"));
		R_ASSERT3	(FS.exist(S),"Script file not found!",S);
		IReader		*F = FS.r_open(S);
		R_ASSERT	(F);
		strconcat	(S1,"@",S);		
		int			l_iErrorCode = lua_dobuffer(tpLuaVirtualMachine,static_cast<LPCSTR>(F->pointer()),F->length(),S);
		Msg			("* Loading common script %s",S);
		vfPrintOutput(tpLuaVirtualMachine,S);
		if (l_iErrorCode)
			vfPrintError(tpLuaVirtualMachine,l_iErrorCode);
		FS.r_close	(F);
	}
	xr_delete		(l_tpIniFile);
}