#include "stdafx.h"
#include "..\flod.h"

extern float r_ssaLOD_A;
extern float r_ssaLOD_B;

#define RGBA_GETALPHA(rgb)      u32((rgb) >> 24)
#define RGBA_GETRED(rgb)        u32(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      u32(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       u32((rgb) & 0xff)

IC u32	color		(u32 Base, u32 Alpha)
{
	return D3DCOLOR_RGBA(RGBA_GETRED(Base),RGBA_GETGREEN(Base),RGBA_GETBLUE(Base),Alpha);
}

void CRender::flush_LODs()
{
	mapLOD.getRL				(lstLODs);
	if (lstLODs.empty())		return;

	// *** Fill VB and generate groups
	IRender_Visual*					firstV		= lstLODs[0].pVisual;
	Shader*						cur_S		= firstV->hShader;
	int							cur_count	= 0;
	u32							vOffset;
	FVF::LIT*					V	= (FVF::LIT*)RCache.Vertex.Lock	(lstLODs.size()*4,firstV->hGeom->vb_stride, vOffset);
	float	ssaRange				= r_ssaLOD_A - r_ssaLOD_B;
	for (u32 i=0; i<lstLODs.size(); i++)
	{
		// sort out redundancy
		SceneGraph::_LodItem		&P = lstLODs[i];
		if (P.pVisual->hShader==cur_S)	cur_count++;
		else {
			vecGroups.push_back	(cur_count);
			cur_S				= P.pVisual->hShader;
			cur_count			= 1;
		}

		// calculate alpha
		float	ssaDiff					= P.ssa-r_ssaLOD_B;
		float	scale					= ssaDiff/ssaRange;
		int		iA						= iFloor((1-scale)*255.f);	clamp(iA,0,255);
		u32	uA						= u32(iA);
		// float	shift_scale				= scale;					clamp(shift_scale,0.f,1.f);

		// calculate direction and shift
		FLOD*							lodV		= (FLOD*)P.pVisual;
		Fvector							Ldir,shift,_P;
		Ldir.sub						(lodV->vis.sphere.P,Device.vCameraPosition);
		Ldir.normalize					();
		shift.mul						(Ldir,-.5f * lodV->vis.sphere.R);

		// gen geometry
		FLOD::_face*					facets		= lodV->facets;
		int								best_id		= 0;
		float							best_dot	= Ldir.dotproduct(facets[0].N);
		float							dot;

		dot	= Ldir.dotproduct			(facets[1].N); if (dot>best_dot) { best_id=1; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[2].N); if (dot>best_dot) { best_id=2; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[3].N); if (dot>best_dot) { best_id=3; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[4].N); if (dot>best_dot) { best_id=4; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[5].N); if (dot>best_dot) { best_id=5; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[6].N); if (dot>best_dot) { best_id=6; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[7].N); if (dot>best_dot) { best_id=7; best_dot=dot; }

		// Fill VB
		FLOD::_face&	F				= facets[best_id];
		_P.add(F.v[3].v,shift);	V->set	(_P,color(F.v[3].c,uA),F.v[3].t.x,F.v[3].t.y); V++;	// 3
		_P.add(F.v[0].v,shift);	V->set	(_P,color(F.v[0].c,uA),F.v[0].t.x,F.v[0].t.y); V++;	// 0
		_P.add(F.v[2].v,shift);	V->set	(_P,color(F.v[2].c,uA),F.v[2].t.x,F.v[2].t.y); V++;	// 2
		_P.add(F.v[1].v,shift);	V->set	(_P,color(F.v[1].c,uA),F.v[1].t.x,F.v[1].t.y); V++;	// 1
	}
	vecGroups.push_back				(cur_count);
	RCache.Vertex.Unlock			(lstLODs.size()*4,firstV->hGeom->vb_stride);

	// *** Render
	int current=0;
	for (u32 g=0; g<vecGroups.size(); g++)
	{
		int p_count				= vecGroups[g];
		RCache.set_Shader		(lstLODs[current].pVisual->hShader);
		RCache.set_Geometry		(firstV->hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4*p_count,0,2*p_count);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	mapLOD.clear	();
	lstLODs.clear	();
	vecGroups.clear	();
}
