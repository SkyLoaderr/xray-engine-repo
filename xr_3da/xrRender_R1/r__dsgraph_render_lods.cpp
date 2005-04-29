#include "stdafx.h"
#include "flod.h"

#ifdef _EDITOR
#include "igame_persistent.h"
#include "environment.h"
#else
#include "..\igame_persistent.h"
#include "..\environment.h"
#endif

extern float r_ssaLOD_A;
extern float r_ssaLOD_B;

IC u32	color					(Fvector& N, u32 rgbh_1, u32 rgbh_2, u8 sun_1, u8 sun_2, float lerpf, u32 Alpha)
{
	CEnvDescriptor&	desc		= g_pGamePersistent->Environment.CurrentEnv;
	Fvector						c_sun,c_ambient,c_lmap,c_hemi,c_sun_dir;
	c_sun_dir.set				(desc.sun_dir);
	c_sun.set					(desc.sun_color.x,	desc.sun_color.y,	desc.sun_color.z);
	c_lmap.set					(desc.lmap_color.x,	desc.lmap_color.y,	desc.lmap_color.z);
	c_ambient.set				(desc.ambient.x,	desc.ambient.y,		desc.ambient.z);
	c_hemi.set					(desc.hemi_color.x, desc.hemi_color.y,	desc.hemi_color.z);
	float	sun_factor			= .5f * clampr	(c_sun_dir.dotproduct(N),0.f,1.f);	//. hack to immitate sun

	Fvector C,rgb,rgb_1,rgb_2; 
	float	divisor				= 255.f;
	rgb_1.set					(float(color_get_R(rgbh_1)),float(color_get_G(rgbh_1)),float(color_get_B(rgbh_1)));
	rgb_2.set					(float(color_get_R(rgbh_2)),float(color_get_G(rgbh_2)),float(color_get_B(rgbh_2)));
	rgb.lerp					(rgb_1,rgb_2,lerpf).div(divisor);
	float h_1					= float(color_get_A(rgbh_1));
	float h_2					= float(color_get_A(rgbh_2));
	float h						= (h_1*(1.f-lerpf) + h_2*lerpf) /divisor;
	float s_1					= float(sun_1);
	float s_2					= float(sun_2);
	float s						= (s_1*(1.f-lerpf) + s_2*lerpf) /divisor;
	C.set						(c_ambient	);
	C.mad						(c_lmap,rgb	);
	C.mad						(c_hemi,h	);
	C.mad						(c_sun,	s + sun_factor);

	return subst_alpha			(color_rgba_f(C.x,C.y,C.z,0),Alpha);
}

ICF		bool	pred_dot		(std::pair<float,u32>& _1, std::pair<float,u32>& _2)	{ return _1.first < _2.first; }
void R_dsgraph_structure::r_dsgraph_render_lods	()
{
	mapLOD.getRL				(lstLODs);
	if (lstLODs.empty())		return;
 
	// *** Fill VB and generate groups
	FLOD*						firstV		= (FLOD*)lstLODs[0].pVisual;
	ref_shader					cur_S		= &*firstV->shader;
	int							cur_count	= 0;
	u32							vOffset;
	FLOD::_hw*					V			= (FLOD::_hw*)RCache.Vertex.Lock	(lstLODs.size()*4,firstV->geom->vb_stride, vOffset);
	float	ssaRange						= r_ssaLOD_A - r_ssaLOD_B;
	if		(ssaRange<EPS_S)	ssaRange	= EPS_S;
	for (u32 i=0; i<lstLODs.size(); i++)
	{
		// sort out redundancy
		R_dsgraph::_LodItem		&P = lstLODs[i];
		if (P.pVisual->shader==cur_S)	cur_count++;
		else {
			lstLODgroups.push_back	(cur_count);
			cur_S				= &*(P.pVisual->shader);
			cur_count			= 1;
		}

		// calculate alpha
		float	ssaDiff					= P.ssa-r_ssaLOD_B;
		float	scale					= ssaDiff/ssaRange;
		int		iA						= iFloor((1-scale)*255.f);	clamp(iA,0,255);
		u32		uA						= u32(iA);
		// float	shift_scale			= scale;					clamp(shift_scale,0.f,1.f);

		// calculate direction and shift
		FLOD*							lodV		= (FLOD*)P.pVisual;
		Fvector							Ldir,shift,_P;
		Ldir.sub						(lodV->vis.sphere.P,Device.vCameraPosition);
		Ldir.normalize					();
		shift.mul						(Ldir,-.5f * lodV->vis.sphere.R);

		// gen geometry
		FLOD::_face*					facets		= lodV->facets;
		svector<std::pair<float,u32>,8>	selector	;
		for (u32 s=0; s<8; s++)			selector.push_back(mk_pair(Ldir.dotproduct(facets[s].N),s));
		std::sort						(selector.begin(),selector.end(),pred_dot);

		float							dot_best	= selector	[selector.size()-1].first	;
		float							dot_next	= selector	[selector.size()-2].first	;
		float							dot_next_2	= selector	[selector.size()-3].first	;
		u32								id_best		= selector	[selector.size()-1].second	;
		u32								id_next		= selector	[selector.size()-2].second	;

		// Now we have two "best" planes, calculate factor, and approx normal
		float	fA = dot_best, fB = dot_next, fC = dot_next_2;
		float	alpha	=	0.5f + 0.5f*(1-(fB-fC)/(fA-fC))	;
		u32		factor	=	iFloor		(alpha*255.5f)		;	clamp	(factor,u32(0),u32(255));
				factor	=	color_rgba	(factor,factor,factor,factor);

		// Fill VB
		FLOD::_face&	FA				= facets[id_best]	;
		FLOD::_face&	FB				= facets[id_next]	;
		Fvector	N;		N.lerp			(FB.N, FA.N, alpha).normalize();
		N.y	+=	1;		N.normalize		();
		_P.lerp(FB.v[3].v,FA.v[3].v,alpha).add(shift);	V->set	(_P,N,color(N,FB.v[3].c_rgb_hemi,FA.v[3].c_rgb_hemi,FB.v[3].c_sun,FA.v[3].c_sun,alpha,uA),factor,FB.v[3].t.x,FB.v[3].t.y,FA.v[3].t.x,FA.v[3].t.y); V++;	// 3
		_P.lerp(FB.v[0].v,FA.v[0].v,alpha).add(shift);	V->set	(_P,N,color(N,FB.v[0].c_rgb_hemi,FA.v[0].c_rgb_hemi,FB.v[0].c_sun,FA.v[0].c_sun,alpha,uA),factor,FB.v[0].t.x,FB.v[0].t.y,FA.v[0].t.x,FA.v[0].t.y); V++;	// 0
		_P.lerp(FB.v[2].v,FA.v[2].v,alpha).add(shift);	V->set	(_P,N,color(N,FB.v[2].c_rgb_hemi,FA.v[2].c_rgb_hemi,FB.v[2].c_sun,FA.v[2].c_sun,alpha,uA),factor,FB.v[2].t.x,FB.v[2].t.y,FA.v[2].t.x,FA.v[2].t.y); V++;	// 2
		_P.lerp(FB.v[1].v,FA.v[1].v,alpha).add(shift);	V->set	(_P,N,color(N,FB.v[1].c_rgb_hemi,FA.v[1].c_rgb_hemi,FB.v[1].c_sun,FA.v[1].c_sun,alpha,uA),factor,FB.v[1].t.x,FB.v[1].t.y,FA.v[1].t.x,FA.v[1].t.y); V++;	// 1
	}
	lstLODgroups.push_back				(cur_count);
	RCache.Vertex.Unlock				(lstLODs.size()*4,firstV->geom->vb_stride);

	// *** Render
	int current=0;
	RCache.set_xform_world		(Fidentity);
	for (u32 g=0; g<lstLODgroups.size(); g++)	{
		int p_count				= lstLODgroups[g];
		RCache.set_Shader		(lstLODs[current].pVisual->shader);
		RCache.set_Geometry		(firstV->geom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4*p_count,0,2*p_count);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	mapLOD.clear		();
	lstLODs.clear		();
	lstLODgroups.clear	();
}
