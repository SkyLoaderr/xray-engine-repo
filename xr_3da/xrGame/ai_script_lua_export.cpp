////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script_lua_extension.h"
#include "ParticlesObject.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"
#include "..\\cameramanager.h"
#include "..\\effectorpp.h"
#include "luabind\\adopt_policy.hpp"
#include "luabind\\dependency_policy.hpp"
#include "luabind\\return_reference_to_policy.hpp"
#include "luabind\\out_value_policy.hpp"
#include "luabind\\discard_result_policy.hpp"
#include "luabind\\iterator_policy.hpp"

using namespace luabind;
using namespace Script;

void LuaLog(LPCSTR caMessage)
{
	Lua::LuaOut	(Lua::eLuaMessageTypeMessage,"%s",caMessage);
}

double get_time()
{
	return((double)Device.TimerAsync());
}

const CRenderDevice &get_device()
{
	return		(Device);
}

const CCameraManager &get_camera_manager()
{
	return		(Level().Cameras);
}

void vfLuaErrorHandler(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	if (!bfPrintOutput(tpLuaVirtualMachine,"unknown script"));
		vfPrintError(tpLuaVirtualMachine,LUA_ERRRUN);
}

CLuaGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(xr_new<CLuaGameObject>(l_tpGameObject));
	else
		return		(0);
}

int Script::LuaPanic(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	if (!bfPrintOutput(tpLuaVirtualMachine,"unknown script"));
		vfPrintError(tpLuaVirtualMachine,LUA_ERRRUN);
	return(0);
}

void Script::LuaHookCall(CLuaVirtualMachine *tpLuaVirtualMachine, lua_Debug *tpLuaDebug)
{
	lua_getinfo(tpLuaVirtualMachine,"nSlu",tpLuaDebug);
	Lua::ELuaMessageType	l_tLuaMessageType = Lua::eLuaMessageTypeError;
	LPCSTR	S = "";
	switch (tpLuaDebug->event) {
		case LUA_HOOKCALL		: {
			l_tLuaMessageType = Lua::eLuaMessageTypeHookCall;
			break;
		}
		case LUA_HOOKRET		: {
			l_tLuaMessageType = Lua::eLuaMessageTypeHookReturn;
			break;
		}
		case LUA_HOOKLINE		: {
			l_tLuaMessageType = Lua::eLuaMessageTypeHookLine;
			break;
		}
		case LUA_HOOKCOUNT		: {
			l_tLuaMessageType = Lua::eLuaMessageTypeHookCount;
			break;
		}
		case LUA_HOOKTAILRET	: {
			l_tLuaMessageType = Lua::eLuaMessageTypeHookTailReturn;
			break;
		}
		default					: NODEFAULT;
	}

	LuaOut		(l_tLuaMessageType,tpLuaDebug->event == LUA_HOOKLINE ? "%s%s : %s %s %s (current line %d)" : "%s%s : %s %s %s",S,tpLuaDebug->short_src,tpLuaDebug->what,tpLuaDebug->namewhat,tpLuaDebug->name ? tpLuaDebug->name : "",tpLuaDebug->currentline);
}

void Script::vfExportGlobals(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	function		(tpLuaVirtualMachine,	"log",	LuaLog);
#ifdef DEBUG
	function		(tpLuaVirtualMachine,	"flush",FlushLog);
#else
	function		(tpLuaVirtualMachine,	"flush",FlushLogFake);
#endif
}

void Script::vfExportFvector(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
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

void Script::vfExportFmatrix(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
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

void Script::vfExportGame(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine,"game")
	[
		// declarations
		def("time",								get_time)
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
	];
}

void Script::vfExportLevel(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine,"level")
	[
		// declarations
		def("cameras",							get_camera_manager),
		def("object",							get_object_by_name)
//		def("get_weather",						Level::get_weather)
	];

	module(tpLuaVirtualMachine)
	[
		def("device",							get_device)
	];
}

void Script::vfExportDevice(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CRenderDevice>("render_device")
			.def_readonly("width",					&CRenderDevice::dwWidth)
			.def_readonly("height",					&CRenderDevice::dwHeight)
			.def_readonly("time_delta",				&CRenderDevice::dwTimeDelta)
			.def_readonly("time_global",			&CRenderDevice::dwTimeGlobal)
			.def_readonly("cam_pos",				&CRenderDevice::vCameraPosition)
			.def_readonly("cam_dir",				&CRenderDevice::vCameraDirection)
			.def_readonly("cam_top",				&CRenderDevice::vCameraTop)
			.def_readonly("cam_right",				&CRenderDevice::vCameraRight)
			.def_readonly("view",					&CRenderDevice::mView)
			.def_readonly("projection",				&CRenderDevice::mProject)
			.def_readonly("full_transform",			&CRenderDevice::mFullTransform)
			.def_readonly("fov",					&CRenderDevice::fFOV)
			.def_readonly("aspect_ratio",			&CRenderDevice::fASPECT)
	];
}

void Script::vfExportParticles(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CParticlesObject>("particles")
			.def(								constructor<LPCSTR,bool>())
			.def("position",					&CParticlesObject::Position)
			.def("play_at_pos",					&CParticlesObject::play_at_pos)
			.def("stop",						&CParticlesObject::Stop)
	];
}

void Script::vfExportSound(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CSound_params>("sound_params")
			.def_readwrite("position",			&CSound_params::position)
			.def_readwrite("volume",			&CSound_params::volume)
			.def_readwrite("frequency",			&CSound_params::freq)
			.def_readwrite("min_distance",		&CSound_params::min_distance)
			.def_readwrite("max_distance",		&CSound_params::max_distance),
			
		class_<CLuaSound>("sound_object")
			.property("frequency",				&CLuaSound::GetFrequency,	&CLuaSound::SetFrequency)
			.property("min_distance",			&CLuaSound::GetMinDistance,	&CLuaSound::SetMinDistance)
			.property("max_distance",			&CLuaSound::GetMaxDistance,	&CLuaSound::SetMaxDistance)
			.property("volume",					&CLuaSound::GetVolume,		&CLuaSound::SetVolume)
			.def(								constructor<LPCSTR>())
			.def("get_position",				&CLuaSound::GetPosition)
			.def("set_position",				&CLuaSound::SetPosition)
			.def("play",						&CLuaSound::Play)
			.def("play_at_pos",					&CLuaSound::PlayAtPos)
			.def("play_unlimited",				&CLuaSound::PlayUnlimited)
			.def("play_at_pos_unlimited",		&CLuaSound::PlayAtPosUnlimited)
			.def("stop",						&CLuaSound::Stop)
	];
}

void Script::vfExportHit(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
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
			.def_readwrite("bone",				&CLuaHit::m_caBoneName)
			.def_readwrite("impulse",			&CLuaHit::m_fImpulse)
			.def_readwrite("type",				&CLuaHit::m_tHitType)
			.def(								constructor<>())
			.def(								constructor<const CLuaHit *>())
	];
}

void Script::vfExportActions(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CPatrolPathParams>("patrol")
			.enum_("start")
			[
				value("start",					int(CPatrolPathParams::ePatrolPathFirst)),
				value("stop",					int(CPatrolPathParams::ePatrolPathLast)),
				value("nearest",				int(CPatrolPathParams::ePatrolPathNearest)),
				value("dummy",					int(CPatrolPathParams::ePatrolPathDummy))
			]
			.enum_("stop")
			[
				value("stop",					int(CPatrolPathParams::ePatrolPathStop)),
				value("continue",				int(CPatrolPathParams::ePatrolPathContinue)),
				value("dummy",					int(CPatrolPathParams::ePatrolPathInvalid))
			]
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR,const CPatrolPathParams::EPatrolPathStart>())
			.def(								constructor<LPCSTR,const CPatrolPathParams::EPatrolPathStart,const CPatrolPathParams::EPatrolPathStop>())
			.def(								constructor<LPCSTR,const CPatrolPathParams::EPatrolPathStart,const CPatrolPathParams::EPatrolPathStop, bool>()),

		class_<CMovementAction>("move")
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
				value("line",					int(MonsterSpace::ePathTypeStraight)),
				value("dodge",					int(MonsterSpace::ePathTypeDodge)),
				value("criteria",				int(MonsterSpace::ePathTypeCriteria)),
				value("curve",					int(MonsterSpace::ePathTypeStraightDodge)),
				value("curve_criteria",			int(MonsterSpace::ePathTypeDodgeCriteria))
			]
			.enum_("input")
			[
				value("none",					int(CMovementAction::eInputKeyNone)),
				value("fwd",					int(CMovementAction::eInputKeyForward)),
				value("back",					int(CMovementAction::eInputKeyBack)),
				value("left",					int(CMovementAction::eInputKeyLeft)),
				value("right",					int(CMovementAction::eInputKeyRight)),
				value("up",						int(CMovementAction::eInputKeyShiftUp)),
				value("down",					int(CMovementAction::eInputKeyShiftDown)),
				value("break",					int(CMovementAction::eInputKeyBreaks)),
				value("on",						int(CMovementAction::eInputKeyEngineOn)),
				value("off",					int(CMovementAction::eInputKeyEngineOff))
			]
			.def(								constructor<>())
			.def(								constructor<const CMovementAction::EInputKeys>())
			.def(								constructor<const CMovementAction::EInputKeys, float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,CLuaGameObject*>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,CLuaGameObject*,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const CPatrolPathParams &,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const Fvector &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const Fvector &,float>())
			.def(								constructor<const Fvector &,float>())
			.def("body",						&CMovementAction::SetBodyState)
			.def("move",						&CMovementAction::SetMovementType)
			.def("path",						&CMovementAction::SetPathType)
			.def("object",						&CMovementAction::SetObjectToGo)
			.def("patrol",						&CMovementAction::SetPatrolPath)
			.def("position",					&CMovementAction::SetPosition)
			.def("input",						&CMovementAction::SetInputKeys),

		class_<CWatchAction>("look")
			.enum_("look")
			[
				value("path_dir",				int(MonsterSpace::eLookTypePathDirection)),
				value("search",					int(MonsterSpace::eLookTypeSearch)),
				value("danger",					int(MonsterSpace::eLookTypeDanger)),
				value("point",					int(MonsterSpace::eLookTypePoint)),
				value("fire_point",				int(MonsterSpace::eLookTypeFirePoint)),
				value("direction",				int(MonsterSpace::eLookTypeDirection))
			]
			.def(								constructor<>())
			.def(								constructor<MonsterSpace::ELookType>())
			.def(								constructor<MonsterSpace::ELookType, const Fvector &>())
			.def(								constructor<MonsterSpace::ELookType, CLuaGameObject*>())
			.def("object",						&CWatchAction::SetWatchObject)		// time
			.def("direct",						&CWatchAction::SetWatchDirection)		// time
			.def("type",						&CWatchAction::SetWatchType),

		class_<CAnimationAction>("anim")
			.enum_("type")
			[
				value("free",					int(MonsterSpace::eMentalStateFree)),
				value("danger",					int(MonsterSpace::eMentalStateDanger)),
				value("asleep",					int(MonsterSpace::eMentalStateAsleep)),
				value("zombied",				int(MonsterSpace::eMentalStateZombied)),
				value("dummy",					int(MonsterSpace::eMentalStateDummy))
			]
			.def(								constructor<>())
			.def(								constructor<LPCSTR>())
			.def(								constructor<MonsterSpace::EMentalState>())
			.def("anim",						&CAnimationAction::SetAnimation)
			.def("type",						&CAnimationAction::SetMentalState),

		class_<CSoundAction>("sound")
			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,LPCSTR,const Fvector &,const Fvector &,bool>())
			.def(								constructor<LPCSTR,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &>())
			.def(								constructor<LPCSTR,const Fvector &,const Fvector &,bool>())
			.def("set_sound",					&CSoundAction::SetSound)
			.def("set_bone",					&CSoundAction::SetBone)
			.def("set_position",				&CSoundAction::SetPosition)
			.def("set_angles",					&CSoundAction::SetAngles),

		class_<CParticleParams>("particle_params")
			.def(								constructor<>())
			.def(								constructor<const Fvector &>())
			.def(								constructor<const Fvector &,const Fvector &>())
			.def(								constructor<const Fvector &,const Fvector &,const Fvector &>()),

		class_<CParticleAction>("particle")
			.def(								constructor<>())
			.def(								constructor<LPCSTR,LPCSTR>())
			.def(								constructor<LPCSTR,LPCSTR,const CParticleParams &>())
			.def(								constructor<LPCSTR,LPCSTR,const CParticleParams &, bool>())
			.def(								constructor<LPCSTR,const CParticleParams &>())
			.def(								constructor<LPCSTR,const CParticleParams &, bool>())
			.def("set_particle",				&CParticleAction::SetParticle)
			.def("set_bone",					&CParticleAction::SetBone)
			.def("set_position",				&CParticleAction::SetPosition)
			.def("set_angles",					&CParticleAction::SetAngles)
			.def("set_velocity",				&CParticleAction::SetVelocity),

		class_<CObjectAction>("object")
			.enum_("state")
			[
				value("idle",					int(MonsterSpace::eObjectActionIdle)),
				value("fire1",					int(MonsterSpace::eObjectActionPrimaryFire)),
				value("fire2",					int(MonsterSpace::eObjectActionSecondaryFire)),
				value("reload",					int(MonsterSpace::eObjectActionReload)),
				value("activate",				int(MonsterSpace::eObjectActionActivate)),
				value("deactivate",				int(MonsterSpace::eObjectActionDeactivate)),
				value("use",					int(MonsterSpace::eObjectActionUse)),
				value("take",					int(MonsterSpace::eObjectActionTake)),
				value("drop",					int(MonsterSpace::eObjectActionDrop)),
				value("dummy",					int(MonsterSpace::eObjectActionDummy))
			]
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*,MonsterSpace::EObjectAction>())
			.def(								constructor<LPCSTR,MonsterSpace::EObjectAction>())
			.def("action",						&CObjectAction::SetObjectAction)
			.def("object",						(void (CObjectAction::*)(LPCSTR))(CObjectAction::SetObject))
			.def("object",						(void (CObjectAction::*)(CLuaGameObject*))(CObjectAction::SetObject)),
			
		class_<CActionCondition>("cond")
			.enum_("cond")
			[
				value("move_end",				int(CActionCondition::MOVEMENT_FLAG	)),
				value("look_end",				int(CActionCondition::WATCH_FLAG)),
				value("anim_end",				int(CActionCondition::ANIMATION_FLAG)),
				value("sound_end",				int(CActionCondition::SOUND_FLAG)),
				value("object_end",				int(CActionCondition::OBJECT_FLAG)),
				value("time_end",				int(CActionCondition::TIME_FLAG))
			]
			.def(								constructor<>())
			.def(								constructor<u32>())
			.def(								constructor<u32,double>()),

		class_<CEntityAction>("entity_action")
			.def(								constructor<>())
			.def("set_action",					(void (CEntityAction::*)(const CMovementAction	&tMovementAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CWatchAction		&tWatchAction))		(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CAnimationAction &tAnimationAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CSoundAction		&tSoundAction))		(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CParticleAction	&tParticleAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CObjectAction	&tObjectAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CActionCondition &tActionCondition))	(CEntityAction::SetAction))
			.def("move",						&CEntityAction::CheckIfMovementCompleted)
			.def("look",						&CEntityAction::CheckIfWatchCompleted)
			.def("anim",						&CEntityAction::CheckIfAnimationCompleted)
			.def("sound",						&CEntityAction::CheckIfSoundCompleted)
			.def("particle",					&CEntityAction::CheckIfParticleCompleted)
			.def("object",						&CEntityAction::CheckIfObjectCompleted)
			.def("time",						&CEntityAction::CheckIfTimeOver)
			.def("all",							(bool (CEntityAction::*)())(CEntityAction::CheckIfActionCompleted))
	];
}

void Script::vfExportObject(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CLuaGameObject>("game_object")
			.enum_("relation")
			[
				value("friend",					int(ALife::eRelationTypeFriend)),
				value("neutral",				int(ALife::eRelationTypeNeutral)),
				value("enemy",					int(ALife::eRelationTypeEnemy)),
				value("dummy",					int(ALife::eRelationTypeDummy))
			]
			.enum_("rank")
			[
				value("novice",					int(ALife::eStalkerRankNovice)),
				value("experienced",			int(ALife::eStalkerRankExperienced)),
				value("veteran",				int(ALife::eStalkerRankVeteran)),
				value("master",					int(ALife::eStalkerRankMaster)),
				value("dummy",					int(ALife::eStalkerRankDummy))
			]

			.property("visible",				&CLuaGameObject::getVisible,		&CLuaGameObject::setVisible)
			.property("enabled",				&CLuaGameObject::getEnabled,		&CLuaGameObject::setEnabled)
			.property("health",					&CLuaGameObject::GetHealth,			&CLuaGameObject::SetHealth)
			.property("power",					&CLuaGameObject::GetPower,			&CLuaGameObject::SetPower)
			.property("satiety",				&CLuaGameObject::GetSatiety,		&CLuaGameObject::SetSatiety)
			.property("radiation",				&CLuaGameObject::GetRadiation,		&CLuaGameObject::SetRadiation)
			.property("circumspection",			&CLuaGameObject::GetCircumspection,	&CLuaGameObject::SetCircumspection)
			.property("morale",					&CLuaGameObject::GetMorale,			&CLuaGameObject::SetMorale)

			.def(								constructor<LPCSTR>())
			.def(								constructor<const CLuaGameObject *>())
			.def("position",					&CLuaGameObject::Position)
			.def("class_id",					&CLuaGameObject::ClassID)
			.def("id",							&CLuaGameObject::ID)
			.def("section",						&CLuaGameObject::Section)
			.def("name",						&CLuaGameObject::Name)
			.def("parent",						&CLuaGameObject::Parent)
			.def("mass",						&CLuaGameObject::Mass)
			.def("cost",						&CLuaGameObject::Cost)
			.def("death_time",					&CLuaGameObject::DeathTime)
			.def("armor",						&CLuaGameObject::Armor)
			.def("max_health",					&CLuaGameObject::DeathTime)
			.def("accuracy",					&CLuaGameObject::Accuracy)
			.def("alive",						&CLuaGameObject::Alive)
			.def("team",						&CLuaGameObject::Team)
			.def("squad",						&CLuaGameObject::Squad)
			.def("group",						&CLuaGameObject::Group)
			.def("kill",						&CLuaGameObject::Kill)
			.def("hit",							&CLuaGameObject::Hit)
			.def("fov",							&CLuaGameObject::GetFOV)
			.def("range",						&CLuaGameObject::GetRange)
			.def("relation",					&CLuaGameObject::GetRelationType)
			.def("script",						&CLuaGameObject::SetScriptControl)
			.def("get_script",					&CLuaGameObject::GetScriptControl)
			.def("get_script_name",				&CLuaGameObject::GetScriptControlName)
			.def("see",							&CLuaGameObject::CheckObjectVisibility)
			.def("use",							&CLuaGameObject::UseObject)				// time
			.def("rank",						&CLuaGameObject::GetRank)
			.def("get_ammo",					&CLuaGameObject::GetWeaponAmmo)
			.def("command",						&CLuaGameObject::AddAction)
			.def("action",						&CLuaGameObject::GetCurrentAction)
			.def("object_count",				&CLuaGameObject::GetInventoryObjectCount)
			.def("object",						(CLuaGameObject *(CLuaGameObject::*)(LPCSTR))(CLuaGameObject::GetObjectByName))
			.def("object",						(CLuaGameObject *(CLuaGameObject::*)(int))(CLuaGameObject::GetObjectByIndex))
			.def("set_callback",				&CLuaGameObject::SetCallback)
			.def("clear_callback",				&CLuaGameObject::ClearCallback)
	];
}

void Script::vfExportEffector(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<SPPInfo::SDuality>("duality")
			.def_readwrite("h",					&SPPInfo::SDuality::h)
			.def_readwrite("v",					&SPPInfo::SDuality::v)
			.def(								constructor<>()),

		class_<SPPInfo::SNoise::SColor>("color")
			.def_readwrite("r",					&SPPInfo::SNoise::SColor::r)
			.def_readwrite("g",					&SPPInfo::SNoise::SColor::g)
			.def_readwrite("b",					&SPPInfo::SNoise::SColor::b)
			.def_readwrite("a",					&SPPInfo::SNoise::SColor::a)
			.def(								constructor<>()),

		class_<SPPInfo::SNoise>("noise")
			.def_readwrite("intensity",			&SPPInfo::SNoise::intensity)
			.def_readwrite("grain",				&SPPInfo::SNoise::grain)
			.def_readwrite("color",				&SPPInfo::SNoise::color)
			.def(								constructor<>()),

		class_<SPPInfo>("effector_params")
			.def_readwrite("blur",				&SPPInfo::blur)
			.def_readwrite("gray",				&SPPInfo::gray)
			.def_readwrite("dual",				&SPPInfo::duality)
			.def_readwrite("noise",				&SPPInfo::noise)
			.def(								constructor<>()),

		class_<CLuaEffector, CLuaEffectorWrapper>("effector")
			.def_readwrite("info",				&CLuaEffector::m_tInfo)
			.def(								constructor<int,float>())
			.def("start",						&CLuaEffector::Add)
			.def("finish",						&CLuaEffector::Remove)
			.def("process",						&CLuaEffectorWrapper::Process_static)
	];
}
