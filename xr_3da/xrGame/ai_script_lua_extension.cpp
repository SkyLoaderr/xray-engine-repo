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

void Script::vfLoadStandardScripts(CLuaVirtualMachine *tpLuaVirtualMachine)
{
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
		int			l_iErrorCode = lua_dobuffer(tpLuaVirtualMachine,static_cast<LPCSTR>(F->pointer()),F->length(),S1);
		Msg			("* Loading common script %s",S);
		vfPrintOutput(tpLuaVirtualMachine,S);
		if (l_iErrorCode)
			vfPrintError(tpLuaVirtualMachine,l_iErrorCode);
		FS.r_close	(F);
	}
	xr_delete		(l_tpIniFile);
}

void Script::vfExportToLua(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	open			(tpLuaVirtualMachine);

	function		(tpLuaVirtualMachine,	"log",	LuaLog);

	module(tpLuaVirtualMachine,"Game")
	[
		// declarations
		def("get_time",							get_time),
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
		
		namespace_("Level")
		[
//			// declarations
			def("get_object_by_name",			get_object_by_name)
//			def("get_weather",					Level::get_weather)
		]

	];

	module(tpLuaVirtualMachine)
	[
		class_<Fvector>("Fvector")
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
			.def("generate_orthonormal_basis",	&Fvector::generate_orthonormal_basis),


		class_<Fmatrix>("Fmatrix")
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
			.def("getXYZi",						(void	   (Fmatrix::*)(float &, float &, float &) const)(Fmatrix::getXYZi)),

		class_<CParticlesObject>("CParticleSystem")
			.def(								constructor<LPCSTR, bool>())
			.def("Position",					&CParticlesObject::Position)
			.def("PlayAtPos",					&CParticlesObject::play_at_pos)
			.def("Stop",						&CParticlesObject::Stop),

		class_<CSound_params>("CSoundParams")
			.def_readwrite("Position",			&CSound_params::position)
			.def_readwrite("Volume",			&CSound_params::volume)
			.def_readwrite("Frequency",			&CSound_params::freq)
			.def_readwrite("MinDistance",		&CSound_params::min_distance)
			.def_readwrite("MaxDistance",		&CSound_params::max_distance),
			
		class_<CLuaSound>("CSound")
			.property("Frequency",				&CLuaSound::GetFrequency,	&CLuaSound::SetFrequency)
			.property("MinDistance",			&CLuaSound::GetMinDistance,	&CLuaSound::SetMinDistance)
			.property("MaxDistance",			&CLuaSound::GetMaxDistance,	&CLuaSound::SetMaxDistance)
			.property("Volume",					&CLuaSound::GetVolume,		&CLuaSound::SetVolume)
			.def(								constructor<LPCSTR>())
			.def("GetPosition",					&CLuaSound::GetPosition)
			.def("SetPosition",					&CLuaSound::SetPosition)
			.def("Play",						&CLuaSound::Play)
			.def("PlayAtPos",					&CLuaSound::PlayAtPos)
			.def("PlayUnlimited",				&CLuaSound::PlayUnlimited)
			.def("PlayAtPosUnlimited",			&CLuaSound::PlayAtPosUnlimited)
			.def("Stop",						&CLuaSound::Stop),

		class_<CLuaHit>("CHit")
			.enum_("EHitType")
			[
				value("eHitTypeBurn",			int(ALife::eHitTypeBurn)),
				value("eHitTypeShock",			int(ALife::eHitTypeShock)),
				value("eHitTypeStrike",			int(ALife::eHitTypeStrike)),
				value("eHitTypeWound",			int(ALife::eHitTypeWound)),
				value("eHitTypeRadiation",		int(ALife::eHitTypeRadiation)),
				value("eHitTypeTelepatic",		int(ALife::eHitTypeTelepatic)),
				value("eHitTypeChemicalBurn",	int(ALife::eHitTypeChemicalBurn)),
				value("eHitTypeExplosion",		int(ALife::eHitTypeExplosion)),
				value("eHitTypeFireWound",		int(ALife::eHitTypeFireWound))
			]
			.def_readwrite("Power",				&CLuaHit::m_fPower)
			.def_readwrite("Direction",			&CLuaHit::m_tDirection)
			.def_readwrite("Draftsman",			&CLuaHit::m_tpDraftsman)
			.def_readwrite("BoneName",			&CLuaHit::m_caBoneName)
			.def_readwrite("Impulse",			&CLuaHit::m_fImpulse)
			.def_readwrite("HitType",			&CLuaHit::m_tHitType)
			.def(								constructor<>())
			.def(								constructor<const CLuaHit *>()),

		class_<CLuaGameObject>("CGameObject")
			.enum_("ERelationType")
			[
				value("eRelationTypeFriend",	int(ALife::eRelationTypeFriend)),
				value("eRelationTypeNeutral",	int(ALife::eRelationTypeNeutral)),
				value("eRelationTypeEnemy",		int(ALife::eRelationTypeEnemy)),
				value("eRelationTypeDummy",		int(ALife::eRelationTypeDummy))
			]
			.property("Visible",				&CLuaGameObject::getVisible,		&CLuaGameObject::setVisible)
			.property("Enabled",				&CLuaGameObject::getEnabled,		&CLuaGameObject::setEnabled)
			.property("Health",					&CLuaGameObject::GetHealth,			&CLuaGameObject::SetHealth)
			.property("Power",					&CLuaGameObject::GetPower,			&CLuaGameObject::SetPower)
			.property("Satiety",				&CLuaGameObject::GetSatiety,		&CLuaGameObject::SetSatiety)
			.property("Radiation",				&CLuaGameObject::GetRadiation,		&CLuaGameObject::SetRadiation)
			.property("Circumspection",			&CLuaGameObject::GetCircumspection,	&CLuaGameObject::SetCircumspection)
			.property("Morale",					&CLuaGameObject::GetMorale,			&CLuaGameObject::SetMorale)
			.def(								constructor<LPCSTR>())
			.def(								constructor<const CLuaGameObject *>())
			.def("Position",					&CLuaGameObject::Position)
			.def("ClassID",						&CLuaGameObject::ClassID)
			.def("ID",							&CLuaGameObject::ID)
			.def("Section",						&CLuaGameObject::Section)
			.def("Name",						&CLuaGameObject::Name)
			.def("Parent",						&CLuaGameObject::Parent)
			.def("Mass",						&CLuaGameObject::Mass)
			.def("Cost",						&CLuaGameObject::Cost)
			.def("DeathTime",					&CLuaGameObject::DeathTime)
			.def("Armor",						&CLuaGameObject::Armor)
			.def("MaxHealth",					&CLuaGameObject::DeathTime)
			.def("Accuracy",					&CLuaGameObject::Accuracy)
			.def("Alive",						&CLuaGameObject::Alive)
			.def("Team",						&CLuaGameObject::Team)
			.def("Squad",						&CLuaGameObject::Squad)
			.def("Group",						&CLuaGameObject::Group)
			.def("Kill",						&CLuaGameObject::Kill)
			.def("Hit",							&CLuaGameObject::Hit)
			.def("GetFOV",						&CLuaGameObject::GetFOV)
			.def("GetRange",					&CLuaGameObject::GetRange)
			.def("GetRelationType",				&CLuaGameObject::GetRelationType)
	];

	vfLoadStandardScripts(tpLuaVirtualMachine);
}