#include "stdafx.h"
#include "flod.h"
#include "fmesh.h"

void FLOD::Load			(LPCSTR N, IReader *data, u32 dwFlags)
{
	inherited::Load		(N,data,dwFlags);

	// 
	Msg		("* %d",children.size());

	// LOD-def
	R_ASSERT			(data->find_chunk(OGF_LODDEF));
	for (int f=0; f<8; f++)
	{
		data->r					(facets[f].v,sizeof(facets[f].v));
		_vertex* v				= facets[f].v;

		Fvector					N,T;
		N.set					(0,0,0);
		T.mknormal				(v[0].v,v[1].v,v[2].v);	N.add	(T);
		T.mknormal				(v[1].v,v[2].v,v[3].v);	N.add	(T);
		T.mknormal				(v[2].v,v[3].v,v[0].v);	N.add	(T);
		T.mknormal				(v[3].v,v[0].v,v[1].v);	N.add	(T);
		N.div					(4.f);
		facets[f].N.normalize	(N);
		facets[f].N.invert		();
	}

	// VS
	hGeom				= Device.Shader.CreateGeom	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer(), RCache.QuadIB);
}
void FLOD::Copy			(CVisual *pFrom	)
{
	inherited::Copy		(pFrom);

	FLOD* F				= (FLOD*)pFrom;
	Memory.mem_copy		(facets,F->facets,sizeof(facets));
}
void FLOD::Render		(float LOD		)
{
	Fvector				Ldir;
	Ldir.sub			(vis.sphere.P,Device.vCameraPosition);
	Ldir.normalize		();

	int					best_id		= 0;
	float				best_dot	= Ldir.dotproduct(facets[0].N);
	float				dot;

	dot	= Ldir.dotproduct	(facets[1].N); if (dot>best_dot) { best_id=1; best_dot=dot; }
	dot	= Ldir.dotproduct	(facets[2].N); if (dot>best_dot) { best_id=2; best_dot=dot; }
	dot	= Ldir.dotproduct	(facets[3].N); if (dot>best_dot) { best_id=3; best_dot=dot; }
	dot	= Ldir.dotproduct	(facets[4].N); if (dot>best_dot) { best_id=4; best_dot=dot; }
	dot	= Ldir.dotproduct	(facets[5].N); if (dot>best_dot) { best_id=5; best_dot=dot; }
	dot	= Ldir.dotproduct	(facets[6].N); if (dot>best_dot) { best_id=6; best_dot=dot; }
	dot	= Ldir.dotproduct	(facets[7].N); if (dot>best_dot) { best_id=7; best_dot=dot; }

	// Fill VB
	_face&		F					= facets[best_id];
	u32			vOffset				= 0;
	FVF::LIT*	V					= (FVF::LIT*) RCache.Vertex.Lock(4,hGeom->vb_stride,vOffset);
	V[0].set	(F.v[0].v,F.v[0].c,F.v[0].t.x,F.v[0].t.y);
	V[1].set	(F.v[1].v,F.v[1].c,F.v[1].t.x,F.v[1].t.y);
	V[2].set	(F.v[2].v,F.v[2].c,F.v[2].t.x,F.v[2].t.y);
	V[3].set	(F.v[3].v,F.v[3].c,F.v[3].t.x,F.v[3].t.y);
	RCache.Vertex.Unlock			(4,hGeom->vb_stride);

	// Draw IT
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLEFAN,vOffset,2);
}
