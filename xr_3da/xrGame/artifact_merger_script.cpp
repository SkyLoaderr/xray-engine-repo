////////////////////////////////////////////////////////////////////////////
//	Module 		: artifact_merger_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Artifact merger script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "artifactmerger.h"
#include "script_space.h"

using namespace luabind;

void CArtifactMerger::script_register(lua_State *L)
{
	module(L)
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

			.def("spawn_mercury_ball",		&CArtifactMerger::SpawnMercuryBall)
			.def("spawn_gravi",				&CArtifactMerger::SpawnGraviArtifact)
			.def("spawn_black_drops",		&CArtifactMerger::SpawnBlackDrops)
			.def("spawn_needles",			&CArtifactMerger::SpawnNeedles)
	];
}
