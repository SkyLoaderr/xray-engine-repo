#pragma once

class ENGINE_API CPrimitive
{
public:
	friend class CDraw;
	IDirect3DVertexBuffer8*		pVertices;
	IDirect3DIndexBuffer8*		pIndices;
	DWORD						dwBaseVertex;
	DWORD						vShader;
	DWORD						vSize;
public:
	CPrimitive() : pVertices(0),pIndices(0),dwBaseVertex(0),vShader(0),vSize(0) {};

	//***** VB interface
	void	VB_Attach			(DWORD FVF, IDirect3DVertexBuffer8* VB);
	void	VB_Create			(DWORD FVF, DWORD dwCount,DWORD dwUsage=D3DUSAGE_WRITEONLY, DWORD srcFVF=0, VOID* pData=NULL);
	void*	VB_Lock				(DWORD dwFlags = D3DLOCK_NOSYSLOCK);
	void	VB_Unlock			();
	void	VB_Destroy			();

	//***** IB interface
	void	IB_SetBase			(DWORD dwBaseVert);
	void	IB_Attach			(DWORD dwBaseVert, IDirect3DIndexBuffer8* IB);
	void	IB_Replicate		(CPrimitive& P);
	void	IB_Create			(DWORD dwBaseVert, DWORD dwCount, DWORD dwUsage=D3DUSAGE_WRITEONLY,VOID* pData=NULL);
	WORD*	IB_Lock				(DWORD dwFlags = D3DLOCK_NOSYSLOCK);
	void	IB_Unlock			();
	void	IB_Destroy			();

	// General
	IC void	TakeCopy(CPrimitive &P)
	{
		VB_Attach(P.vShader,P.pVertices);
		IB_Attach(P.dwBaseVertex,P.pIndices);
	}
	IC DWORD	GetFVF() {
		return vShader;
	}
	IC void	Release()
	{
		VB_Destroy();
		IB_Destroy();
	}
};
