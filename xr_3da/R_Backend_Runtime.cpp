#include "stdafx.h"

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
	Memory.mem_fill		(&stat,0,sizeof(stat));
	Vertex.Flush		();
	Index.Flush			();
}
