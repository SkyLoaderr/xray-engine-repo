#include "stdafx.h"
#include "cl_collector.h"
#include "build.h"

RAPID::Model	RCAST_Model;

void CBuild::BuildRapid()
{
	float	p_total = 0;
	float	p_cost  = 1.f/(g_faces.size());

	Status("Converting faces...");

	RAPID::CollectorPacked	CL(scene_bb);
	for (vecFaceIt it=g_faces.begin(); it!=g_faces.end(); it++)
	{
		Face*	F = (*it);
		CL.add_face(
			F->v[0]->P,F->v[1]->P,F->v[2]->P,
			RAPID::edge_open,RAPID::edge_open,RAPID::edge_open,
			0,0,DWORD(F)
			);
		Progress(p_total+=p_cost);
	}
	Status("Building OBB tree..");
	RCAST_Model.BuildModel(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());
}
