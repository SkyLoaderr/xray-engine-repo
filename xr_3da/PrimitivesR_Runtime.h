#ifndef _PRIMITIVER_RUNTIME_H_
#define _PRIMITIVER_RUNTIME_H_

#pragma once

// The most used wrappers --- recomendation DON'T USE AT ALL :)
/*
IC void CDraw::Draw			(CPrimitive& P,		u32 dwNumVerts, u32 dwNumPrimitives)
{
	setVertices	(P.vShader,P.vSize,P.pVertices);
	setIndices	(P.dwBaseVertex,P.pIndices);
	Render		(D3DPT_TRIANGLELIST,0,dwNumVerts,0,dwNumPrimitives);
	UPDATEC		(dwNumVerts,dwNumPrimitives,1);
}
IC void CDraw::Draw			(CVS* S,	u32 dwNumVerts, u32 dwNumPrimitives, u32 dwBase, IDirect3DIndexBuffer8* IB)
{
	setVertices	(S->dwHandle,S->dwStride,Device.Streams.Vertex.getBuffer());
	setIndices	(dwBase,IB);;
	Render		(D3DPT_TRIANGLELIST,0,dwNumVerts,0,dwNumPrimitives);
	UPDATEC		(dwNumVerts,dwNumPrimitives,1);
}
IC void CDraw::Draw			(CVS* S,	u32 dwNumPrimitives, u32 dwBase)
{
	setVertices	(S->mFVF,S->mStride,S->pVB);
	setIndices	(0,0);
	Render		(D3DPT_TRIANGLELIST,dwBase,dwNumPrimitives);
	UPDATEC		(dwNumPrimitives*3,dwNumPrimitives,1);
}
IC void CDraw::Draw			(CVertexStream* S,  u32 dwNumVerts, u32 dwNumPrimitives, u32 vBase, CIndexStream* IS, u32 iBase)
{
	setVertices	(S->mFVF,S->mStride,S->pVB);
	setIndices	(vBase, IS->getBuffer());
	Render		(D3DPT_TRIANGLELIST,0,dwNumVerts,iBase,dwNumPrimitives);
	UPDATEC		(dwNumVerts,dwNumPrimitives,1);
}
*/
/*
IC void CDraw::DrawSubset	(CPrimitive& P, u32 dwStartVertex,u32 dwNumVerts, u32 dwStartIndex, u32 dwNumPrimitives)
{
	setVertices	(P.vShader,P.vSize,P.pVertices);
	setIndices	(P.dwBaseVertex,P.pIndices);
	Render		(D3DPT_TRIANGLELIST,dwStartVertex,dwNumVerts,dwStartIndex,dwNumPrimitives);
	UPDATEC		(dwNumVerts,dwNumPrimitives,1);
}
*/
#endif