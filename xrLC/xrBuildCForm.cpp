#include "stdafx.h"
#include "cl_collector.h"
#include "build.h"
#include "fs.h"

int GetVertexIndex(Vertex *F)
{
	vecVertexIt it = lower_bound(g_vertices.begin(),g_vertices.end(),F);
	return it-g_vertices.begin();
}

int getCFormVID(vecVertex& V,Vertex *F)
{
	vecVertexIt it = lower_bound(V.begin(),V.end(),F);
	return it-V.begin();
}

int bCriticalErrCnt = 0;
int getTriByEdge(Vertex *V1, Vertex *V2, Face* parent, vecFace &ids)
{
	Face*	found	= 0;
	int		f_count = 0;

	for (vecFaceIt I=V1->adjacent.begin(); I!=V1->adjacent.end(); I++)
	{
		Face* test = *I;
		if (test == parent) continue;
		if (test->VContains(V2)) {
			f_count++;
			found = test;
		}
	}
	if (f_count>1) {
		bCriticalErrCnt++;
	}
	if (found) {
		vecFaceIt F = lower_bound(ids.begin(),ids.end(),found);
		if (found == *F) return F-ids.begin();
		else return -1;
	} else {
		return -1;
	}
}

void CBuild::BuildCForm(CFS_Base &fs)
{
	// Collecting data
	Phase	("CFORM: creating...");
	vecFace		cfFaces;
	vecVertex	cfVertices;
	{
		vector<bool>	cfVertexMarks(g_vertices.size(),false);

		Status("Sorting...");
		std::sort(g_vertices.begin(),g_vertices.end());

		Status("Collecting faces...");
		for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++)
		{
			Face* F = *I;
			if (F->Shader().C.bCollision) 
			{
				cfFaces.push_back(F);
				cfVertexMarks[GetVertexIndex(F->v[0])] = true;
				cfVertexMarks[GetVertexIndex(F->v[1])] = true;
				cfVertexMarks[GetVertexIndex(F->v[2])] = true;
			}
		}

		Status("Collecting vertices...");
		std::sort(cfFaces.begin(),cfFaces.end());
		for (DWORD V=0; V<g_vertices.size(); V++)
			if (cfVertexMarks[V]) cfVertices.push_back(g_vertices[V]);
	}

	float	p_total = 0;
	float	p_cost  = 1.f/(cfVertices.size());
	
	Fbox BB; BB.invalidate();
	for (vecVertexIt it = cfVertices.begin(); it!=cfVertices.end(); it++)
		BB.modify((*it)->P );

	// CForm
	Phase	("CFORM: collision model...");
	Status	("Items to process: %d", cfFaces.size());
	p_total = 0;
	p_cost  = 1.f/(cfFaces.size());

	// Collect faces
	RAPID::CollectorPacked CL(BB);
	for (vecFaceIt F = cfFaces.begin(); F!=cfFaces.end(); F++)
	{
		Face*	T = *F;
		CL.add_face(
			T->v[0]->P, T->v[1]->P, T->v[2]->P,
			getTriByEdge(T->v[0],T->v[1],T,cfFaces),
			getTriByEdge(T->v[1],T->v[2],T,cfFaces),
			getTriByEdge(T->v[2],T->v[0],T,cfFaces),
			0,materials[T->dwMaterial].sector,0
			);
		Progress(p_total+=p_cost);		// progress
	}
	if (bCriticalErrCnt)
		Msg("MultipleEdges: %d faces",bCriticalErrCnt);

	// Saving
	CFS_Memory		MFS;
	Status("Saving...");

	// Header
	hdrCFORM hdr;
	hdr.version		= CFORM_CURRENT_VERSION;
	hdr.vertcount	= CL.getVS();
	hdr.facecount	= CL.getTS();
	hdr.aabb		= BB;
	MFS.write		(&hdr,sizeof(hdr));

	// Data
	MFS.write		(CL.getV(),CL.getVS()*sizeof(Fvector));
	MFS.write		(CL.getT(),CL.getTS()*sizeof(RAPID::tri));

	// Compress chunk
	fs.write_chunk	(fsL_CFORM|CFS_CompressMark,MFS.pointer(),MFS.size());

	cfFaces.clear();
	cfVertices.clear();
}

void CBuild::BuildPortals(CFS_Base& fs)
{
	fs.write_chunk(fsL_PORTALS,portals.begin(),portals.size()*sizeof(b_portal));
}
