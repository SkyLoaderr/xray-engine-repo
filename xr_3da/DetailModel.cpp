#include "stdafx.h"
#include "detailmodel.h"

void CDetail::Load		(CStream* S)
{
	// Shader
	FILE_NAME		fnT,fnS;
	S->RstringZ		(fnS);
	S->RstringZ		(fnT);
	shader			= Device.Shader.Create(fnS,fnT);

	// Params
	flags			= S->Rdword	();
	s_min			= S->Rfloat	();
	s_max			= S->Rfloat	();
	number_vertices	= S->Rdword	();
	number_indices	= S->Rdword	();
	R_ASSERT		(0==(number_indices%3));
	
	// Vertices
	DWORD			size_vertices		= number_vertices*sizeof(fvfVertexIn); 
	vertices		= (CDetail::fvfVertexIn *)	_aligned_malloc	(size_vertices,64);
	S->Read			(vertices,size_vertices);
	
	// Indices
	DWORD			size_indices		= number_indices*sizeof(WORD);
	indices			= (WORD*)					_aligned_malloc	(size_indices,64);
	S->Read			(indices,size_indices);
	
	// Calc BB & SphereRadius
	Fbox bb;
	bb.invalidate	();
	for (DWORD i=0; i<number_vertices; i++)
		bb.modify	(vertices[i].P);
	radius			= bb.getradius()*2;
}

void CDetail::Unload	()
{
	if (vertices)		{ _aligned_free(vertices);	vertices=0; }
	if (indices)		{ _aligned_free(indices);	indices=0;	}
	Device.Shader.Delete(shader);
}

void CDetail::Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, DWORD C, WORD* iDest, DWORD iOffset)
{
	// Transfer vertices
	{
		CDetail::fvfVertexIn	*srcIt = vertices, *srcEnd = vertices+number_vertices;
		CDetail::fvfVertexOut	*dstIt = vDest;
		for	(; srcIt!=srcEnd; srcIt++, dstIt++)
		{
			mXform.transform_tiny	(dstIt->P,srcIt->P);
			dstIt->C	= C;
			dstIt->u	= srcIt->u;
			dstIt->v	= srcIt->v;
		}
	}
	
	// Transfer indices (in 32bit lines)
	VERIFY	(iOffset<65535);
	{
		DWORD	item	= (iOffset<<16) | iOffset;
		DWORD	count	= number_indices/2;
		LPDWORD	sit		= LPDWORD(indices);
		LPDWORD	send	= sit+count;
		LPDWORD	dit		= LPDWORD(iDest);
		for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
		if		(number_indices&1)	
			iDest[number_indices-1]=indices[number_indices-1]+WORD(iOffset);
	}
}

