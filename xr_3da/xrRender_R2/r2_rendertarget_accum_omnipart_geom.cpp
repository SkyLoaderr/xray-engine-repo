#include "stdafx.h"
#include "du_sphere_part.h"

void CRenderTarget::accum_omnip_geom_create		()
{
	u32	dwUsage				= D3DUSAGE_WRITEONLY;

	// vertices
	{
		u32		vCount		= DU_SPHERE_PART_NUMVERTEX;
		u32		vSize		= 3*4;
		R_CHK	(HW.pDevice->CreateVertexBuffer(
			vCount*vSize,
			dwUsage,
			0,
			D3DPOOL_MANAGED,
			&g_accum_omnip_vb,
			0));
		BYTE*	pData				= 0;
		R_CHK						(g_accum_omnip_vb->Lock(0,0,(void**)&pData,0));
		Memory.mem_copy				(pData,du_sphere_part_vertices,vCount*vSize);
		g_accum_omnip_vb->Unlock	();
	}

	// Indices
	{
		u32		iCount		= DU_SPHERE_PART_NUMFACES*3;

		BYTE*	pData		= 0;
		R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&g_accum_omnip_ib,0));
		R_CHK				(g_accum_omnip_ib->Lock(0,0,(void**)&pData,0));
		Memory.mem_copy		(pData,du_sphere_part_faces,iCount*2);
		g_accum_omnip_ib->Unlock	();
	}
}

void CRenderTarget::accum_omnip_geom_destroy()
{
	_RELEASE(g_accum_omnip_ib);
	_RELEASE(g_accum_omnip_vb);
}
