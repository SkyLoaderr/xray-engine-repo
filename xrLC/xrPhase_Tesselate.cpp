#include "stdafx.h"
#include "build.h"

void CBuild::Tesselate	()
{
	DWORD	cnt_splits	= 0;
	g_bUnregister		= FALSE;
	
	const float	tess	= g_params.m_SS_maxsize/2.f;

	for (int I=0; I<int(g_faces.size()); I++)
	{
		Face* F				= g_faces[I];
		if (0==F)			continue;
		
		Progress			(float(I)/float(g_faces.size()));
		
		// Iterate on edges - select longest
		float	max_len		= -1;
		int		max_id		= -1;
		for (DWORD e=0; e<3; e++)
		{
			float len		= F->EdgeLen(e);
			if (len>max_len)	
			{
				max_len = len;
				max_id	= e;
			}
		}
		if (max_len<tess)	continue;
		
		// Split edge
		cnt_splits	++;
		
		// indices
		int id1				= edge2idx[max_id][0];
		int id2				= edge2idx[max_id][1];
		int idB				= 3-(id1+id2);
		
		// lerp
		Vertex*		V		= VertexPool.create();
		V->P.lerp			(F->v[id1]->P, F->v[id2]->P, .5f);
		UVpoint		UV;
		UV.averageA			(F->tc.front().uv[id1],F->tc.front().uv[id2]);
		
		// F1
		Face* F1			= FacePool.create();
		F1->dwMaterial		= F->dwMaterial;
		F1->SetVertices		(F->v[idB],F->v[id1],V);
		F1->AddChannel		(F->tc.front().uv[idB],F->tc.front().uv[id1],UV);
		
		// F2
		Face* F2			= FacePool.create();
		F2->dwMaterial		= F->dwMaterial;
		F2->SetVertices		(F->v[idB],V,F->v[id2]);
		F2->AddChannel		(F->tc.front().uv[idB],UV,F->tc.front().uv[id2]);
		
		// Destroy old face
		FacePool.destroy	(g_faces[I]);
	}
	
	Msg ("%d splits performed.",cnt_splits);
	
	// Remove ZEROed faces
	g_faces.erase(remove(g_faces.begin(),g_faces.end(),(Face*)0),g_faces.end());
	
	g_bUnregister		= TRUE;
}
