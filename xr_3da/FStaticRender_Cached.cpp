#include "stdafx.h"
#include "fstaticrender.h"
#include "fcached.h"
#include "fmesh.h"

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
		vs				=	cache[Start]->VS;

		// Transfer geometry
		DWORD	vBase,iBase;
		DWORD	Stride	= vs->Stride();
		BYTE*	verts	= LPBYTE(vs->Lock(v_count,vBase));
		WORD*	indices	= LPWORD(is->Lock(i_count,iBase));
		
		WORD*	test_indices	= indices;

		DWORD	iOffset	= 0;
		Fbox	bb; bb.invalidate();
		for (DWORD I=Start; I!=End; I++)
		{
			FCached& V	=	*(cache[I]);
			
			// Transfer vertices
			CopyMemory	(verts,V.pVertices,V.vCount*Stride);
			
			// Transfer indices (in 32bit lines)
			/*
			VERIFY	(iOffset<65535);
			{
				DWORD	iCount	= V.iCount;
				DWORD	item	= (iOffset<<16) | iOffset;
				DWORD	count	= iCount/2;
				LPDWORD	sit		= LPDWORD(V.pIndices);
				LPDWORD	send	= sit+count;
				LPDWORD	dit		= LPDWORD(indices);
				for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
				if		(iCount&1)	indices[iCount-1] = V.pIndices[iCount-1]+WORD(iOffset);
			}
			*/
			{
				WORD*	iDest	= indices;
				WORD*	iSrc	= V.pIndices;
				WORD*	iSrc_End= iSrc+V.iCount;
				WORD	offs	= WORD(iOffset);
				for (; iSrc!=iSrc_End; iSrc++, iDest++)
					*iDest = *iSrc + offs;
			}

			// Increment counters
			verts		+=	V.vCount*Stride;
			indices		+=	V.iCount;
			iOffset		+=	V.vCount;
			bb.merge	(V.bv_BBox);
		}
		R_ASSERT(ValidateIndices(v_count,i_count,test_indices));

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
