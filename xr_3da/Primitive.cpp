#include "stdafx.h"
#pragma hdrstop

extern void	 ConvertVertices(u32 dwTypeDest, void *pDest, u32 dwTypeSrc, void *pSource, u32 dwCount);

//***** VB interface
void	CPrimitive::VB_Attach	(u32 FVF, IDirect3DVertexBuffer8* VB)
{
	_RELEASE(pVertices);
	pVertices	= VB;
	pVertices->AddRef();
	vShader		= FVF;
	vSize		= D3DXGetFVFVertexSize(FVF);
}
void	CPrimitive::VB_Create	(u32 FVF, u32 dwCount,u32 dwUsage, u32 srcFVF, VOID* pData)
{
	Device.Shader.Evict		();
	
	vShader = FVF;
	vSize	= D3DXGetFVFVertexSize(FVF);
	if (HW.Caps.vertex.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
	R_CHK(HW.pDevice->CreateVertexBuffer(
		dwCount*vSize,
		dwUsage,
		FVF,
		(dwUsage&D3DUSAGE_SOFTWAREPROCESSING)?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT,
		&pVertices));
	if (pData){
		ConvertVertices(FVF,VB_Lock(),srcFVF,pData,dwCount);
		VB_Unlock();
	}
}
void*	CPrimitive::VB_Lock		(u32 dwFlags)
{
	BYTE* pData;
	VERIFY(pVertices);
	CHK_DX(pVertices->Lock(0,0,&pData,dwFlags));
	return pData;
}
void	CPrimitive::VB_Unlock	()
{ 
	VERIFY(pVertices);
	CHK_DX(pVertices->Unlock());	
}
void	CPrimitive::VB_Destroy	()			
{ 
	_RELEASE(pVertices);
}

//***** IB interface
void	CPrimitive::IB_SetBase	(u32 dwBaseVert)
{
	dwBaseVertex=dwBaseVert;
}
void	CPrimitive::IB_Attach	(IDirect3DIndexBuffer8* IB)
{
	_RELEASE(pIndices);
	pIndices	= IB;
	pIndices->AddRef();
}
void	CPrimitive::IB_Replicate(CPrimitive& P)
{
	D3DINDEXBUFFER_DESC	desc;
	P.pIndices->GetDesc(&desc);
	WORD* data = P.IB_Lock(D3DLOCK_READONLY);
	IB_Create(P.dwBaseVertex,desc.Size/2,desc.Usage,data);
	P.IB_Unlock();
}
void	CPrimitive::IB_Create	(u32 dwBaseVert, u32 dwCount, u32 dwUsage,VOID* pData)
{
	Device.Shader.Evict		();
	
	dwBaseVertex			= dwBaseVert;
	if (HW.Caps.vertex.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
	R_CHK(HW.pDevice->CreateIndexBuffer(
		dwCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_DEFAULT,
		&pIndices
		));
	if (pData){
		PSGP.memCopy(IB_Lock(),pData,dwCount*2);
		IB_Unlock	();
	}
}
WORD*	CPrimitive::IB_Lock				(u32 dwFlags)
{
	BYTE* pData;
	VERIFY(pIndices);
	CHK_DX(pIndices->Lock(0,0,&pData,dwFlags));
	return LPWORD(pData);
}
void	CPrimitive::IB_Unlock			()
{
	VERIFY(pIndices);
	CHK_DX(pIndices->Unlock());
}
void	CPrimitive::IB_Destroy			()
{ 
	_RELEASE(pIndices);
}
