#include "stdafx.h"
#include "compiler.h"

#include "cl_intersect.h"

Nodes			g_nodes;
Merged			g_merged;
Lights			g_lights;
Params			g_params;
RAPID::Model	Level;
RAPID::Model	LevelLight;
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

void xrCompiler(LPCSTR name)
{
	Phase("Loading level...");
	xrLoad		(name);

	Phase("Building nodes...");
	xrBuildNodes();
	Msg("%d nodes created",int(g_nodes.size()));

	Phase("Smoothing nodes...");
	xrSmoothNodes();

	Phase("Lighting nodes...");
	xrLight		();
//	xrDisplay	();

	Phase("Calculating coverage...");
	xrCover		();

	Phase("Merging nodes...");
	xrMerge		();

	Phase("Converting and linking...");
	xrConvertAndLink();

	Phase("Visualizing nodes...");
//	xrDisplay	();

	Phase("Saving nodes...");
	xrSaveNodes	(name);
}
