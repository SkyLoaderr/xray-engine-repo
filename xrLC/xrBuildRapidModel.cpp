#include "stdafx.h"
#include "cl_collector.h"
#include "build.h"

CDB::MODEL*	RCAST_Model	= 0;


IC bool				FaceEqual(Face& F1, Face& F2)
{
	// Test for 6 variations
	if ((F1.v[0]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[0]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	return false;
}

void CBuild::BuildRapid()
{
	float	p_total			= 0;
	float	p_cost			= 1.f/(g_faces.size());

	Status			("Converting faces...");
	for				(DWORD fit=0; fit<g_faces.size(); fit++)	g_faces[fit]->bProcessed = FALSE;

	vector<Face*>	adjacent;	adjacent.reserve(6*2*3);
	CDB::CollectorPacked	CL	(scene_bb,g_vertices.size(),g_faces.size());
	for (vecFaceIt it=g_faces.begin(); it!=g_faces.end(); it++)
	{
		Face*	F	= (*it);
		Progress	(float(it-g_faces.begin())/float(g_faces.size()));

		// Collect
		adjacent.clear	();
		for (int vit=0; vit<3; vit++)
		{
			Vertex* V = F->v[vit];
			for (DWORD adj=0; adj<V->adjacent.size(); adj++)
			{
				adjacent.push_back(V->adjacent[adj]);
			}
		}

		// Unique
		std::sort		(adjacent.begin(),adjacent.end());
		adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());
		BOOL			bAlready	= FALSE;
		for (DWORD ait=0; ait<adjacent.size(); ait++)
		{
			Face*	Test			= adjacent[ait];
			if (Test==F)			continue;
			if (!Test->bProcessed)	continue;
			if (FaceEqual(*F,*Test)){
				bAlready			= TRUE;
				break;
			}
		}

		//
		if (!bAlready) 
		{
			F->bProcessed	= true;
			CL.add_face(
				F->v[0]->P,F->v[1]->P,F->v[2]->P,
				CDB::edge_open,CDB::edge_open,CDB::edge_open,
				0,0,DWORD(F)
				);
		}
	}

	Status					("Building OBB tree..");
	Msg						("Faces: original(%d), model(%d), ratio(%f)",
		g_faces.size(),CL.getTS(),float(CL.getTS())/float(g_faces.size()));
	RCAST_Model				= new CDB::MODEL;
	// RCAST_Model->setheap	(hLargeHeap);
	switch					(RCAST_Model->build	(CL.getV(),CL.getVS(),CL.getT(),CL.getTS(),TRUE))
	{
	case 0:						break;
	case CDB::err_memory_0:		Msg		("ERR: mem_0");	break;
	case CDB::err_memory_1:		Msg		("ERR: mem_1");	break;
	case CDB::err_memory_2:		Msg		("ERR: mem_2");	break;
	case CDB::err_build:		Msg		("ERR: build");	break;
	}
	Msg						("%d K memory usage",RCAST_Model->memory()/1024);
	
	// Saving for AI/DO usage
	Status					("Saving...");
	CFS_File		MFS		((pBuild->path+"build.cform").c_str());

	// Header
	hdrCFORM hdr;
	hdr.version				= CFORM_CURRENT_VERSION;
	hdr.vertcount			= CL.getVS();
	hdr.facecount			= CL.getTS();
	hdr.aabb				= scene_bb;
	MFS.write				(&hdr,sizeof(hdr));

	// Data
	MFS.write				(CL.getV(),CL.getVS()*sizeof(Fvector));
	MFS.write				(CL.getT(),CL.getTS()*sizeof(CDB::TRI));
}
