#include "stdafx.h"
#include "fstaticrender.h"
#include "fcached.h"

const DWORD	v_limit			= 2048;

// Render of cached meshes
void __fastcall render_Cached(CList<FCached*>& cache)
{
	CVertexStream*			vs	= cache[0]->VS;
	CIndexStream*			is	= Device.Streams.Get_IB();
	DWORD dwPassesRequired		= Device.Shader.dwPassesRequired;

	for (DWORD Start=0; Start<cache.size(); )
	{
		// Calculate lock parameters
		DWORD	v_count=0,i_count=0,t;
		for (DWORD End=Start; End<cache.size(); End++)
		{
			FCached& V	=	*(cache[End]);
			if (V.VS!=vs)		break;
			t			=	v_count+V.vCount;
			if (t>v_limit)		break;
			v_count		=	t;
			i_count		+=	V.iCount;
		}

		// Transfer geometry
		DWORD	vBase,iBase;
		DWORD	Stride	= vs->Stride();
		BYTE*	verts	= LPBYTE(vs->Lock(v_count,vBase));
		WORD*	indices	= LPWORD(is->Lock(i_count,iBase));
		DWORD	iOffset	= 0;
		Fbox	bb; bb.invalidate();
		for (DWORD I=Start; I!=End; I++)
		{
			FCached& V	=	*(cache[I]);
			PSGP.transfer	(
				verts,	V.pVertices,V.vCount,Stride,
				indices,V.pIndices,	V.iCount,iOffset,
				0);
			verts		+=	V.vCount*Stride;
			indices		+=	V.iCount;
			iOffset		+=	V.vCount;
			bb.merge	(V.bv_BBox);
		}
		vs->Unlock		(v_count);
		is->Unlock		(i_count);

		// Render
		Fsphere	S;		bb.getsphere	(S.P,S.R);
		::Render.Lights.Select			(S.P,S.R);

		Device.Primitive.setVertices	(vs->getFVF(),Stride,vs->getBuffer());
		Device.Primitive.setIndicesUC	(vBase, is->getBuffer());
		DWORD	dwNumPrimitives			= i_count/3;
		for (DWORD dwPass = 0; dwPass<dwPassesRequired; dwPass++)
		{
			Device.Shader.SetupPass		(dwPass);
			Device.Primitive.Render		(D3DPT_TRIANGLELIST,0,v_count,iBase,dwNumPrimitives);
		}
		UPDATEC(v_count,dwNumPrimitives,dwPassesRequired);

		Start = End;
	}
}
