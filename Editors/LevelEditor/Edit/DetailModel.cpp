#include "stdafx.h"
#include "detailmodel.h"
#include "xrstripify.h"

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
	
	// Validate indices
	for (u32 idx = 0; idx<number_indices; idx++)
		R_ASSERT	(indices[idx]<number_vertices);
	
	// Calc BB & SphereRadius
	bv_bb.invalidate	();
	for (DWORD i=0; i<number_vertices; i++)
		bv_bb.modify	(vertices[i].P);
	bv_bb.getsphere		(bv_sphere.P,bv_sphere.R);

	Optimize	();
}

void CDetail::Optimize	()
{
	vector<WORD>		vec_indices, vec_permute;
	const int			cache	= 14;

	// Stripify
	vec_indices.assign	(indices,indices+number_indices);
	vec_permute.resize	(number_vertices);
	int vt_old			= xrSimulate(vec_indices,cache);
	xrStripify			(vec_indices,vec_permute,cache,0);
	int vt_new			= xrSimulate(vec_indices,cache);
	if (vt_new<vt_old)	
	{
		Msg					("DM: %d verts, %d indices, VT: %d/%d",number_vertices,number_indices,vt_old,vt_new);

		// Copy faces
		PSGP.memCopy		(indices,&*vec_indices.begin(),vec_indices.size()*sizeof(WORD));

		// Permute vertices
		vector<fvfVertexIn>	verts	(vertices,vertices+number_vertices);
		for(DWORD i=0; i<verts.size(); i++)
			vertices[i]=verts[vec_permute[i]];
	}
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

