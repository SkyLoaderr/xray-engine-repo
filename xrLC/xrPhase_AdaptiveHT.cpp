#include "stdafx.h"
#include "build.h"

const	float	aht_min_edge	= 0.1f;			// 10cm
const	float	aht_min_err		= 8.f/255.f;	// 8 

void CBuild::xrPhase_AdaptiveHT	()
{
	CDB::COLLIDER	DB;
	DB.ray_options	(0);

	u32				cnt_verts	= g_vertices.size	();
	u32				cnt_faces	= g_faces.size		();

	// calc approximate normals for vertices
	for (u32 vit=0; vit<cnt_verts; vit++)
		g_vertices[vit]->normalFromAdj	();

	// clear split flag from all faces
	for (u32 fit=0; fit<cnt_faces; fit++)
	{
		g_faces[fit]->flags.bSplitted		= false;
		g_faces[fit]->flags.bLocked			= !g_faces[fit]->flags.bOpaque;
	}

	// main process
	g_bUnregister		= FALSE;
	xr_vector<Face*>	adjacent;	adjacent.reserve(6*2*3);
	for (u32 I=0; I<g_faces.size(); I++)
	{
		Face* F					= g_faces[I];
		if (0==F)				continue;
		if (F->flags.bSplitted)	continue;

		Progress				(float(I)/float(g_faces.size()));

		// Calculate hemisphere, if need it
		for (u32 vh=0; vh<3; vh++)
		{
			Vertex* V				= F->v	[vh];
			if (V->C._tmp_ > EPS)	continue;
			LightPoint				(&DB, RCAST_Model, V->C, V->P, V->N, pBuild->L_static, LP_dont_rgb+LP_dont_sun+LP_UseFaceDisable, F);
			V->C._tmp_				= 1;	// use as marker
		}

		// Iterate on edges - select longest
		float	max_err		= -1;
		int		max_id		= -1;
		for (u32 e=0; e<3; e++)
		{
			Vertex					*V1,*V2;
			F->EdgeVerts			(e,&V1,&V2);
			float len				= V1->P.distance_to	(V2->P);		// len
			float err				= _abs(V1->C.hemi - V2->C.hemi);	// error in hemi-space
			if (len<aht_min_edge)	continue;
			if (err<aht_min_err)	continue;
			if (err>max_err)
			{
				max_err = err;
				max_id	= e;
			}
		}
		if (max_id<0)		continue;	// nothing selected

		// now, we need to tesselate all faces which shares this 'problematic' edge
		// collect all this faces
		Vertex			*V1,*V2;
		F->EdgeVerts	(max_id,&V1,&V2);
		adjacent.clear	();
		for (u32 adj=0; adj<V1->adjacent.size(); adj++)
		{
			Face* A					= V1->adjacent[adj];
			if (A->VContains(V2))	
				adjacent.push_back	(A);
		}
		std::sort		(adjacent.begin(),adjacent.end());
		adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());

		// create new vertex (lerp)
		Vertex*		V		= VertexPool.create();
		V->P.lerp			(V1->P, V2->P, .5f);
		Fvector2			UV;
		UV.averageA			(F->tc.front().uv[F->VIndex(V1)],F->tc.front().uv[F->VIndex(V2)]);

		// iterate on faces which shares this 'problematic' edge
		for (u32 af_it=0; af_it<adjacent.size(); af_it++)
		{
			Face*	AF			= adjacent[af_it];
			AF->flags.bSplitted	= true;

			// indices
			int id1				= AF->VIndex(V1);
			int id2				= AF->VIndex(V2);
			int idB				= 3-(id1+id2);

			// F1
			Face* F1			= FacePool.create();
			F1->flags.bSplitted	= false;
			F1->dwMaterial		= AF->dwMaterial;
			F1->dwMaterialGame	= AF->dwMaterialGame;
			F1->SetVertices		(AF->v[idB],AF->v[id1],V);
			F1->AddChannel		(AF->tc.front().uv[idB],AF->tc.front().uv[id1],UV);

			// F2
			Face* F2			= FacePool.create();
			F2->flags.bSplitted	= false;
			F2->dwMaterial		= AF->dwMaterial;
			F2->dwMaterialGame	= AF->dwMaterialGame;
			F2->SetVertices		(AF->v[idB],V,AF->v[id2]);
			F2->AddChannel		(AF->tc.front().uv[idB],UV,AF->tc.front().uv[id2]);

			// don't destroy old face	(it can be used as occluder during ray-trace)
			if (!AF->flags.bLocked)		FacePool.destroy	(AF);
		}
		V->normalFromAdj		();
	}

	// Cleanup
	xr_delete			(RCAST_Model);
	for (I=0; I<g_faces.size(); I++)	if (0==g_faces[I] || g_faces[I]->flags.bSplitted)	FacePool.destroy	(g_faces[I]);
	for (I=0; I<g_vertices.size(); I++)	if (0==g_vertices[I]->adjacent.size())				VertexPool.destroy	(g_vertices[I]);
	g_faces.erase		(std::remove(g_faces.begin(),g_faces.end(),(Face*)0),g_faces.end());
	g_vertices.erase	(std::remove(g_vertices.begin(),g_vertices.end(),(Vertex*)0),g_vertices.end());
	g_bUnregister		= true;

	//
	u32				cnt_verts2	= g_vertices.size	();
	u32				cnt_faces2	= g_faces.size		();
	clMsg			("* faces:    was(%d), become(%d)",cnt_faces,cnt_faces2);
	clMsg			("* vertices: was(%d), become(%d)",cnt_verts,cnt_verts2);
}
