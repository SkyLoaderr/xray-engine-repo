////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_lua_extension.h"
#include "ParticlesObject.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"

using namespace Script;

void LuaLog(LPCSTR caMessage)
{
	Msg			("* [LUA] %s",caMessage);
}

double get_time()
{
	return((double)Device.TimerAsync());
}

CLuaGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(xr_new<CLuaGameObject>(l_tpGameObject));
	else
		return		(0);
}

void Script::vfExportGlobals(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	function		(tpLuaVirtualMachine,	"log",	LuaLog);
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
			.def("set",							(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::set))
			.def("set",							(Fmatrix & (Fmatrix::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(Fmatrix::set))
			.def("identity",					&Fmatrix::identity)
//			.def("mk_xform",					&Fmatrix::mk_xform)
			.def("mul",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, const Fmatrix &))(Fmatrix::mul))
			.def("mul",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, float))(Fmatrix::mul))
			.def("mul",							(Fmatrix & (Fmatrix::*)(float))(Fmatrix::mul))
			.def("mul_43",						&Fmatrix::mul_43)
			.def("mulA",						&Fmatrix::mulA)
			.def("mulB",						&Fmatrix::mulB)
			.def("mulA_43",						&Fmatrix::mulA_43)
			.def("mulB_43",						&Fmatrix::mulB_43)
			.def("div",							(Fmatrix & (Fmatrix::*)(const Fmatrix &, float))(Fmatrix::div))
			.def("div",							(Fmatrix & (Fmatrix::*)(float))(Fmatrix::div))
			.def("invert",						(Fmatrix & (Fmatrix::*)())(Fmatrix::invert))
			.def("invert",						(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::invert))
			.def("transpose",					(Fmatrix & (Fmatrix::*)())(Fmatrix::transpose))
			.def("transpose",					(Fmatrix & (Fmatrix::*)(const Fmatrix &))(Fmatrix::transpose))
			.def("translate",					(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::translate))
			.def("translate",					(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::translate))
			.def("translate_over",				(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::translate_over))
			.def("translate_over",				(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::translate_over))
			.def("translate_add",				&Fmatrix::translate_add)
			.def("scale",						(Fmatrix & (Fmatrix::*)(const Fvector &))(Fmatrix::scale))
			.def("scale",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::scale))
			.def("rotateX",						&Fmatrix::rotateX)
			.def("rotateY",						&Fmatrix::rotateY)
			.def("rotateZ",						&Fmatrix::rotateZ)
			.def("rotation",					(Fmatrix & (Fmatrix::*)(const Fvector &, const Fvector &))(Fmatrix::rotation))
			.def("rotation",					(Fmatrix & (Fmatrix::*)(const Fvector &, float))(Fmatrix::rotation))
//			.def("rotation",					&Fmatrix::rotation)
			.def("mapXYZ",						&Fmatrix::mapXYZ)
			.def("mapXZY",						&Fmatrix::mapXZY)
			.def("mapYXZ",						&Fmatrix::mapYXZ)
			.def("mapYZX",						&Fmatrix::mapYZX)
			.def("mapZXY",						&Fmatrix::mapZXY)
			.def("mapZYX",						&Fmatrix::mapZYX)
			.def("mirrorX",						&Fmatrix::mirrorX)
			.def("mirrorX_over",				&Fmatrix::mirrorX_over)
			.def("mirrorX_add ",				&Fmatrix::mirrorX_add)
			.def("mirrorY",						&Fmatrix::mirrorY)
			.def("mirrorY_over",				&Fmatrix::mirrorY_over)
			.def("mirrorY_add ",				&Fmatrix::mirrorY_add)
			.def("mirrorZ",						&Fmatrix::mirrorZ)
			.def("mirrorZ_over",				&Fmatrix::mirrorZ_over)
			.def("mirrorZ_add ",				&Fmatrix::mirrorZ_add)
			.def("build_projection",			&Fmatrix::build_projection)
			.def("build_projection_HAT",		&Fmatrix::build_projection_HAT)
			.def("build_projection_ortho",		&Fmatrix::build_projection_ortho)
			.def("build_camera",				&Fmatrix::build_camera)
			.def("build_camera_dir",			&Fmatrix::build_camera_dir)
			.def("inertion",					&Fmatrix::inertion)
//			.def("transform_tiny32",			&Fmatrix::transform_tiny32)
//			.def("transform_tiny23",			&Fmatrix::transform_tiny23)
			.def("transform_tiny",				(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform_tiny))
			.def("transform_tiny",				(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform_tiny))
			.def("transform_dir",				(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform_dir))
			.def("transform_dir",				(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform_dir))
			.def("transform",					(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::transform))
			.def("transform",					(void	   (Fmatrix::*)(Fvector &, const Fvector &) const)(Fmatrix::transform))
			.def("setHPB",						&Fmatrix::setHPB)
			.def("setXYZ",						(Fmatrix & (Fmatrix::*)(Fvector &))(Fmatrix::setXYZ))
			.def("setXYZ",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::setXYZ))
			.def("setXYZi",						(Fmatrix & (Fmatrix::*)(Fvector &))(Fmatrix::setXYZi))
			.def("setXYZi",						(Fmatrix & (Fmatrix::*)(float, float, float))(Fmatrix::setXYZi))
			.def("getHPB",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getHPB))
			.def("getHPB",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getHPB))
			.def("getXYZ",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getXYZ))
			.def("getXYZ",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getXYZ))
			.def("getXYZi",						(void	   (Fmatrix::*)(Fvector &) const)(Fmatrix::getXYZi))
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
		def("object",							get_object_by_name)
//		def("get_weather",						Level::get_weather)
	];
}

void Script::vfExportParticles(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CParticlesObject>("particles")
			.def(								constructor<LPCSTR, bool>())
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
			
		class_<CLuaSound>("sound")
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
		class_<CMovementAction>("move")
			.enum_("body")
			[
				value("crouch",					int(StalkerSpace::eBodyStateCrouch)),
				value("standing",				int(StalkerSpace::eBodyStateStand))
			]
			.enum_("move")
			[
				value("walk",					int(StalkerSpace::eMovementTypeWalk)),
				value("run",					int(StalkerSpace::eMovementTypeRun)),
				value("stand",					int(StalkerSpace::eMovementTypeStand))
			]
			.enum_("path")
			[
				value("line",					int(StalkerSpace::ePathTypeStraight)),
				value("dodge",					int(StalkerSpace::ePathTypeDodge)),
				value("criteria",				int(StalkerSpace::ePathTypeCriteria)),
				value("curve",					int(StalkerSpace::ePathTypeStraightDodge)),
				value("curve_criteria",			int(StalkerSpace::ePathTypeDodgeCriteria))
			]
			.def(								constructor<>())
			.def(								constructor<StalkerSpace::EBodyState,StalkerSpace::EMovementType,StalkerSpace::EPathType,CLuaGameObject*>())
			.def(								constructor<StalkerSpace::EBodyState,StalkerSpace::EMovementType,StalkerSpace::EPathType,LPCSTR>())
			.def(								constructor<StalkerSpace::EBodyState,StalkerSpace::EMovementType,StalkerSpace::EPathType,const Fvector &>())
			.def("body",						&CMovementAction::SetBodyState)
			.def("move",						&CMovementAction::SetMovementType)
			.def("path",						&CMovementAction::SetPathType)
			.def("object",						&CMovementAction::SetObjectToGo)
			.def("patrol",						&CMovementAction::SetPatrolPath)
			.def("position",					&CMovementAction::SetPosition),

		class_<CWatchAction>("look")
			.enum_("look")
			[
				value("path_dir",				int(StalkerSpace::eLookTypePathDirection)),
				value("search",					int(StalkerSpace::eLookTypeSearch)),
				value("danger",					int(StalkerSpace::eLookTypeDanger)),
				value("point",					int(StalkerSpace::eLookTypePoint)),
				value("fire_point",				int(StalkerSpace::eLookTypeFirePoint)),
				value("direction",				int(StalkerSpace::eLookTypeDirection))
			]
			.def(								constructor<>())
			.def(								constructor<StalkerSpace::ELookType>())
			.def(								constructor<StalkerSpace::ELookType, const Fvector &>())
			.def(								constructor<StalkerSpace::ELookType, CLuaGameObject*>())
			.def("object",						&CWatchAction::SetWatchObject)		// time
			.def("direct",						&CWatchAction::SetWatchDirection)		// time
			.def("type",						&CWatchAction::SetWatchType),

		class_<CAnimationAction>("anim")
			.enum_("type")
			[
				value("free",					int(StalkerSpace::eMentalStateFree)),
				value("danger",					int(StalkerSpace::eMentalStateDanger)),
				value("asleep",					int(StalkerSpace::eMentalStateAsleep)),
				value("zombied",				int(StalkerSpace::eMentalStateZombied)),
				value("dummy",					int(StalkerSpace::eMentalStateDummy))
			]
			.def(								constructor<>())
			.def(								constructor<LPCSTR>())
			.def(								constructor<StalkerSpace::EMentalState>())
			.def("anim",						&CAnimationAction::SetAnimation)
			.def("type",						&CAnimationAction::SetMentalState),

		class_<CSoundAction>("sound")
			.def(								constructor<>())
			.def(								constructor<LPCSTR>())
			.def(								constructor<LPCSTR, const Fvector &>())
			.def("set",							&CSoundAction::SetSound)
			.def("set",							&CSoundAction::SetPosition),

		class_<CObjectAction>("object")
			.enum_("state")
			[
				value("idle",					int(StalkerSpace::eObjectActionIdle)),
				value("primary_fire",			int(StalkerSpace::eObjectActionPrimaryFire)),
				value("secondary_fire",			int(StalkerSpace::eObjectActionSecondaryFire))
			]
			.def(								constructor<>())
			.def(								constructor<CLuaGameObject*,StalkerSpace::EObjectAction>())
			.def("action",						&CObjectAction::SetObjectAction)
			.def("object",						&CObjectAction::SetObject),
			
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
			.def("set_action",					(void (CEntityAction::*)(const CObjectAction	&tObjectAction))	(CEntityAction::SetAction))
			.def("set_action",					(void (CEntityAction::*)(const CActionCondition &tActionCondition))	(CEntityAction::SetAction))
			.def("move",						&CEntityAction::CheckIfMovementCompleted)
			.def("look",						&CEntityAction::CheckIfWatchCompleted)
			.def("anim",						&CEntityAction::CheckIfAnimationCompleted)
			.def("sound",						&CEntityAction::CheckIfSoundCompleted)
			.def("object",						&CEntityAction::CheckIfObjectCompleted)
			.def("time",						&CEntityAction::CheckIfTimeOver)
			.def("wait",						(bool (CEntityAction::*)() const)(CEntityAction::CheckIfActionCompleted))
	];
}

void Script::vfExportObject(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CLuaGameObject>("object")
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
			.def("classid",						&CLuaGameObject::ClassID)
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
	];
}
