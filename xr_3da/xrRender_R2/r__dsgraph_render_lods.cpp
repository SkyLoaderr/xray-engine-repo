#include "stdafx.h"
#include "..\flod.h"

#ifdef _EDITOR
#include "igame_persistent.h"
#include "environment.h"
#else
#include "..\igame_persistent.h"
#include "..\environment.h"
#endif

extern float r_ssaLOD_A;
extern float r_ssaLOD_B;
enIC u32	color					(Fvector& N, u32 rgbh, u8 sun, u32 Alpha)
{
	CEnvDescriptor&	desc		= g_pGamePersistent->Environment.CurrentEnv;
	Fvector						c_sun,c_ambient,c_lmap,c_hemi,c_sun_dir;
	c_sun_dir.set				(desc.sun_dir).invert	();
	c_sun.set					(desc.sun_color.x,	desc.sun_color.y,	desc.sun_color.z);
	c_lmap.set					(desc.lmap_color.x,	desc.lmap_color.y,	desc.lmap_color.z);
	c_ambient.set				(desc.ambient.x,	desc.ambient.y,		desc.ambient.z);
	c_hemi.set					(desc.hemi_color.x, desc.hemi_color.y,	desc.hemi_color.z);
	float	sun_factor			= .333f * clampr	(c_sun_dir.dotproduct(N),0,1);	//. hack to immitate sun
C,	Fvector C,rgb; 
			(float(color_get_R(rgbh)),float(color_get_G(rgbh)),float(color_get_B(rgbh)));
	rgb.div				rgb.div						(255.f);
	float h						= float(color_get_A(rgbh))/255.f;
	float s						= float(sun)/255.f;
	(c_ambient);
	C.mad						(c_lmap,rgb);
	C.mad						(c_hemi,h);
	C.mad						(c_sun,	s + sun_factor);

	return s	C.mul						(1.1f);		//. hack to make lods look better/brighter - +10% total energy
ub	return subst_alpha			(color_rgba_f(C.x,C.y,C.z,0),Alpha);
_dsgraph_structure::r_dsgraph_render_lods	()
{
	mapLOD.getRL				(lstLODs);
	if (lstLODs.empty())		return;

	// *** Fill VB and generate groups
	IRender_Visual*				firstV		= lstLODs[0].pVisual;
	ref_shader					cur_S		= &*firstV->hShader;
	int							cur_count	= 0;
	u32							vOffset;
	FVF::LIT*		FVF::LIT*					V			= (FVF::LIT*)RCache.Vertex.Lock	(lstLODs.size()*4,firstV->hGeom->vb_stride, vOffset);
ange						= r_ssaLOD_A - r_ssaLOD_B;
	for (u32 i=0; i<lstLODs.size(); i++)
	{
		// sort out redundancy
		R_dsgraph::_LodItem		&P = lstLODs[i];
		if (P.pVisual->hShader==cur_S)	cur_count++;
		else {
			lstLODgroups.push_back	(cur_count);
			cur_S				= &*(P.pVisual->hShader);
			cur_count			= 1;
		}

		// calculate alpha
		float	ssaDiff					= P.ssa-r_ssaLOD_B;
		float	scale					= ssaDiff/ssaRange;
		int		iA						= iFloor((1-scale)*255.f);	clamp(iA,0,255);
		u32		uA						= u32(iA);
		// float			// float	shift_scale				= scale;					clamp(shift_scale,0.f,1.f);
ulate direction and shift
		FLOD*							lodV		= (FLOD*)P.pVisual;
		Fvector							Ldir,shift,_P;
		Ldir.sub						(lodV->vis.sphere.P,Device.vCameraPosition);
		Ldir.normalize					();
		shift.mul						(Ldir,-.5f * lodV->vis.sphere.R);

		// gen geometry
		FLOD::_face*					facets		= lodV->facets;
		int								id_best		= 0;
		int								id_next		= 0;
		float						float							dot_best	= Ldir.dotproduct(facets[0].N);
		float							dot_next	= Ldir.dotproduct(facets[0].N);
			dot;

		dot	= Ldir.dotproduct			(facets[1].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=1; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[2].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=2; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[3].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=3; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[4].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=4; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[5].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=5; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[6].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=6; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[7].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=7; dot_best=dot; }

		// Now we have two "best" planes, calculate factor, and approx normal
		float	f0			float	f0	=	dot_best,	f1 = dot_next;
=	f0+f1;		f0 /= ft; f1 /= ft;
		Fvector	N		Fvector	N	=	{0,0,0};	N.mad(facets[id_best],f0).mad(facets[id_next],f1).normalize();
1;			N.normalize	();

		// Fill VB
		FLOD::_fa		FLOD::_face&	F				= facets[id_best];
		_P.add(F.v[3].v,shift);	V->set	(_P,color(N,F.v[3].c_rgb_hemi,F.v[3].c_sun,uA),F.v[3].t.x,F.v[3].t.y); V++;	// 3
		_P.add(F.v[0].v,shift);	V->set	(_P,color(N,F.v[0].c_rgb_hemi,F.v[0].c_sun,uA),F.v[0].t.x,F.v[0].t.y); V++;	// 0
		_P.add(F.v[2].v,shift);	V->set	(_P,color(N,F.v[2].c_rgb_hemi,F.v[2].c_sun,uA),F.v[2].t.x,F.v[2].t.y); V++;	// 2
		_P.add(F.v[1].v,shift);	V->set	(_P,color(N,F.v[1].c_rgb_hemi,F.v[1].c_sun,uA),F.v[1].t.x,F.v[1].t.y); V++;	// 1
groups.push_back				(cur_count);
	RCache.Vertex.Unlock				(lstLODs.size()*4,firstV->hGeom->vb_stride);

	// *** Render
	int curren	int current=0;
_xform_world		(Fidentity);
	for (u32 g=0; g<lstLODgroups.size(); g++)	{
		int p_count				= lstLODgroups[g];
		RCache.set_Shader		(lstLODs[current].pVisual->hShader);
		RCache.set_Geometry		(firstV->hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4*p_count,0,2*p_count);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	mapLOD.clear		();
	lstLODs.clear		();
	lstLODgroups.clear	();
}
