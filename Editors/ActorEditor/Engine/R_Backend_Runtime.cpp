#include "stdafx.h"
#pragma hdrstop

void CBackend::OnFrameEnd	()
{
	for (u32 stage=0; stage<HW.Caps.pixel.dwStages; stage++)
		CHK_DX(HW.pDevice->SetTexture(0,0));
	CHK_DX				(HW.pDevice->SetStreamSource	(0,0,0,0));
	CHK_DX				(HW.pDevice->SetIndices			(0));
	CHK_DX				(HW.pDevice->SetVertexShader	(0));
	CHK_DX				(HW.pDevice->SetPixelShader		(0));
	Invalidate			();
}

void CBackend::OnFrameBegin	()
{
	PGO					(Msg("PGO:*****frame[%d]*****",Device.dwFrame));
	Memory.mem_fill		(&stat,0,sizeof(stat));
	Vertex.Flush		();
	Index.Flush			();
	set_Stencil			(FALSE);
}

void CBackend::Invalidate	()
{
	pRT[0]						= NULL;
	pRT[1]						= NULL;
	pRT[2]						= NULL;
	pRT[3]						= NULL;
	pZB							= NULL;

	decl						= NULL;
	vb							= NULL;
	ib							= NULL;
	vb_stride					= 0;

	state						= NULL;
	ps							= NULL;
	vs							= NULL;
	ctable						= NULL;

	T							= NULL;
	M							= NULL;
	C							= NULL;

	textures[0]=textures[1]=textures[2]=textures[3]=textures[4]=textures[5]=textures[6]=textures[7]=NULL;
	matrices[0]=matrices[1]=matrices[2]=matrices[3]=matrices[4]=matrices[5]=matrices[6]=matrices[7]=NULL;
	cull_mode					= u32(-1);
	colorwrite_mask				= u32(-1);
}
