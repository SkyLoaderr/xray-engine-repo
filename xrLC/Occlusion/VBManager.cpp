// VBManager.cpp: implementation of the CVBManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VBManager.h"

CVBManager VB;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVBManager::CVBManager()
{
	pTLB	= NULL;
}

CVBManager::~CVBManager()
{

}

_E(LPDIRECT3DVERTEXBUFFER7 CVBManager::Create(
   DWORD dwVertType,
   DWORD dwVertCount,
   bool bSoftware))
{
	VERIFY		(dwVertCount);

	LPDIRECT3DVERTEXBUFFER7	m_pVertexBuffer;
    D3DVERTEXBUFFERDESC		vbdesc;

	ZeroMemory			(&vbdesc, sizeof(D3DVERTEXBUFFERDESC));
	vbdesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps        = D3DVBCAPS_WRITEONLY;
	vbdesc.dwFVF         = dwVertType;
	vbdesc.dwNumVertices = dwVertCount;

	// If the device does not support transform and lighting in hardware,
	// make sure the vertex buffers end up in system memory.
	vbdesc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;

	CHK_DX(D3D()->CreateVertexBuffer(&vbdesc, &m_pVertexBuffer, 0L ));

	VERIFY(m_pVertexBuffer);
	return m_pVertexBuffer;
} _end;

#include "fmesh.h"
LPDIRECT3DVERTEXBUFFER7 CVBManager::CreateFromData(
	DWORD dwTypeDest,
	DWORD dwTypeSrc,
	DWORD dwVertCount,
	void *pSource,
	bool bSoftware)
{
	void *pDest;
	LPDIRECT3DVERTEXBUFFER7 pVertexBuffer =
		Create(dwTypeDest,dwVertCount,bSoftware);
	CHK_DX(pVertexBuffer->Lock(
		DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,
		&pDest, NULL ));
	VERIFY(pDest);

	ConvertVertices(dwTypeDest, pDest, dwTypeSrc, pSource, dwVertCount);

    CHK_DX(pVertexBuffer->Unlock());
	return pVertexBuffer;
}

void CVBManager::OnDeviceCreate()
{
	pTLB = Create(FTLVERTEX,TLB_SIZE);
}

void CVBManager::OnDeviceDestroy()
{
	_RELEASE(pTLB);
}

FTLvertex*	CVBManager::TLB_Start()
{
	CHK_DX(pTLB->Lock(
		DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,
		(void **)&pStart, NULL ));
	return pStart;
}

void CVBManager::TLB_End(FTLvertex *pEnd) {
	VERIFY((pEnd-pStart) < TLB_SIZE);
	VERIFY((pEnd-pStart) > 0);
	CHK_DX(pTLB->Unlock());
	CHK_DX(Device()->DrawPrimitiveVB(
		D3DPT_TRIANGLESTRIP,
		pTLB,
		0,
		pEnd-pStart,
		0));
}

void CVBManager::TLB_EndIndexed(FTLvertex *pEnd, WORD *pIndices, DWORD Count) {
	VERIFY((pEnd-pStart) < TLB_SIZE);
	CHK_DX(pTLB->Unlock());
	if (pEnd-pStart) {
		CHK_DX(Device()->DrawIndexedPrimitiveVB(
			D3DPT_TRIANGLELIST,
			pTLB,
			0,
			pEnd-pStart,
			pIndices,
			Count,
			0));
	}
}
