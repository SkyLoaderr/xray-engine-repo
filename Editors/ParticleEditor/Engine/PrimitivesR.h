// PrimitivesR.h: interface for the CPrimitivesR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRIMITIVESR_H__1213C75A_5F4A_4121_9FB2_6FBFCFDA84B9__INCLUDED_)
#define AFX_PRIMITIVESR_H__1213C75A_5F4A_4121_9FB2_6FBFCFDA84B9__INCLUDED_
#pragma once

//#include "primitive.h"
#include "sharedprimitive.h"
#include "fvf.h"

class ENGINE_API CDraw
{
	IDirect3DVertexBuffer8*	pCurVB;
	IDirect3DIndexBuffer8*	pCurIB;
	u32					vCurShader;
	u32					vCurBase;
	u32					vCurStride;

public:
	u32					stat_polys;
	u32					stat_verts;
	u32					stat_calls;
	u32					stat_vs;
	u32					stat_vb;
	u32					stat_ib;
public:
	// Tight interface
	IDirect3DIndexBuffer8*	&CurrentIB()	{ return pCurIB; }
	IDirect3DVertexBuffer8*	&CurrentVB()	{ return pCurVB; }
	
	// Main interface
	IC void setVertices		(u32 vs,  u32 STRIDE, IDirect3DVertexBuffer8* VB)
	{
		if (vs!=vCurShader)	
		{
			stat_vs++;
			HW.pDevice->SetVertexShader(vCurShader=vs);
		}
		if (VB!=pCurVB)
		{
			stat_vb++;
			HW.pDevice->SetStreamSource(0,pCurVB=VB,vCurStride=STRIDE);
		} else {
			if (STRIDE!=vCurStride)
				HW.pDevice->SetStreamSource(0,pCurVB=VB,vCurStride=STRIDE);
		}
	}
	IC void setIndices		(u32 BASE, IDirect3DIndexBuffer8* IB)
	{
		if (IB!=pCurIB)
		{
			stat_ib++;
			HW.pDevice->SetIndices(pCurIB=IB,vCurBase=BASE);
		} else {
			if (BASE!=vCurBase)
				HW.pDevice->SetIndices(pCurIB=IB,vCurBase=BASE);
		}
	}
	IC void Render			(D3DPRIMITIVETYPE T, u32 SV, u32 CV, u32 SI, u32 PC)
	{	
		stat_calls			++;
		stat_verts			+= CV;
		stat_polys			+= PC;		
		HW.pDevice->DrawIndexedPrimitive(T,SV,CV,SI,PC);	
	}
	IC void Render			(D3DPRIMITIVETYPE T, u32 SV, u32 PC)
	{	
		stat_calls			++;
		stat_verts			+= 3*PC;
		stat_polys			+= PC;
		HW.pDevice->DrawPrimitive(T,	SV, PC);			
	}
	IC void Reset			()
	{
		vCurShader					= 0;
		HW.pDevice->SetStreamSource	(0,pCurVB=0,vCurStride=0);
		HW.pDevice->SetIndices		(pCurIB=0,vCurBase=0);
	}
	
	// Device create / destroy
	void					OnDeviceCreate	();
	void					OnDeviceDestroy	();

	// Debug render
	IC void dbg_Draw		(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, WORD* pIdx, int pcnt);
	IC void dbg_Draw		(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);
	IC void dbg_DrawAABB	(Fvector& T, float sx, float sy, float sz, u32 C)
	{	Fvector half_dim;	half_dim.set(sx,sy,sz); Fmatrix	TM;	TM.translate(T);
		dbg_DrawOBB(TM,half_dim,C);	}
	IC void dbg_DrawOBB		(Fmatrix& T, Fvector& half_dim, u32 C);
	IC void dbg_DrawTRI		(Fmatrix& T, Fvector* p, u32 C)
	{ dbg_DrawTRI(T,p[0],p[1],p[2],C);	}
	IC void dbg_DrawTRI		(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C);
	IC void dbg_DrawLINE	(Fmatrix& T, Fvector& p1, Fvector& p2, u32 C);
	IC void dbg_DrawEllipse	(Fmatrix& T, u32 C);

	CDraw()	: pCurVB(0), pCurIB(0), vCurShader(0) {};
};

#endif // !defined(AFX_PRIMITIVESR_H__1213C75A_5F4A_4121_9FB2_6FBFCFDA84B9__INCLUDED_)
