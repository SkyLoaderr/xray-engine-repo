#include "stdafx.h"
#include "detailmodel.h"
#include "xrstripify.h"

CDetail::~CDetail()
{
}

void CDetail::Load		(IReader* S)
{
	// Shader
	string256		fnT,fnS;
	S->r_stringZ		(fnS);
	S->r_stringZ		(fnT);
	shader			= Device.Shader.Create(fnS,	fnT);

	// Params
	flags			= S->r_u32	();
	s_min			= S->r_float();
	s_max			= S->r_float();
	number_vertices	= S->r_u32	();
	number_indices	= S->r_u32	();
	R_ASSERT		(0==(number_indices%3));
	
	// Vertices
	u32				size_vertices		= number_vertices*sizeof(fvfVertexIn); 
	vertices		= (CDetail::fvfVertexIn *)	xr_malloc	(size_vertices);
	S->r			(vertices,size_vertices);
	
	// Indices
	u32				size_indices		= number_indices*sizeof(WORD);
	indices			= (WORD*)					xr_malloc	(size_indices);
	S->r			(indices,size_indices);
	
	// Validate indices
	for (u32 idx = 0; idx<number_indices; idx++)
		R_ASSERT	(indices[idx]<number_vertices);
	
	// Calc BB & SphereRadius
	bv_bb.invalidate	();
	for (u32 i=0; i<number_vertices; i++)
		bv_bb.modify	(vertices[i].P);
	bv_bb.getsphere		(bv_sphere.P,bv_sphere.R);

	Optimize	();
}

void CDetail::Optimize	()
{
	vector<WORD>		vec_indices, vec_permute;
	const int			cache	= HW.Caps.vertex.dwVertexCache;

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
		Memory.mem_copy		(indices,&*vec_indices.begin(),vec_indices.size()*sizeof(WORD));

		// Permute vertices
		vector<fvfVertexIn>	verts	(vertices,vertices+number_vertices);
		for(u32 i=0; i<verts.size(); i++)
			vertices[i]=verts[vec_permute[i]];
	}
}

void CDetail::Unload	()
{
	if (vertices)		{ xr_free(vertices);	vertices=0; }
	if (indices)		{ xr_free(indices);	indices=0;	}
	Device.Shader.Delete(shader);
}

void CDetail::Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, WORD* iDest, u32 iOffset)
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
		u32	item	= (iOffset<<16) | iOffset;
		u32	count	= number_indices/2;
		LPDWORD	sit		= LPDWORD(indices);
		LPDWORD	send	= sit+count;
		LPDWORD	dit		= LPDWORD(iDest);
		for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
		if		(number_indices&1)	
			iDest[number_indices-1]=indices[number_indices-1]+WORD(iOffset);
	}
}
