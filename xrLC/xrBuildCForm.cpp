#include "stdafx.h"
#include "cl_collector.h"
#include "build.h"
#include "fs.h"

int GetVertexIndex(Vertex *F)
{
	vecVertexIt it = std::lower_bound(g_vertices.begin(),g_vertices.end(),F);
	return it-g_vertices.begin();
}

int getCFormVID(vecVertex& V,Vertex *F)
{
	vecVertexIt it = std::lower_bound(V.begin(),V.end(),F);
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
		bCriticalErrCnt	++;
		pBuild->err_multiedge.w_fvector3(V1->P);
		pBuild->err_multiedge.w_fvector3(V2->P);
	}
	if (found) {
		vecFaceIt F = std::lower_bound(ids.begin(),ids.end(),found);
		if (found == *F) return F-ids.begin();
		else return -1;
	} else {
		return -1;
	}
}

void CBuild::BuildCForm(IWriter &fs)
{
	// Collecting data
	Phase	("CFORM: creating...");
	vecFace*	cfFaces		= xr_new<vecFace>	();
	vecVertex*	cfVertices	= xr_new<vecVertex>	();
	{
		xr_vector<bool>	cfVertexMarks;
		cfVertexMarks.assign	(g_vertices.size(),false);

		Status("Sorting...");
		std::sort(g_vertices.begin(),g_vertices.end());

		Status("Collecting faces...");
		cfFaces->reserve	(g_faces.size());
		for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++)
		{
			Face* F = *I;
			if (F->Shader().flags.bCollision) 
			{
				cfFaces->push_back(F);
				cfVertexMarks[GetVertexIndex(F->v[0])] = true;
				cfVertexMarks[GetVertexIndex(F->v[1])] = true;
				cfVertexMarks[GetVertexIndex(F->v[2])] = true;
			}
		}

		Status("Collecting vertices...");
		cfVertices->reserve	(g_vertices.size());
		std::sort(cfFaces->begin(),cfFaces->end());
		for (DWORD V=0; V<g_vertices.size(); V++)
			if (cfVertexMarks[V]) cfVertices->push_back(g_vertices[V]);
	}

	float	p_total = 0;
	float	p_cost  = 1.f/(cfVertices->size());
	
	Fbox BB; BB.invalidate();
	for (vecVertexIt it = cfVertices->begin(); it!=cfVertices->end(); it++)
		BB.modify((*it)->P );

	// CForm
	Phase	("CFORM: collision model...");
	Status	("Items to process: %d", cfFaces->size());
	p_total = 0;
	p_cost  = 1.f/(cfFaces->size());

	// Collect faces
	CDB::CollectorPacked CL	(BB,cfVertices->size(),cfFaces->size());
	for (vecFaceIt F = cfFaces->begin(); F!=cfFaces->end(); F++)
	{
		Face*	T = *F;
		CL.add_face(
			T->v[0]->P, T->v[1]->P, T->v[2]->P,
			getTriByEdge(T->v[0],T->v[1],T,*cfFaces),
			getTriByEdge(T->v[1],T->v[2],T,*cfFaces),
			getTriByEdge(T->v[2],T->v[0],T,*cfFaces),
			0,materials[T->dwMaterial].sector,T->dwMaterialGame
			);
		Progress(p_total+=p_cost);		// progress
	}
	if (bCriticalErrCnt)
		clMsg("MultipleEdges: %d faces",bCriticalErrCnt);
	xr_delete		(cfFaces);
	xr_delete		(cfVertices);

	// Models
	Status			("Models...");
	for (u32 ref=0; ref<mu_refs.size(); ref++)
		mu_refs[ref]->export_cform_game(CL);

	// Saving
	CMemoryWriter	MFS;
	Status			("Saving...");

	// Header
	hdrCFORM hdr;
	hdr.version		= CFORM_CURRENT_VERSION;
	hdr.vertcount	= CL.getVS();
	hdr.facecount	= CL.getTS();
	hdr.aabb		= BB;
	MFS.w			(&hdr,sizeof(hdr));

	// Data
	MFS.w			(CL.getV(),CL.getVS()*sizeof(Fvector));
	MFS.w			(CL.getT(),CL.getTS()*sizeof(CDB::TRI));

	// Compress chunk
	fs.w_chunk		(fsL_CFORM|CFS_CompressMark,MFS.pointer(),MFS.size());

	// clear pDeflector (it is stored in the same memory space with dwMaterialGame)
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++)
	{
		Face* F			= *I;
		F->pDeflector	= NULL;
	}
}

void CBuild::BuildPortals(IWriter& fs)
{
	fs.w_chunk		(fsL_PORTALS,&*portals.begin(),portals.size()*sizeof(b_portal));
}
