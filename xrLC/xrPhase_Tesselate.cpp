#include "stdafx.h"
#include "build.h"

const float		tess	= 32.0001f;

void CBuild::Tesselate	()
{
	BOOL	bTesselate	= TRUE;
	DWORD	cnt_splits	= 0;
	DWORD	cnt_pass	= 0;
	g_bUnregister		= FALSE;
	
	while (bTesselate)	
	{
		bTesselate		= FALSE;
		cnt_pass		++;
		cnt_splits		= 0;
		
		string256		phase_name;
		sprintf			(phase_name,"Tesselating, pass %d...",cnt_pass);
		Phase			(phase_name);
		
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
			bTesselate			= TRUE;
			cnt_splits	++;

			// indices
			int id1				= edge2idx[e][0];
			int id2				= edge2idx[e][1];
			int idB				= 3-(id1+id2);

			// lerp
			Vertex*		V		= new Vertex;
			V->P.lerp			(F->v[id1]->P, F->v[id2]->P, .5f);
			UVpoint		UV;
			UV.averageA			(F->tc.front()[id1],F->tc.front()[id2]);

			// F1
			Face* F1			= new Face;
			F1->dwMaterial		= F->dwMaterial;
			F1->SetVertices		(F->v[idB],F->v[id1],V);
			F1->AddChannel		(F->tc.front()[idB],F->tc.front()[id1],UV);
			
			// F2
			Face* F2			= new Face;
			F2->dwMaterial		= F->dwMaterial;
			F2->SetVertices		(F->v[idB],V,F->v[id2]);
			F2->AddChannel		(F->tc.front()[idB],UV,F->tc.front()[id2]);

			// Destroy old face
			_DELETE				(F);
		}

		Msg ("%d splits performed.",cnt_splits);
	}

	// Remove ZERO faces


	g_bUnregister		= TRUE;
}
