#include "stdafx.h"
#include "build.h"

const float		tess	= 32.0001f;

void CBuild::Tesselate	()
{
	BOOL	bTesselate	= TRUE;
	DWORD	cnt_splits	= 0;
	
	while (bTesselate)	
	{
		bTesselate		= FALSE;
		for (DWORD I=0; I<g_faces.size(); I++)
		{
			Face* F = g_faces[I];
			
			// Iterate on edges - select longest
			float	max_len		= -1;
			int		max_id		= -1;
			for (DWORD e=0; e<3; e++)
			{
				Vertex			*vA,*vB;
				F->EdgeVerts	(e,&vA,&vB);
				
				float len		= vA->P.distance_to(vB->P);
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

			// 
		}
	}
}
