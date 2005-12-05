#include "stdafx.h"

Fvector du_cone_vertices[DU_CONE_NUMVERTEX]=
{
	0.0000f,    0.0000f, 0.0000f,
	0.5000f,   0.0000f,	 1.0000f,
	0.4619f,   0.1913f,	 1.0000f,
	0.3536f,   0.3536f,	 1.0000f,
	0.1913f,   0.4619f,	 1.0000f,
	-0.0000f,  0.5000f,	 1.0000f,
	-0.1913f,  0.4619f,	 1.0000f,
	-0.3536f,  0.3536f,	 1.0000f,
	-0.4619f,  0.1913f,	 1.0000f,
	-0.5000f,  -0.0000f, 1.0000f,
	-0.4619f,  -0.1913f, 1.0000f,
	-0.3536f,  -0.3536f, 1.0000f,
	-0.1913f,  -0.4619f, 1.0000f,
	0.0000f,   -0.5000f, 1.0000f,
	0.1913f,   -0.4619f, 1.0000f,
	0.3536f,   -0.3536f, 1.0000f,
	0.4619f,   -0.1913f, 1.0000f,
	0.0000f,   0.0000f,	 1.0000f+EPS_L
};
u16 du_cone_faces[DU_CONE_NUMFACES*3]=
{
	0,	2,	1,
	0,	3,	2,
	0,	4,	3,
	0,  5,	4,
	0,	6,	5,
	0,  7,  6,
	0,  8,  7,
	0,  9,  8,
	0, 10,  9,
	0, 11, 10,
	0, 12, 11,
	0, 13, 12,
	0, 14, 13,
	0, 15, 14,
	0, 16, 15,
	0,  1, 16,
	17, 1, 2,
	17, 2, 3,
	17, 3, 4,
	17, 4, 5,
	17, 5, 6,
	17, 6, 7,
	17, 7, 8,
	17, 8, 9,
	17, 9, 10,
	17,10, 11,
	17,11, 12,
	17,12, 13,
	17,13, 14,
	17,14, 15,
	17,15, 16,
	17,16, 1
};


void CRenderTarget::accum_spot_geom_create	()
{
	u32	dwUsage				= D3DUSAGE_WRITEONLY;

	// vertices
	{
		u32		vCount		= DU_CONE_NUMVERTEX;
		u32		vSize		= 3*4;
		R_CHK	(HW.pDevice->CreateVertexBuffer(
			vCount*vSize,
			dwUsage,
			0,
			D3DPOOL_MANAGED,
			&g_accum_spot_vb,
			0));
		BYTE*	pData				= 0;
		R_CHK						(g_accum_spot_vb->Lock(0,0,(void**)&pData,0));
		CopyMemory				(pData,du_cone_vertices,vCount*vSize);
		g_accum_spot_vb->Unlock	();
	}

	// Indices
	{
		u32		iCount		= DU_CONE_NUMFACES*3;

		BYTE*	pData		= 0;
		R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&g_accum_spot_ib,0));
		R_CHK				(g_accum_spot_ib->Lock(0,0,(void**)&pData,0));
		CopyMemory		(pData,du_cone_faces,iCount*2);
		g_accum_spot_ib->Unlock	();
	}
}

void CRenderTarget::accum_spot_geom_destroy()
{
	_RELEASE	(g_accum_spot_ib);
	_RELEASE	(g_accum_spot_vb);
}
