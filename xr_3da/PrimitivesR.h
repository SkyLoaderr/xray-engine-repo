// PrimitivesR.h: interface for the CPrimitivesR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRIMITIVESR_H__1213C75A_5F4A_4121_9FB2_6FBFCFDA84B9__INCLUDED_)
#define AFX_PRIMITIVESR_H__1213C75A_5F4A_4121_9FB2_6FBFCFDA84B9__INCLUDED_
#pragma once

#include "primitive.h"
#include "sharedprimitive.h"
#include "fvf.h"

class ENGINE_API CDraw
{
	friend class			CRender;

	CVertexStream*			vsTL;
	FVF::TL*				pStart;
	FVF::TL*				pCurrent;
	DWORD					dwLN_Offset;

	IDirect3DVertexBuffer8*	pCurVB;
	IDirect3DIndexBuffer8*	pCurIB;
	DWORD					vCurShader;
public:
	// Tight interface
	IDirect3DIndexBuffer8*	&CurrentIB()	{ return pCurIB; }
	IDirect3DVertexBuffer8*	&CurrentVB()	{ return pCurVB; }

	// Main interface
	IC void setVertices		(DWORD FVF,  DWORD STRIDE, IDirect3DVertexBuffer8* VB)
	{
		if (FVF!=vCurShader)HW.pDevice->SetVertexShader(vCurShader=FVF);
		if (VB!=pCurVB)		HW.pDevice->SetStreamSource(0,pCurVB=VB,STRIDE);
	}
	IC void setIndices		(DWORD BASE, IDirect3DIndexBuffer8* IB)
	{
		if (IB!=pCurIB)		HW.pDevice->SetIndices(pCurIB=IB,BASE);
	}
	IC void setIndicesUC	(DWORD BASE, IDirect3DIndexBuffer8* IB)
	{
		HW.pDevice->SetIndices	(pCurIB=IB,BASE);
	}
	IC void Render			(D3DPRIMITIVETYPE T, DWORD SV, DWORD CV, DWORD SI, DWORD PC)
	{	HW.pDevice->DrawIndexedPrimitive(T,SV,CV,SI,PC);	}
	IC void Render			(D3DPRIMITIVETYPE T, DWORD SV, DWORD PC)
	{	HW.pDevice->DrawPrimitive(T,	SV, PC);			}
	
	IC void Draw			(CPrimitive& P,		DWORD dwNumVerts, DWORD dwNumPrimitives);
	IC void Draw			(CVertexStream* S,	DWORD dwNumVerts, DWORD dwNumPrimitives, DWORD dwBase, IDirect3DIndexBuffer8* IB);
	IC void Draw			(CVertexStream* S,	DWORD dwNumPrimitives, DWORD dwBase);	// non indexed
	IC void DrawNI_SP		(CPrimitive& P, DWORD dwStartVert,	DWORD dwNumPrimitives);
	IC void DrawSubset		(CPrimitive& P, DWORD dwStartVertex,DWORD dwNumVerts, DWORD dwStartIndex, DWORD dwNumPrimitives);
	IC void Reset			();

	void Lines_Begin		(int count);
	void Lines_Draw			(Fvector& P1, Fvector& P2, float width, DWORD C);
	void Lines_End			();

	// Device create / destroy
	void					OnDeviceCreate	();
	void					OnDeviceDestroy	();

	// Debug render
	IC void dbg_Draw		(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, WORD* pIdx, int pcnt);
	IC void dbg_Draw		(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);
	IC void dbg_DrawAABB	(Fvector& T, float sx, float sy, float sz, DWORD C)
	{	Fvector half_dim;	half_dim.set(sx,sy,sz); Fmatrix	TM;	TM.translate(T);
		dbg_DrawOBB(TM,half_dim,C);	}
	IC void dbg_DrawOBB		(Fmatrix& T, Fvector& half_dim, DWORD C);
	IC void dbg_DrawTRI		(Fmatrix& T, Fvector* p, DWORD C)
	{ dbg_DrawTRI(T,p[0],p[1],p[2],C);	}
	IC void dbg_DrawTRI		(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, DWORD C);
	IC void dbg_DrawLINE	(Fmatrix& T, Fvector& p1, Fvector& p2, DWORD C);
	IC void dbg_DrawEllipse	(Fmatrix& T, DWORD C);

	CDraw()	: pCurVB(0), pCurIB(0), vCurShader(0) {};
};

#endif // !defined(AFX_PRIMITIVESR_H__1213C75A_5F4A_4121_9FB2_6FBFCFDA84B9__INCLUDED_)
