#include "stdafx.h"
#include "build.h"

const	float	aht_min_edge	= 16.0f;		// 50 cm
const	float	aht_min_err		= 64.f/255.f;	// ~10% error

void CBuild::xrPhase_AdaptiveHT	()
{
	CDB::COLLIDER	DB;
	DB.ray_options	(0);

	//for (u32 pass=0; pass<16; pass++)
	//{
		// Build model
		FPU::m64r					();
		Phase						("Building hemisphere-RayCast model...");
		mem_Compact					();
		BuildRapid					(FALSE);

		// Prepare
		FPU::m64r					();
		Phase						("Adaptive HT...");
		mem_Compact					();
		Light_prepare				();

		// Perform AHT
		u32				cnt_verts		= g_vertices.size	();
		u32				cnt_faces		= g_faces.size		();
		u32				counter_create	= 0;

		// clear split flag from all faces
		for (u32 fit=0; fit<cnt_faces; fit++)
		{
			g_faces[fit]->flags.bSplitted		= false;
			g_faces[fit]->flags.bLocked			= true;
			g_faces[fit]->CalcNormal			();
		}

		// calc approximate normals for vertices + base lighting
		base_color_c							vC;
		for (u32 vit=0; vit<cnt_verts; vit++)	
		{
			Vertex*		V	= g_vertices[vit];
			V->normalFromAdj	();
			LightPoint			(&DB, RCAST_Model, vC, V->P, V->N, pBuild->L_static, LP_dont_rgb+LP_dont_sun,0);
			V->C._set			(vC);
		}

		// main process
		g_bUnregister		= FALSE;
		xr_vector<Face*>	adjacent;	adjacent.reserve(6*2*3);
		for (u32 I=0; I<g_faces.size(); I++)
		{
			Face* F					= g_faces[I];
			if (0==F)				continue;
			if (F->flags.bSplitted)	{
				if (!F->flags.bLocked)	FacePool.destroy	(g_faces[I]);
				continue;
			}
			if (F->CalcArea()<EPS_L)continue;

			Progress				(float(I)/float(g_faces.size()));

			// Iterate on edges - select longest
			float	max_err			= -1;
			int		max_id			= -1;
			for (u32 e=0; e<3; e++)
			{
				Vertex					*V1,*V2;
				F->EdgeVerts			(e,&V1,&V2);
				float len				= V1->P.distance_to	(V2->P);	// len
				base_color_c			v1c; V1->C._get(v1c);			// C1
				base_color_c			v2c; V2->C._get(v2c);			// C2
				float err				= _abs(v1c.hemi - v2c.hemi);	// error in hemi-space
				if (len<aht_min_edge)	continue;
				if (len>max_err)
				{
					max_err = len;
					max_id	= e;
				}
				/*
				if (err<aht_min_err)	continue;
				if (err>max_err)
				{
					max_err = err;
					max_id	= e;
				}
				*/
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
				if (A->flags.bSplitted)	continue;
				if (A->VContains(V2))	adjacent.push_back	(A);
			}
			std::sort		(adjacent.begin(),adjacent.end());
			adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());

			// create new vertex (lerp)
			counter_create		++;
			if (0==(counter_create%10000))	{
				for (u32 I=0; I<g_vertices.size(); I++)	if (0==g_vertices[I]->adjacent.size())	VertexPool.destroy	(g_vertices[I]);
				Status				("%d verts created, %d(now) / %d(was)",counter_create,g_vertices.size(),cnt_verts);
				FlushLog			();
			}

			Vertex*		V		= VertexPool.create();
			V->P.lerp			(V1->P, V2->P, .5f);
			Fvector2			UV;
			UV.averageA			(F->tc.front().uv[F->VIndex(V1)],F->tc.front().uv[F->VIndex(V2)]);

			// iterate on faces which share this 'problematic' edge
			for (u32 af_it=0; af_it<adjacent.size(); af_it++)
			{
				Face*	AF			= adjacent[af_it];
				VERIFY				(false==AF->flags.bSplitted);
				AF->flags.bSplitted	= true;

				// indices
				int id1				= AF->VIndex(V1);
				int id2				= AF->VIndex(V2);
				int idB				= 3-(id1+id2);

				// F1
				Face* F1			= FacePool.create();
				F1->flags.bSplitted	= false;
				F1->flags.bLocked	= false;
				F1->dwMaterial		= AF->dwMaterial;
				F1->dwMaterialGame	= AF->dwMaterialGame;
				F1->SetVertices		(AF->v[idB],AF->v[id1],V);
				F1->AddChannel		(AF->tc.front().uv[idB],AF->tc.front().uv[id1],UV);
				F1->CalcNormal		();

				// F2
				Face* F2			= FacePool.create();
				F2->flags.bSplitted	= false;
				F2->flags.bLocked	= false;
				F2->dwMaterial		= AF->dwMaterial;
				F2->dwMaterialGame	= AF->dwMaterialGame;
				F2->SetVertices		(AF->v[idB],V,AF->v[id2]);
				F2->AddChannel		(AF->tc.front().uv[idB],UV,AF->tc.front().uv[id2]);
				F2->CalcNormal		();

				// don't destroy old face	(it can be used as occluder during ray-trace)
				// if (AF->bLocked)	continue;
				// FacePool.destroy	(g_faces[I]);
			}

			V->normalFromAdj		();
			LightPoint				(&DB, RCAST_Model, vC, V->P, V->N, pBuild->L_static, LP_dont_rgb+LP_dont_sun,0);
			V->C._set				(vC);
		}

		// Cleanup
		for (u32 I=0; I<g_faces.size(); I++)	if (0!=g_faces[I] && g_faces[I]->flags.bSplitted)	FacePool.destroy	(g_faces[I]);
		for (u32 I=0; I<g_vertices.size(); I++)	if (0==g_vertices[I]->adjacent.size())				VertexPool.destroy	(g_vertices[I]);
		g_faces.erase		(std::remove(g_faces.begin(),g_faces.end(),(Face*)0),g_faces.end());
		g_vertices.erase	(std::remove(g_vertices.begin(),g_vertices.end(),(Vertex*)0),g_vertices.end());
		g_bUnregister		= true;

		//
		u32				cnt_verts2	= g_vertices.size	();
		u32				cnt_faces2	= g_faces.size		();
		clMsg			("* faces:    was(%d), become(%d)",cnt_faces,cnt_faces2);
		clMsg			("* vertices: was(%d), become(%d)",cnt_verts,cnt_verts2);
	//	if (cnt_faces2 == cnt_faces)	break;			// nothing was tesselated - exit
	//}
}
