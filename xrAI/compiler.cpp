#include "stdafx.h"
#include "compiler.h"

#include "cl_intersect.h"

Nodes			g_nodes;
Merged			g_merged;
Lights			g_lights;
SAIParams		g_params;
CDB::MODEL		Level;
CDB::MODEL		LevelLight;
CDB::COLLIDER	XRC;
Fbox			LevelBB;
Vectors			Emitters;

void Node::PointLF(Fvector& D)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= g_params.fPatchSize/2;
	v.x			-= s;
	v.z			+= s;
	Plane.intersectRayPoint(v,d,D);
}

void Node::PointFR(Fvector& D)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= g_params.fPatchSize/2;
	v.x			+= s;
	v.z			+= s;
	Plane.intersectRayPoint(v,d,D);
}

void Node::PointRB(Fvector& D)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= g_params.fPatchSize/2;
	v.x			+= s;
	v.z			-= s;
	Plane.intersectRayPoint(v,d,D);
}

void Node::PointBL(Fvector& D)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= g_params.fPatchSize/2;
	v.x			-= s;
	v.z			-= s;
	Plane.intersectRayPoint(v,d,D);
}

void	mem_Optimize	()
{
	Memory.mem_compact	();
	Msg("* Memory usage: %d M",Memory.mem_usage()/(1024*1024));
}

void xrCompiler(LPCSTR name)
{
	Phase("Loading level...");
	xrLoad		(name);
	mem_Optimize();

//	Phase("Building nodes...");
//	xrBuildNodes();
//	Msg("%d nodes created",int(g_nodes.size()));
//	mem_Optimize();
//	
	Phase("Smoothing nodes...");
	xrSmoothNodes();
	mem_Optimize();
	
	Phase("Lighting nodes...");
	xrLight		();
//	xrDisplay	();
	mem_Optimize();
	
	Phase("Calculating coverage...");
	xrCover		();
	mem_Optimize();
	
	/////////////////////////////////////

	Phase("Merging nodes...");
	xrMerge		();
	mem_Optimize();
	
	Phase("Converting and linking...");
	xrConvertAndLink();
	mem_Optimize();
	
	Phase("Visualizing nodes...");
	xrDisplay	();

	Phase("Saving nodes...");
	xrSaveNodes	(name);
	mem_Optimize();
}
