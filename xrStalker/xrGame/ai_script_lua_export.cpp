////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"
#include "ai_script_sound.h"
#include "ai_script_hit.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/adopt_policy.hpp"
//#include "luabind/dependency_policy.hpp"
//#include "luabind/discard_result_policy.hpp"
//#include "luabind/iterator_policy.hpp"
#include "script_engine.h"

using namespace luabind;

extern CLuaGameObject	*tpfGetActor();

void LuaLog(LPCSTR caMessage)
{
	ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeMessage,"%s",caMessage);
}

double get_time()
{
	return((double)Level().GetGameTime());
}

int CScriptEngine::lua_panic(CLuaVirtualMachine *L)
{
	if (!print_output(L,"unknown script"))
		print_error(L,LUA_ERRRUN);
	return(0);
}

void CScriptEngine::lua_hook_call(CLuaVirtualMachine *L, lua_Debug *tpLuaDebug)
{
	return;
//	lua_getinfo(lua(),"nSlu",tpLuaDebug);
//	ScriptStorage::ELuaMessageType	l_tLuaMessageType = ScriptStorage::eLuaMessageTypeError;
//	LPCSTR	S = "";
//	switch (tpLuaDebug->event) {
//		case LUA_HOOKCALL		: {
//			l_tLuaMessageType = ScriptStorage::eLuaMessageTypeHookCall;
//			break;
//		}
//		case LUA_HOOKRET		: {
//			l_tLuaMessageType = ScriptStorage::eLuaMessageTypeHookReturn;
//			break;
//		}
//		case LUA_HOOKLINE		: {
//			l_tLuaMessageType = ScriptStorage::eLuaMessageTypeHookLine;
//			break;
//		}
//		case LUA_HOOKCOUNT		: {
//			l_tLuaMessageType = ScriptStorage::eLuaMessageTypeHookCount;
//			break;
//		}
//		case LUA_HOOKTAILRET	: {
//			l_tLuaMessageType = ScriptStorage::eLuaMessageTypeHookTailReturn;
//			break;
//		}
//		default					: NODEFAULT;
//	}
//
//	ai().script_engine().script_log		(l_tLuaMessageType,tpLuaDebug->event == LUA_HOOKLINE ? "%s%s : %s %s %s (current line %d)" : "%s%s : %s %s %s",S,tpLuaDebug->short_src,tpLuaDebug->what,tpLuaDebug->namewhat,tpLuaDebug->name ? tpLuaDebug->name : "",tpLuaDebug->currentline);
}

#ifndef DEBUG
void FlushLogFake()
{
}
#endif

void LoadScriptModule(LPCSTR script_name)
{
	ai().script_engine().add_file(script_name);
}

void CScriptEngine::export_globals()
{
	function	(lua(),	"log",		LuaLog);

#ifdef DEBUG
	function	(lua(),	"flush",	FlushLog);
#else
	function	(lua(),	"flush",	FlushLogFake);
#endif
	function	(lua(),	"module",	LoadScriptModule);
}

void CScriptEngine::export_fvector()
{
	module(lua())
	[
		class_<Fvector>("vector")
			.def_readwrite("x",					&Fvector::x)
			.def_readwrite("y",					&Fvector::y)
			.def_readwrite("z",					&Fvector::z)
			.def(								constructor<>())
			.def("set",							(Fvector & (Fvector::*)(float,float,float))(Fvector::set),																return_reference_to(self))
			.def("set",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::set),																return_reference_to(self))
			.def("add",							(Fvector & (Fvector::*)(float))(Fvector::add),																			return_reference_to(self))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::add),																return_reference_to(self))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::add),												return_reference_to(self))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::add),															return_reference_to(self))
			.def("sub",							(Fvector & (Fvector::*)(float))(Fvector::sub),																			return_reference_to(self))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::sub),																return_reference_to(self))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::sub),												return_reference_to(self))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::sub),															return_reference_to(self))
			.def("mul",							(Fvector & (Fvector::*)(float))(Fvector::mul),																			return_reference_to(self))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::mul),																return_reference_to(self))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mul),												return_reference_to(self))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mul),															return_reference_to(self))
			.def("div",							(Fvector & (Fvector::*)(float))(Fvector::div),																			return_reference_to(self))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::div),																return_reference_to(self))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::div),												return_reference_to(self))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::div),															return_reference_to(self))
			.def("invert",						(Fvector & (Fvector::*)())(Fvector::invert),																			return_reference_to(self))
			.def("invert",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::invert),																return_reference_to(self))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::min),																return_reference_to(self))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::min),												return_reference_to(self))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::max),																return_reference_to(self))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::max),												return_reference_to(self))
			.def("abs",							&Fvector::abs,																											return_reference_to(self))
			.def("similar",						&Fvector::similar)
			.def("set_length",					&Fvector::set_length,																									return_reference_to(self))
			.def("align",						&Fvector::align,																										return_reference_to(self))
			.def("squeeze",						&Fvector::squeeze,																										return_reference_to(self))
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::clamp),																return_reference_to(self))
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &, const Fvector))(Fvector::clamp),												return_reference_to(self))
			.def("inertion",					&Fvector::inertion,																										return_reference_to(self))
			.def("average",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::average),															return_reference_to(self))
			.def("average",						(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::average),											return_reference_to(self))
			.def("lerp",						&Fvector::lerp,																											return_reference_to(self))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mad),															return_reference_to(self))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, float))(Fvector::mad),										return_reference_to(self))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mad),												return_reference_to(self))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &))(Fvector::mad),								return_reference_to(self))
			.def("square_magnitude",			&Fvector::square_magnitude)
			.def("magnitude",					&Fvector::magnitude)
			.def("magnitude",					&Fvector::normalize_magn)
			.def("normalize",					(Fvector & (Fvector::*)())(Fvector::normalize),																			return_reference_to(self))
			.def("normalize",					(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize),															return_reference_to(self))
			.def("normalize_safe",				(Fvector & (Fvector::*)())(Fvector::normalize_safe),																	return_reference_to(self))
			.def("normalize_safe",				(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize_safe),														return_reference_to(self))
//			.def("random_dir",					(Fvector & (Fvector::*)(CRandom &))(Fvector::random_dir),																/**/return_reference_to(self)/**,	out_value(_1)/**/)
//			.def("random_dir",					(Fvector & (Fvector::*)(const Fvector &, float, CRandom &))(Fvector::random_dir),										/**/return_reference_to(self)/**,	out_value(_3)/**/)
//			.def("random_point",				(Fvector & (Fvector::*)(const Fvector &, CRandom &))(Fvector::random_point),											/**/return_reference_to(self)/**,	out_value(_2)/**/)
//			.def("random_point",				(Fvector & (Fvector::*)(float, CRandom &))(Fvector::random_point),														/**/return_reference_to(self)/**,	out_value(_2)/**/)
			.def("dotproduct",					&Fvector::dotproduct)
			.def("crossproduct",				&Fvector::crossproduct,																									return_reference_to(self))
			.def("distance_to_xz",				&Fvector::distance_to_xz)
			.def("distance_to_sqr",				&Fvector::distance_to_sqr)
			.def("distance_to",					&Fvector::distance_to)
//			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(Fvector::from_bary),	return_reference_to(self))
			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fvector::from_bary),		return_reference_to(self))
//			.def("from_bary4",					&Fvector::from_bary4,																									return_reference_to(self))
			.def("mknormal_non_normalized",		&Fvector::mknormal_non_normalized,																						return_reference_to(self))
			.def("mknormal",					&Fvector::mknormal,																										return_reference_to(self))
			.def("setHP",						&Fvector::setHP,																										return_reference_to(self))
			.def("getHP",						&Fvector::getHP,																																	out_value(_1) + out_value(_2))
			.def("reflect",						&Fvector::reflect,																										return_reference_to(self))
			.def("slide",						&Fvector::slide,																										return_reference_to(self))
			.def("generate_orthonormal_basis",	&Fvector::generate_orthonormal_basis)
	];
}

void CScriptEngine::export_fmatrix()
{
	module(lua())
	[
		class_<Fmatrix>("matrix")
			.def_readwrite("i",					&Fmatrix::i)
			.def_readwrite("_14_",				&Fmatrix::_14_)
			.def_readwrite("j",					&Fmatrix::j)
			.def_readwrite("_24_",				&Fmatrix::_24_)
			.def_readwrite("k",					&Fmatrix::k)
			.def_readwrite("_34_",				&Fmatrix::_34_)
			.def_readwrite("c",					&Fmatrix::c)
			.def_readwrite("_44_",				&Fmatrix::_44_)
			.def(								constructor<>())
			.def("set",							(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::set),																return_reference_to(self))
			.def("set",							(Fmatrix & (Fmatrix::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fmatrix::set),				return_reference_to(self))
			.def("identity",					&Fmatrix::identity,																										return_reference_to(self))
//			.def("mk_xform",					&Fmatrix::mk_xform,																										return_reference_to(self))
			.def("mul",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, const Fmatrix &))(Fmatrix::mul),												return_reference_to(self))
			.def("mul",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, float))(Fmatrix::mul),															return_reference_to(self))
			.def("mul",							(Fmatrix & (Fmatrix::*)(float))(Fmatrix::mul),																			return_reference_to(self))
			.def("mul_43",						&Fmatrix::mul_43,																										return_reference_to(self))
			.def("mulA",						&Fmatrix::mulA,																											return_reference_to(self))
			.def("mulB",						&Fmatrix::mulB,																											return_reference_to(self))
			.def("mulA_43",						&Fmatrix::mulA_43,																										return_reference_to(self))
			.def("mulB_43",						&Fmatrix::mulB_43,																										return_reference_to(self))
			.def("div",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, float))(Fmatrix::div),															return_reference_to(self))
			.def("div",							(Fmatrix & (Fmatrix::*)(float))(Fmatrix::div),																			return_reference_to(self))
			.def("invert",						(Fmatrix & (Fmatrix::*)())(Fmatrix::invert),																			return_reference_to(self))
			.def("invert",						(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::invert),																return_reference_to(self))
			.def("transpose",					(Fmatrix & (Fmatrix::*)())(Fmatrix::transpose),																			return_reference_to(self))
			.def("transpose",					(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::transpose),															return_reference_to(self))
			.def("translate",					(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::translate),															return_reference_to(self))
			.def("translate",					(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::translate),														return_reference_to(self))
			.def("translate_over",				(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::translate_over),														return_reference_to(self))
			.def("translate_over",				(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::translate_over),													return_reference_to(self))
			.def("translate_add",				&Fmatrix::translate_add,																								return_reference_to(self))
			.def("scale",						(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::scale),																return_reference_to(self))
			.def("scale",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::scale),															return_reference_to(self))
			.def("rotateX",						&Fmatrix::rotateX,																										return_reference_to(self))
			.def("rotateY",						&Fmatrix::rotateY,																										return_reference_to(self))
			.def("rotateZ",						&Fmatrix::rotateZ,																										return_reference_to(self))
			.def("rotation",					(Fmatrix & (Fmatrix::*)(const Fvector &, const Fvector &))(Fmatrix::rotation),											return_reference_to(self))
			.def("rotation",					(Fmatrix & (Fmatrix::*)(const Fvector &, float))(Fmatrix::rotation),													return_reference_to(self))
//			.def("rotation",					&Fmatrix::rotation,																										return_reference_to(self))
			.def("mapXYZ",						&Fmatrix::mapXYZ,																										return_reference_to(self))
			.def("mapXZY",						&Fmatrix::mapXZY,																										return_reference_to(self))
			.def("mapYXZ",						&Fmatrix::mapYXZ,																										return_reference_to(self))
			.def("mapYZX",						&Fmatrix::mapYZX,																										return_reference_to(self))
			.def("mapZXY",						&Fmatrix::mapZXY,																										return_reference_to(self))
			.def("mapZYX",						&Fmatrix::mapZYX,																										return_reference_to(self))
			.def("mirrorX",						&Fmatrix::mirrorX,																										return_reference_to(self))
			.def("mirrorX_over",				&Fmatrix::mirrorX_over,																									return_reference_to(self))
			.def("mirrorX_add ",				&Fmatrix::mirrorX_add,																									return_reference_to(self))
			.def("mirrorY",						&Fmatrix::mirrorY,																										return_reference_to(self))
			.def("mirrorY_over",				&Fmatrix::mirrorY_over,																									return_reference_to(self))
			.def("mirrorY_add ",				&Fmatrix::mirrorY_add,																									return_reference_to(self))
			.def("mirrorZ",						&Fmatrix::mirrorZ,																										return_reference_to(self))
			.def("mirrorZ_over",				&Fmatrix::mirrorZ_over,																									return_reference_to(self))
			.def("mirrorZ_add ",				&Fmatrix::mirrorZ_add,																									return_reference_to(self))
			.def("build_projection",			&Fmatrix::build_projection,																								return_reference_to(self))
			.def("build_projection_HAT",		&Fmatrix::build_projection_HAT,																							return_reference_to(self))
			.def("build_projection_ortho",		&Fmatrix::build_projection_ortho,																						return_reference_to(self))
			.def("build_camera",				&Fmatrix::build_camera,																									return_reference_to(self))
			.def("build_camera_dir",			&Fmatrix::build_camera_dir,																								return_reference_to(self))
			.def("inertion",					&Fmatrix::inertion,																										return_reference_to(self))
//			.def("transform_tiny32",			&Fmatrix::transform_tiny32)
//			.def("transform_tiny23",			&Fmatrix::transform_tiny23)
			.def("transform_tiny",				(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform_tiny),																					out_value(_1))
			.def("transform_tiny",				(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform_tiny),																out_value(_1))
			.def("transform_dir",				(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform_dir),																					out_value(_1))
			.def("transform_dir",				(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform_dir),																	out_value(_1))
			.def("transform",					(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform),																						out_value(_1))
			.def("transform",					(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform),																		out_value(_1))
			.def("setHPB",						&Fmatrix::setHPB,																										return_reference_to(self))
			.def("setXYZ",						(Fmatrix & (Fmatrix::*)(Fvector &))(Fmatrix::setXYZ),																	return_reference_to(self) + out_value(_1))
			.def("setXYZ",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::setXYZ),															return_reference_to(self))	 
			.def("setXYZi",						(Fmatrix & (Fmatrix::*)(Fvector &))(Fmatrix::setXYZi),																	return_reference_to(self) + out_value(_1))
			.def("setXYZi",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::setXYZi),														return_reference_to(self))
			.def("getHPB",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getHPB),																							out_value(_1))
			.def("getHPB",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getHPB))
			.def("getXYZ",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getXYZ),																							out_value(_1))
			.def("getXYZ",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getXYZ))
			.def("getXYZi",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getXYZi),																						out_value(_1))
			.def("getXYZi",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getXYZi))
	];
}

void CScriptEngine::export_game()
{
	module(lua(),"game")
	[
		// declarations
		def("time",								get_time)
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
	];
}

void CScriptEngine::export_device()
{
	module(lua())
	[
		class_<CRenderDevice>("render_device")
			.def_readonly("width",					&CRenderDevice::dwWidth)
			.def_readonly("height",					&CRenderDevice::dwHeight)
			.def_readonly("time_delta",				&CRenderDevice::dwTimeDelta)
			.def_readonly("cam_pos",				&CRenderDevice::vCameraPosition)
			.def_readonly("cam_dir",				&CRenderDevice::vCameraDirection)
			.def_readonly("cam_top",				&CRenderDevice::vCameraTop)
			.def_readonly("cam_right",				&CRenderDevice::vCameraRight)
			.def_readonly("view",					&CRenderDevice::mView)
			.def_readonly("projection",				&CRenderDevice::mProject)
			.def_readonly("full_transform",			&CRenderDevice::mFullTransform)
			.def_readonly("fov",					&CRenderDevice::fFOV)
			.def_readonly("aspect_ratio",			&CRenderDevice::fASPECT)
			.def("time_global",						&CRenderDevice::TimerAsync)
	];
}

void CScriptEngine::export_sound()
{
	module(lua())
	[
		class_<CSound_params>("sound_params")
			.def_readwrite("position",			&CSound_params::position)
			.def_readwrite("volume",			&CSound_params::volume)
			.def_readwrite("frequency",			&CSound_params::freq)
			.def_readwrite("min_distance",		&CSound_params::min_distance)
			.def_readwrite("max_distance",		&CSound_params::max_distance),
			
		class_<CLuaSound>("sound_object")
			.enum_("sound_play_type")
			[
				value("looped",					sm_Looped),
				value("s2d",					sm_2D),
				value("s3d",					0)
			]
			.property("frequency",				&CLuaSound::GetFrequency,	&CLuaSound::SetFrequency)
			.property("min_distance",			&CLuaSound::GetMinDistance,	&CLuaSound::SetMinDistance)
			.property("max_distance",			&CLuaSound::GetMaxDistance,	&CLuaSound::SetMaxDistance)
			.property("volume",					&CLuaSound::GetVolume,		&CLuaSound::SetVolume)
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,ESoundTypes>())
			.def("get_position",				&CLuaSound::GetPosition)
			.def("set_position",				&CLuaSound::SetPosition)
			.def("play",						(void (CLuaSound::*)(CLuaGameObject*))(CLuaSound::Play))
			.def("play",						(void (CLuaSound::*)(CLuaGameObject*,float))(CLuaSound::Play))
			.def("play",						(void (CLuaSound::*)(CLuaGameObject*,float,int))(CLuaSound::Play))
			.def("play_at_pos",					(void (CLuaSound::*)(CLuaGameObject*,const Fvector &))(CLuaSound::PlayAtPos))
			.def("play_at_pos",					(void (CLuaSound::*)(CLuaGameObject*,const Fvector &,float))(CLuaSound::PlayAtPos))
			.def("play_at_pos",					(void (CLuaSound::*)(CLuaGameObject*,const Fvector &,float, int))(CLuaSound::PlayAtPos))
			.def("play_clone",					(void (CLuaSound::*)(CLuaGameObject*))(CLuaSound::PlayUnlimited))
			.def("play_clone",					(void (CLuaSound::*)(CLuaGameObject*,float))(CLuaSound::PlayUnlimited))
			.def("play_clone",					(void (CLuaSound::*)(CLuaGameObject*,float,int))(CLuaSound::PlayUnlimited))
			.def("play_at_pos_clone",			(void (CLuaSound::*)(CLuaGameObject*,const Fvector &))(CLuaSound::PlayAtPosUnlimited))
			.def("play_at_pos_clone",			(void (CLuaSound::*)(CLuaGameObject*,const Fvector &,float))(CLuaSound::PlayAtPosUnlimited))
			.def("play_at_pos_clone",			(void (CLuaSound::*)(CLuaGameObject*,const Fvector &,float, int))(CLuaSound::PlayAtPosUnlimited))
			.def("stop",						&CLuaSound::Stop)
			.def("playing",						&CLuaSound::IsPlaying)
			.def("length",						&CLuaSound::Length)
	];
}

void CScriptEngine::export_hit()
{
	module(lua())
	[
		class_<CLuaHit>("hit")
			.enum_("hit_type")
			[
				value("burn",					int(ALife::eHitTypeBurn)),
				value("shock",					int(ALife::eHitTypeShock)),
				value("strike",					int(ALife::eHitTypeStrike)),
				value("wound",					int(ALife::eHitTypeWound)),
				value("radiation",				int(ALife::eHitTypeRadiation)),
				value("telepatic",				int(ALife::eHitTypeTelepatic)),
				value("chemical_burn",			int(ALife::eHitTypeChemicalBurn)),
				value("explosion",				int(ALife::eHitTypeExplosion)),
				value("fire_wound",				int(ALife::eHitTypeFireWound)),
				value("dummy",					int(ALife::eHitTypeMax))
			]
			.def_readwrite("power",				&CLuaHit::m_fPower)
			.def_readwrite("direction",			&CLuaHit::m_tDirection)
			.def_readwrite("draftsman",			&CLuaHit::m_tpDraftsman)
			.def_readwrite("impulse",			&CLuaHit::m_fImpulse)
			.def_readwrite("type",				&CLuaHit::m_tHitType)
			.def(								constructor<>())
			.def(								constructor<const CLuaHit *>())
			.def("bone",						&CLuaHit::set_bone_name)
	];
}

void CScriptEngine::export_actions()
{
	module(lua())
	[
		class_<CPatrolPathParams>("patrol")
			.enum_("start")
			[
				value("start",					int(CPatrolPathManager::ePatrolStartTypeFirst)),
				value("stop",					int(CPatrolPathManager::ePatrolStartTypeLast)),
				value("nearest",				int(CPatrolPathManager::ePatrolStartTypeNearest)),
				value("dummy",					int(CPatrolPathManager::ePatrolStartTypeDummy))
			]
			.enum_("stop")
			[
				value("stop",					int(CPatrolPathManager::ePatrolRouteTypeStop)),
				value("continue",				int(CPatrolPathManager::ePatrolRouteTypeContinue)),
				value("dummy",					int(CPatrolPathManager::ePatrolRouteTypeDummy))
			]
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType>())
			.def(								constructor<LPCSTR,const CPatrolPathManager::EPatrolStartType,const CPatrolPathManager::EPatrolRouteType, bool>())
			.def("count",						&CPatrolPathParams::count)
			.def("point",						(const Fvector &(CPatrolPathParams::*)(u32)				const)	(CPatrolPathParams::point))
			.def("index",						(u32			(CPatrolPathParams::*)(LPCSTR)			const)	(CPatrolPathParams::point))
			.def("nearest",						(u32			(CPatrolPathParams::*)(const Fvector &) const)	(CPatrolPathParams::point))
			.def("flag",						&CPatrolPathParams::flag),

		class_<CScriptMovementAction>("move")
			.enum_("body")
			[
				value("crouch",					int(MonsterSpace::eBodyStateCrouch)),
				value("standing",				int(MonsterSpace::eBodyStateStand))
			]
			.enum_("move")
			[
				value("walk",					int(MonsterSpace::eMovementTypeWalk)),
				value("run",					int(MonsterSpace::eMovementTypeRun)),
				value("stand",					int(MonsterSpace::eMovementTypeStand))
			]
			.enum_("path")
			[
				value("line",					int(CDetailPathManager::eDetailPathTypeSmooth)),
				value("dodge",					int(CDetailPathManager::eDetailPathTypeSmoothDodge)),
				value("criteria",				int(CDetailPathManager::eDetailPathTypeSmoothCriteria)),
				value("curve",					int(CDetailPathManager::eDetailPathTypeSmooth)),
				value("curve_criteria",			int(CDetailPathManager::eDetailPathTypeSmoothCriteria))
			]
			.enum_("input")
			[
				value("none",					int(CScriptMovementAction::eInputKeyNone)),
				value("fwd",					int(CScriptMovementAction::eInputKeyForward)),
				value("back",					int(CScriptMovementAction::eInputKeyBack)),
				value("left",					int(CScriptMovementAction::eInputKeyLeft)),
				value("right",					int(CScriptMovementAction::eInputKeyRight)),
				value("up",						int(CScriptMovementAction::eInputKeyShiftUp)),
				value("down",					int(CScriptMovementAction::eInputKeyShiftDown)),
				value("break",					int(CScriptMovementAction::eInputKeyBreaks)),
				value("on",						int(CScriptMovementAction::eInputKeyEngineOn)),
				value("off",					int(CScriptMovementAction::eInputKeyEngineOff))
			]
			.enum_("monster")
			[

				value("walk_fwd",					int(MonsterSpace::eMA_WalkFwd)),
				value("walk_bkwd",					int(MonsterSpace::eMA_WalkBkwd)),
				value("run",						int(MonsterSpace::eMA_Run)),
				value("drag",						int(MonsterSpace::eMA_Drag)),
				value("jump",						int(MonsterSpace::eMA_Jump)),
				value("steal",						int(MonsterSpace::eMA_Steal))
			]
			.enum_("monster_speed_param")
			[
				value("default",					int(MonsterSpace::eSP_Default)),
				value("force",						int(MonsterSpace::eSP_ForceSpeed))
			]

			.def(								constructor<>())
			.def(								constructor<const CScriptMovementAction::EInputKeys>())
			.def(								constructor<const CScriptMovementAction::EInputKeys, float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,CLuaGameObject*>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,CLuaGameObject*,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const CPatrolPathParams &,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const Fvector &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,CDetailPathManager::EDetailPathType,const Fvector &,float>())
			.def(								constructor<const Fvector &,float>())
			
			// Monsters 
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,const Fvector &,				MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,const CPatrolPathParams &,	MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CLuaGameObject*,				MonsterSpace::EScriptMonsterSpeedParam>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,const Fvector &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EScriptMonsterMoveAction,CLuaGameObject*>())


			.def("body",						&CScriptMovementAction::SetBodyState)
			.def("move",						&CScriptMovementAction::SetMovementType)
			.def("path",						&CScriptMovementAction::SetPathType)
			.def("object",						&CScriptMovementAction::SetObjectToGo)
			.def("patrol",						&CScriptMovementAction::SetPatrolPath)
			.def("position",					&CScriptMovementAction::SetPosition)
			.def("input",						&CScriptMovementAction::SetInputKeys)
			.def("completed",					(bool (CScriptMovementAction::*)())(CScriptMovementAction::completed)),


		class_<CScriptWatchAction>("look")
			.enum_("look")
			[
				value("path_dir",				int(SightManager::eSightTypePathDirection)),
				value("search",					int(SightManager::eSightTypeSearch)),
				value("danger",					int(SightManager::eSightTypeCover)),
				value("point",					int(SightManager::eSightTypePosition)),
				value("fire_point",				int(SightManager::eSightTypeFirePosition)),
				value("cur_dir",				int(SightManager::eSightTypeCurrentDirection)),
				value("direction",				int(SightManager::eSightTypeDirection))
			]
			.def(								constructor<>())
				.def(							constructor<SightManager::ESightType>())
			.def(								constructor<SightManager::ESightType, const Fvector &>())
			.def(								constructor<SightManager::ESightType, CLuaGameObject*>())
			.def(								constructor<SightManager::ESightType, CLuaGameObject*, LPCSTR>())

			// searchlight 
			.def(								constructor<const Fvector &,float,float>())
			.def(								constructor<CLuaGameObject*,float,float>())

			.def("object",						&CScriptWatchAction::SetWatchObject)		// time
			.def("direct",						&CScriptWatchAction::SetWatchDirection)		// time
			.def("type",						&CScriptWatchAction::SetWatchType)
			.def("bone",						&CScriptWatchAction::SetWatchBone)
			.def("completed",					(bool (CScriptWatchAction::*)())(CScriptWatchAction::completed)),

		class_<CScriptAnimationAction>("anim")
			.enum_("type")
			[
				value("free",					int(MonsterSpace::eMentalStateFree)),
				value("danger",					int(MonsterSpace::eMentalStateDanger)),
				value("asleep",					int(MonsterSpace::eMentalStateAsleep)),
				value("zombied",				int(MonsterSpace::eMentalStateZombied)),
				value("dummy",					int(MonsterSpace::eMentalStateDummy))
			]
			.enum_("monster")
			[
				value("stand_idle",				int(MonsterSpace::eAA_StandIdle)),
				value("sit_idle",				int(MonsterSpace::eAA_SitIdle)),
				value("lie_idle",				int(MonsterSpace::eAA_LieIdle)),
				value("eat",					int(MonsterSpace::eAA_Eat)),
				value("sleep",					int(MonsterSpace::eAA_Sleep)),
				value("rest",					int(MonsterSpace::eAA_Rest)),
				value("attack",					int(MonsterSpace::eAA_Attack)),
				value("look_around",			int(MonsterSpace::eAA_LookAround)),
				value("turn",					int(MonsterSpace::eAA_Turn))
			]

			.def(								constructor<>())
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,bool>())
			.def(								constructor<MonsterSpace::EMentalState>())
			
			// Monster specific
			.def(								constructor<MonsterSpace::EScriptMonsterAnimAction, int>())
			
			.def("anim",						&CScriptAnimationAction::SetAnimation)
			.def("type",						&CScriptAnimationAction::SetMentalState)
			.def("completed",					(bool (CScriptAnimationAction::*)())(CScriptAnimationAction::completed)),

		class_<CScriptSoundAction>("sound")
			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<CLuaSound&,LPCSTR,const Fvector &>())
			.def(								constructor<CLuaSound&,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<CLuaSound&,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<CLuaSound&,const Fvector &>())
			.def(								constructor<CLuaSound&,const Fvector &,const Fvector &>())
			.def(								constructor<CLuaSound&,const Fvector &,const Fvector &,bool>())
			.def("set_sound",					(void (CScriptSoundAction::*)(LPCSTR))(CScriptSoundAction::SetSound))
			.def("set_sound",					(void (CScriptSoundAction::*)(const CLuaSound &))(CScriptSoundAction::SetSound))
			.def("set_sound_type",				&CScriptSoundAction::SetSoundType)
			.def("set_bone",					&CScriptSoundAction::SetBone)
			.def("set_position",				&CScriptSoundAction::SetPosition)
			.def("set_angles",					&CScriptSoundAction::SetAngles)
			.def("completed",					(bool (CScriptSoundAction::*)())(CScriptSoundAction::completed)),

		class_<CParticleParams>("particle_params")
			.def(								constructor<>())
			.def(								constructor<const Fvector &>())
			.def(								constructor<const Fvector &,const Fvector &>())
			.def(								constructor<const Fvector &,const Fvector &,const Fvector &>()),

		class_<CScriptParticleAction>("particle")
			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const CParticleParams &>())
			.def(								constructor<LPCSTR,LPCSTR,const CParticleParams &, bool>())
			.def(								constructor<LPCSTR,const CParticleParams &>())
			.def(								constructor<LPCSTR,const CParticleParams &, bool>())
			.def("set_particle",				&CScriptParticleAction::SetParticle)
			.def("set_bone",					&CScriptParticleAction::SetBone)
			.def("set_position",				&CScriptParticleAction::SetPosition)
			.def("set_angles",					&CScriptParticleAction::SetAngles)
			.def("set_velocity",				&CScriptParticleAction::SetVelocity)
			.def("completed",					(bool (CScriptParticleAction::*)())(CScriptParticleAction::completed)),

		class_<CScriptObjectAction>("object")
			.enum_("state")
			[
				value("idle",					int(MonsterSpace::eObjectActionIdle)),
				value("show",					int(MonsterSpace::eObjectActionShow)),		
				value("hide",					int(MonsterSpace::eObjectActionHide)),		
				value("take",					int(MonsterSpace::eObjectActionTake)),		
				value("drop",					int(MonsterSpace::eObjectActionDrop)),		
				value("strap",					int(MonsterSpace::eObjectActionStrapped)),		
				value("aim1",					int(MonsterSpace::eObjectActionAim1)),		
				value("aim2",					int(MonsterSpace::eObjectActionAim2)),		
				value("reload",					int(MonsterSpace::eObjectActionReload1)),	
				value("reload1",				int(MonsterSpace::eObjectActionReload1)),	
				value("reload2",				int(MonsterSpace::eObjectActionReload2)),	
				value("fire1",					int(MonsterSpace::eObjectActionFire1)),		
				value("fire2",					int(MonsterSpace::eObjectActionFire2)),		
				value("switch1",				int(MonsterSpace::eObjectActionSwitch1)),	
				value("switch2",				int(MonsterSpace::eObjectActionSwitch2)),	
				value("activate",				int(MonsterSpace::eObjectActionActivate)),
				value("deactivate",				int(MonsterSpace::eObjectActionDeactivate)),
				value("use",					int(MonsterSpace::eObjectActionUse)),
				value("turn_on",				int(MonsterSpace::eObjectActionTurnOn)),
				value("turn_off",				int(MonsterSpace::eObjectActionTurnOff)),
				value("dummy",					int(MonsterSpace::eObjectActionDummy))
			]
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*,MonsterSpace::EObjectAction>())
			.def(								constructor<CLuaGameObject*,MonsterSpace::EObjectAction,u32>())
			.def(								constructor<MonsterSpace::EObjectAction>())
			.def(								constructor<LPCSTR,MonsterSpace::EObjectAction>())
			.def("action",						&CScriptObjectAction::SetObjectAction)
			.def("object",						(void (CScriptObjectAction::*)(LPCSTR))(CScriptObjectAction::SetObject))
			.def("object",						(void (CScriptObjectAction::*)(CLuaGameObject*))(CScriptObjectAction::SetObject))
			.def("completed",					(bool (CScriptObjectAction::*)())(CScriptObjectAction::completed)),
			
		class_<CActionCondition>("cond")
			.enum_("cond")
			[
				value("move_end",				int(CActionCondition::MOVEMENT_FLAG	)),
				value("look_end",				int(CActionCondition::WATCH_FLAG)),
				value("anim_end",				int(CActionCondition::ANIMATION_FLAG)),
				value("sound_end",				int(CActionCondition::SOUND_FLAG)),
				value("object_end",				int(CActionCondition::OBJECT_FLAG)),
				value("time_end",				int(CActionCondition::TIME_FLAG)),
				value("act_end",				int(CActionCondition::ACT_FLAG))
			]
			.def(								constructor<>())
			.def(								constructor<u32>())
			.def(								constructor<u32,double>()),

		class_<CEntityAction>("entity_action")
			.def(								constructor<>())
			.def(								constructor<const CEntityAction *>())
			.def("set_action",					(void (CEntityAction::*)(const CScriptMovementAction	&tMovementAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CScriptWatchAction		&tWatchAction))		(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CScriptAnimationAction &tAnimationAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CScriptSoundAction		&tSoundAction))		(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CScriptParticleAction	&tParticleAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CScriptObjectAction	&tObjectAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CActionCondition &tActionCondition))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CMonsterAction	&tMonsterAction))	(CEntityAction::SetAction))
			.def("move",						&CEntityAction::CheckIfMovementCompleted)
			.def("look",						&CEntityAction::CheckIfWatchCompleted)
			.def("anim",						&CEntityAction::CheckIfAnimationCompleted)
			.def("sound",						&CEntityAction::CheckIfSoundCompleted)
			.def("particle",					&CEntityAction::CheckIfParticleCompleted)
			.def("object",						&CEntityAction::CheckIfObjectCompleted)
			.def("time",						&CEntityAction::CheckIfTimeOver)
			.def("all",							(bool (CEntityAction::*)())(CEntityAction::CheckIfActionCompleted))
			.def("completed",					(bool (CEntityAction::*)())(CEntityAction::CheckIfActionCompleted)),

		class_<CMonsterAction>("act")
			.enum_("type")
			[
				value("rest",					int(MonsterSpace::eGA_Rest)),
				value("eat",					int(MonsterSpace::eGA_Eat)),
				value("attack",					int(MonsterSpace::eGA_Attack)),
				value("panic",					int(MonsterSpace::eGA_Panic))
			]

			.def(								constructor<>())
			.def(								constructor<MonsterSpace::EScriptMonsterGlobalAction>())
			.def(								constructor<MonsterSpace::EScriptMonsterGlobalAction, CLuaGameObject*>())
	];
}
