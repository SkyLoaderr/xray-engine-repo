////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_export_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_space.h"
#include "ai_script_lua_extension.h"
#include "../cameramanager.h"
#include "../effectorpp.h"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "ParticlesObject.h"
#include "ArtifactMerger.h"
#include "actor.h"
#include "ai_script_classes.h"

using namespace luabind;
using namespace Script;


const CRenderDevice &get_device()
{
	return		(Device);
}

CLuaGameObject *tpfGetActor()
{
	CActor *l_tpActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return(xr_new<CLuaGameObject>(dynamic_cast<CGameObject*>(l_tpActor)));
	else
		return(0);
}

const CCameraManager &get_camera_manager()
{
	return		(Level().Cameras);
}

CLuaGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(xr_new<CLuaGameObject>(l_tpGameObject));
	else
		return		(0);
}

void Script::vfExportArtifactMerger(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine)
	[
		class_<CArtifactMerger>("artifact_merger")
		.def("get_mercury_ball_num",	&CArtifactMerger::GetMercuryBallNum)
		.def("get_gravi_num",			&CArtifactMerger::GetGraviArtifactNum)
		.def("get_black_drops_num",		&CArtifactMerger::GetBlackDropsNum)
		.def("get_needles_num",			&CArtifactMerger::GetNeedlesNum)

		.def("destroy_mercury_ball",	&CArtifactMerger::DestroyMercuryBall)
		.def("destroy_gravi",			&CArtifactMerger::DestroyGraviArtifact)
		.def("destroy_black_drops",		&CArtifactMerger::DestroyBlackDrops)
		.def("destroy_needles",			&CArtifactMerger::DestroyNeedles)

		.def("spawn_mercury_ball",	&CArtifactMerger::SpawnMercuryBall)
		.def("spawn_gravi",			&CArtifactMerger::SpawnGraviArtifact)
		.def("spawn_black_drops",	&CArtifactMerger::SpawnBlackDrops)
		.def("spawn_needles",		&CArtifactMerger::SpawnNeedles)
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

void Script::vfExportLevel(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	module(tpLuaVirtualMachine,"level")
	[
		// declarations
		def("cameras",							get_camera_manager),
		def("object",							get_object_by_name),
		def("actor",							tpfGetActor),
		def("set_artifact_merge",				&CArtifactMerger::SetArtifactMergeFunctor)
//		def("get_weather",						Level::get_weather)
	];

	module(tpLuaVirtualMachine)
	[
		def("device",							get_device)
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