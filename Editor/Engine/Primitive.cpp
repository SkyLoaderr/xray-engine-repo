#include "stdafx.h"

extern void	 ConvertVertices(DWORD dwTypeDest, void *pDest, DWORD dwTypeSrc, void *pSource, DWORD dwCount);

//***** VB interface
void	CPrimitive::VB_Attach	(DWORD FVF, IDirect3DVertexBuffer8* VB)
{
	_RELEASE(pVertices);
	pVertices	= VB;
	pVertices->AddRef();
	vShader		= FVF;
	vSize		= D3DXGetFVFVertexSize(FVF);
}
void	CPrimitive::VB_Create	(DWORD FVF, DWORD dwCount,DWORD dwUsage, DWORD srcFVF, VOID* pData)
{
	CHK_DX(HW.pDevice->ResourceManagerDiscardBytes(0));

	vShader = FVF;
	vSize	= D3DXGetFVFVertexSize(FVF);
	if (HW.Caps.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
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
void*	CPrimitive::VB_Lock		(DWORD dwFlags)
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
void	CPrimitive::IB_SetBase	(DWORD dwBaseVert)
{
	dwBaseVertex=dwBaseVert;
}
void	CPrimitive::IB_Attach	(DWORD dwBaseVert, IDirect3DIndexBuffer8* IB)
{
	_RELEASE(pIndices);
	pIndices	= IB;
	pIndices->AddRef();
	dwBaseVertex=dwBaseVert;
}
void	CPrimitive::IB_Replicate(CPrimitive& P)
{
	D3DINDEXBUFFER_DESC	desc;
	P.pIndices->GetDesc(&desc);
	WORD* data = P.IB_Lock(D3DLOCK_READONLY|D3DLOCK_NOSYSLOCK);
	IB_Create(P.dwBaseVertex,desc.Size/2,desc.Usage,data);
	P.IB_Unlock();
}
void	CPrimitive::IB_Create	(DWORD dwBaseVert, DWORD dwCount, DWORD dwUsage,VOID* pData)
{
	CHK_DX(HW.pDevice->ResourceManagerDiscardBytes(0));

	dwBaseVertex=dwBaseVert;
	if (HW.Caps.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
	R_CHK(HW.pDevice->CreateIndexBuffer(
		dwCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_DEFAULT,
		&pIndices
		));
	if (pData){
		CopyMemory(IB_Lock(),pData,dwCount*2);
		IB_Unlock();
	}
}
WORD*	CPrimitive::IB_Lock				(DWORD dwFlags)
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
