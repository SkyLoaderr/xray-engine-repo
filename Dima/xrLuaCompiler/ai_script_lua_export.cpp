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
#include "ai_script_snd_info.h"
#include "ai_script_monster_hit_info.h"
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

void LoadScriptModule(LPCSTR script_name)
{
	ai().script_engine().add_file(script_name);
}

void FlushLogs()
{
#ifdef DEBUG
	FlushLog();
	ai().script_engine().flush_log();
#endif
}

void verify_if_thread_is_running()
{
	if (!ai().script_engine().current_thread()) {
		ai().script_engine().script_stack_tracker().print_stack(ai().script_engine().lua());
		VERIFY2		(ai().script_engine().current_thread(),"coroutine.yield() is called outside the LUA thread!");
	}
}

void CScriptEngine::export_globals()
{
	function	(lua(),	"log",							LuaLog);
	function	(lua(),	"flush",						FlushLogs);
	function	(lua(),	"module",						LoadScriptModule);
	function	(lua(),	"verify_if_thread_is_running",	verify_if_thread_is_running);
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
			.def("set",							(Fvector & (Fvector::*)(float,float,float))(Fvector::set),																return_reference_to(_1))
			.def("set",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::set),																return_reference_to(_1))
			.def("add",							(Fvector & (Fvector::*)(float))(Fvector::add),																			return_reference_to(_1))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::add),																return_reference_to(_1))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::add),												return_reference_to(_1))
			.def("add",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::add),															return_reference_to(_1))
			.def("sub",							(Fvector & (Fvector::*)(float))(Fvector::sub),																			return_reference_to(_1))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::sub),																return_reference_to(_1))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::sub),												return_reference_to(_1))
			.def("sub",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::sub),															return_reference_to(_1))
			.def("mul",							(Fvector & (Fvector::*)(float))(Fvector::mul),																			return_reference_to(_1))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::mul),																return_reference_to(_1))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mul),												return_reference_to(_1))
			.def("mul",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mul),															return_reference_to(_1))
			.def("div",							(Fvector & (Fvector::*)(float))(Fvector::div),																			return_reference_to(_1))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::div),																return_reference_to(_1))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::div),												return_reference_to(_1))
			.def("div",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::div),															return_reference_to(_1))
			.def("invert",						(Fvector & (Fvector::*)())(Fvector::invert),																			return_reference_to(_1))
			.def("invert",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::invert),																return_reference_to(_1))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::min),																return_reference_to(_1))
			.def("min",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::min),												return_reference_to(_1))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &))(Fvector::max),																return_reference_to(_1))
			.def("max",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::max),												return_reference_to(_1))
			.def("abs",							&Fvector::abs,																											return_reference_to(_1))
			.def("similar",						&Fvector::similar)
			.def("set_length",					&Fvector::set_length,																									return_reference_to(_1))
			.def("align",						&Fvector::align,																										return_reference_to(_1))
			.def("squeeze",						&Fvector::squeeze,																										return_reference_to(_1))
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::clamp),																return_reference_to(_1))
			.def("clamp",						(Fvector & (Fvector::*)(const Fvector &, const Fvector))(Fvector::clamp),												return_reference_to(_1))
			.def("inertion",					&Fvector::inertion,																										return_reference_to(_1))
			.def("average",						(Fvector & (Fvector::*)(const Fvector &))(Fvector::average),															return_reference_to(_1))
			.def("average",						(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::average),											return_reference_to(_1))
			.def("lerp",						&Fvector::lerp,																											return_reference_to(_1))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::mad),															return_reference_to(_1))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, float))(Fvector::mad),										return_reference_to(_1))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &))(Fvector::mad),												return_reference_to(_1))
			.def("mad",							(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &))(Fvector::mad),								return_reference_to(_1))
			.def("square_magnitude",			&Fvector::square_magnitude)
			.def("magnitude",					&Fvector::magnitude)
			.def("normalize_magnitude",			&Fvector::normalize_magn)
			.def("normalize",					(Fvector & (Fvector::*)())(Fvector::normalize),																			return_reference_to(_1))
			.def("normalize",					(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize),															return_reference_to(_1))
			.def("normalize_safe",				(Fvector & (Fvector::*)())(Fvector::normalize_safe),																	return_reference_to(_1))
			.def("normalize_safe",				(Fvector & (Fvector::*)(const Fvector &))(Fvector::normalize_safe),														return_reference_to(_1))
//			.def("random_dir",					(Fvector & (Fvector::*)())(Fvector::random_dir),																		return_reference_to(_1))
//			.def("random_dir",					(Fvector & (Fvector::*)(const Fvector &, float))(Fvector::random_dir),													return_reference_to(_1))
//			.def("random_point",				(Fvector & (Fvector::*)(const Fvector &))(Fvector::random_point),														return_reference_to(_1))
//			.def("random_point",				(Fvector & (Fvector::*)(float))(Fvector::random_point),																	return_reference_to(_1))
			.def("dotproduct",					&Fvector::dotproduct)
			.def("crossproduct",				&Fvector::crossproduct,																									return_reference_to(_1))
			.def("distance_to_xz",				&Fvector::distance_to_xz)
			.def("distance_to_sqr",				&Fvector::distance_to_sqr)
			.def("distance_to",					&Fvector::distance_to)
			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(Fvector::from_bary),	return_reference_to(_1))
			.def("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fvector::from_bary),		return_reference_to(_1))
			.def("from_bary4",					&Fvector::from_bary4,																									return_reference_to(_1))
			.def("mknormal_non_normalized",		&Fvector::mknormal_non_normalized,																						return_reference_to(_1))
			.def("mknormal",					&Fvector::mknormal,																										return_reference_to(_1))
			.def("setHP",						&Fvector::setHP,																										return_reference_to(_1))
			.def("getHP",						&Fvector::getHP,																																	out_value(_2) + out_value(_3))
			.def("reflect",						&Fvector::reflect,																										return_reference_to(_1))
			.def("slide",						&Fvector::slide,																										return_reference_to(_1))
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
			.def("set",							(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::set),																return_reference_to(_1))
			.def("set",							(Fmatrix & (Fmatrix::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fmatrix::set),				return_reference_to(_1))
			.def("identity",					&Fmatrix::identity,																										return_reference_to(_1))
			.def("mk_xform",					&Fmatrix::mk_xform,																										return_reference_to(_1))
			.def("mul",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, const Fmatrix &))(Fmatrix::mul),												return_reference_to(_1))
			.def("mul",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, float))(Fmatrix::mul),															return_reference_to(_1))
			.def("mul",							(Fmatrix & (Fmatrix::*)(float))(Fmatrix::mul),																			return_reference_to(_1))
			.def("mul_43",						&Fmatrix::mul_43,																										return_reference_to(_1))
			.def("mulA",						&Fmatrix::mulA,																											return_reference_to(_1))
			.def("mulB",						&Fmatrix::mulB,																											return_reference_to(_1))
			.def("mulA_43",						&Fmatrix::mulA_43,																										return_reference_to(_1))
			.def("mulB_43",						&Fmatrix::mulB_43,																										return_reference_to(_1))
			.def("div",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, float))(Fmatrix::div),															return_reference_to(_1))
			.def("div",							(Fmatrix & (Fmatrix::*)(float))(Fmatrix::div),																			return_reference_to(_1))
			.def("invert",						(Fmatrix & (Fmatrix::*)())(Fmatrix::invert),																			return_reference_to(_1))
			.def("invert",						(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::invert),																return_reference_to(_1))
			.def("transpose",					(Fmatrix & (Fmatrix::*)())(Fmatrix::transpose),																			return_reference_to(_1))
			.def("transpose",					(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::transpose),															return_reference_to(_1))
			.def("translate",					(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::translate),															return_reference_to(_1))
			.def("translate",					(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::translate),														return_reference_to(_1))
			.def("translate_over",				(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::translate_over),														return_reference_to(_1))
			.def("translate_over",				(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::translate_over),													return_reference_to(_1))
			.def("translate_add",				&Fmatrix::translate_add,																								return_reference_to(_1))
			.def("scale",						(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::scale),																return_reference_to(_1))
			.def("scale",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::scale),															return_reference_to(_1))
			.def("rotateX",						&Fmatrix::rotateX,																										return_reference_to(_1))
			.def("rotateY",						&Fmatrix::rotateY,																										return_reference_to(_1))
			.def("rotateZ",						&Fmatrix::rotateZ,																										return_reference_to(_1))
			.def("rotation",					(Fmatrix & (Fmatrix::*)(const Fvector &, const Fvector &))(Fmatrix::rotation),											return_reference_to(_1))
			.def("rotation",					(Fmatrix & (Fmatrix::*)(const Fvector &, float))(Fmatrix::rotation),													return_reference_to(_1))
//			.def("rotation",					&Fmatrix::rotation,																										return_reference_to(_1))
			.def("mapXYZ",						&Fmatrix::mapXYZ,																										return_reference_to(_1))
			.def("mapXZY",						&Fmatrix::mapXZY,																										return_reference_to(_1))
			.def("mapYXZ",						&Fmatrix::mapYXZ,																										return_reference_to(_1))
			.def("mapYZX",						&Fmatrix::mapYZX,																										return_reference_to(_1))
			.def("mapZXY",						&Fmatrix::mapZXY,																										return_reference_to(_1))
			.def("mapZYX",						&Fmatrix::mapZYX,																										return_reference_to(_1))
			.def("mirrorX",						&Fmatrix::mirrorX,																										return_reference_to(_1))
			.def("mirrorX_over",				&Fmatrix::mirrorX_over,																									return_reference_to(_1))
			.def("mirrorX_add ",				&Fmatrix::mirrorX_add,																									return_reference_to(_1))
			.def("mirrorY",						&Fmatrix::mirrorY,																										return_reference_to(_1))
			.def("mirrorY_over",				&Fmatrix::mirrorY_over,																									return_reference_to(_1))
			.def("mirrorY_add ",				&Fmatrix::mirrorY_add,																									return_reference_to(_1))
			.def("mirrorZ",						&Fmatrix::mirrorZ,																										return_reference_to(_1))
			.def("mirrorZ_over",				&Fmatrix::mirrorZ_over,																									return_reference_to(_1))
			.def("mirrorZ_add ",				&Fmatrix::mirrorZ_add,																									return_reference_to(_1))
			.def("build_projection",			&Fmatrix::build_projection,																								return_reference_to(_1))
			.def("build_projection_HAT",		&Fmatrix::build_projection_HAT,																							return_reference_to(_1))
			.def("build_projection_ortho",		&Fmatrix::build_projection_ortho,																						return_reference_to(_1))
			.def("build_camera",				&Fmatrix::build_camera,																									return_reference_to(_1))
			.def("build_camera_dir",			&Fmatrix::build_camera_dir,																								return_reference_to(_1))
			.def("inertion",					&Fmatrix::inertion,																										return_reference_to(_1))
//			.def("transform_tiny32",			&Fmatrix::transform_tiny32)
//			.def("transform_tiny23",			&Fmatrix::transform_tiny23)
			.def("transform_tiny",				(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform_tiny),																					out_value(_2))
			.def("transform_tiny",				(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform_tiny),																out_value(_2))
			.def("transform_dir",				(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform_dir),																					out_value(_2))
			.def("transform_dir",				(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform_dir),																	out_value(_2))
			.def("transform",					(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform),																						out_value(_2))
			.def("transform",					(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform),																		out_value(_2))
			.def("setHPB",						&Fmatrix::setHPB,																										return_reference_to(_1))
			.def("setXYZ",						(Fmatrix & (Fmatrix::*)(Fvector &))(Fmatrix::setXYZ),																	return_reference_to(_1)	+	out_value(_2))
			.def("setXYZ",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::setXYZ),															return_reference_to(_1))	 
			.def("setXYZi",						(Fmatrix & (Fmatrix::*)(Fvector &))(Fmatrix::setXYZi),																	return_reference_to(_1) +	out_value(_2))
			.def("setXYZi",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::setXYZi),														return_reference_to(_1))
			.def("getHPB",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getHPB),																							out_value(_2))
			.def("getHPB",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getHPB))
			.def("getXYZ",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getXYZ),																							out_value(_2))
			.def("getXYZ",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getXYZ))
			.def("getXYZi",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getXYZi),																						out_value(_2))
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

struct CMonsterSpace {};

void CScriptEngine::export_monster_info()
{
	module(lua())
	[
		class_<CLuaMonsterHitInfo>("MonsterHitInfo")
			.def_readwrite("who",				&CLuaMonsterHitInfo::who)
			.def_readwrite("direction",			&CLuaMonsterHitInfo::direction)
			.def_readwrite("time",				&CLuaMonsterHitInfo::time),

		class_<CLuaSoundInfo>("SoundInfo")
			.def_readwrite("who",				&CLuaSoundInfo::who)
			.def_readwrite("danger",			&CLuaSoundInfo::dangerous)
			.def_readwrite("position",			&CLuaSoundInfo::position)
			.def_readwrite("power",				&CLuaSoundInfo::power)
			.def_readwrite("time",				&CLuaSoundInfo::time),

		class_<CMonsterSpace>("MonsterSpace")
			.enum_("sounds")
			[
				value("sound_script",			MonsterSpace::eMonsterSoundScript)
			]

			.enum_("head_anim")
			[
				value("head_anim_normal",		MonsterSpace::eHeadAnimNormal),
				value("head_anim_angry",		MonsterSpace::eHeadAnimAngry),
				value("head_anim_glad",			MonsterSpace::eHeadAnimGlad),
				value("head_anim_kind",			MonsterSpace::eHeadAnimKind)
			]
	];
}
