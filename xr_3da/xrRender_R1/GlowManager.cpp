// GlowManager.cpp: implementation of the CGlowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
#include "..\x_ray.h"
#include "..\GameFont.h"
#include "GlowManager.h"
#include "..\xr_object.h"

#define FADE_SCALE_UP		4096.f
#define FADE_SCALE_DOWN		1024.f
#define MAX_GlowsDist1		float(g_pGamePersistent->Environment.CurrentEnv.far_plane)
#define MAX_GlowsDist2		float(MAX_GlowsDist1*MAX_GlowsDist1)


//////////////////////////////////////////////////////////////////////
CGlow::CGlow()				{
	flags.bActive	= false;
	position.set	(0,0,0);
	direction.set	(0,0,0);
	radius			= 0.1f;
	color.set		(1,1,1,1);
	bTestResult		= FALSE;
	fade			= 1.f;
	dwFrame			= 0;
	spatial.type	= STYPE_RENDERABLE;
}

void	CGlow::set_active		(bool a)				
{
	if (a)
	{
		if (flags.bActive)					return;
		flags.bActive						= true;
		spatial_register					();
	}
	else
	{
		if (!flags.bActive)					return;
		flags.bActive						= false;
		spatial_unregister					();
	}
}

bool	CGlow::get_active		()					{return flags.bActive;}
void	CGlow::set_position		(const Fvector& P)	{
	if (position.similar(P))	return;
	position.set				(P);
	spatial_move				();
};
void	CGlow::set_direction	(const Fvector& D)	{
	direction.normalize_safe	(D);
};
void	CGlow::set_radius		(float R)			{
	if (fsimilar(radius,R))		return;
	radius						= R;
	spatial_move				();
};
void	CGlow::set_texture		(LPCSTR name)		{
	shader.create				("effects\\glow",name);
}
void	CGlow::set_color		(const Fcolor& C)	{
	color						= C;
}
void	CGlow::set_color		(float r, float g, float b)	{
	color.set					(r,g,b,1);
}
void	CGlow::spatial_move		()
{
	spatial.center				= position;
	spatial.radius				= radius;
	ISpatial::spatial_move		();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlowManager::CGlowManager()
{
}

CGlowManager::~CGlowManager	()
{
}

void CGlowManager::Load		(IReader* fs)
{
	// glows itself
	u32 size	= fs->length();
	R_ASSERT	(size);
	u32 one		= 4*sizeof(float)+1*sizeof(u16);
	R_ASSERT	(size%one == 0);
	u32 count	= size/one;
	Glows.reserve(count);

	for (;count;count--)
	{
		CGlow* G			= xr_new<CGlow>();
		fs->r				(&G->position,	3*sizeof(float));
		fs->r				(&G->radius,	1*sizeof(float));
		G->spatial.center	= G->position;
		G->spatial.radius	= G->radius;
		G->direction.set	( 0,0,0 );

		u16 S				= fs->r_u16();
		G->shader			= ::RImplementation.getShader(S);

		G->fade				= 255.f;
		G->dwFrame			= 0x0;
		G->bTestResult		= TRUE;

		G->spatial.type		= STYPE_RENDERABLE;

		G->set_active		(true);

		Glows.push_back		(G);
	}
	dwTestID		= 0;

	hGeom.create	(FVF::F_TL,RCache.Vertex.Buffer(),RCache.QuadIB);
}

void CGlowManager::Unload	()
{
	// glows
	xr_vector<CGlow*>::iterator	it	= Glows.begin();
	xr_vector<CGlow*>::iterator	end	= Glows.end();
	for (; it!=end; it++)
	{
		CGlow*		G			= *it;
		G->set_active			(false);
		xr_delete				(G);
	}
	Glows.clear		();
	Selected.clear	();
}

IC bool glow_compare(CGlow* g1, CGlow *g2)
{	return g1->shader < g2->shader; }

void CGlowManager::add	(CGlow *G)
{
	if (G->dwFrame	==Device.dwFrame)		return;
	G->dwFrame		= Device.dwFrame;

	Device.Statistic.RenderDUMP_Glows.Begin();

	float	dt		= Device.fTimeDelta;
	float	dlim2	= MAX_GlowsDist2;

	float	range = Device.vCameraPosition.distance_to_sqr	(G->spatial.center);
	if (range < dlim2) 
	{
		// 2. Use result of test
		if (G->bTestResult) {
			G->fade -= dt*FADE_SCALE_DOWN;
			if (G->fade<1.)		G->fade = 1;
		} else {
			G->fade	+= dt*FADE_SCALE_UP;
			if (G->fade>255.f)	G->fade = 255.f;
		}

		Selected.push_back(G);
		return;
	}
	G->fade -= dt*FADE_SCALE_DOWN;
	if (G->fade<1.) G->fade = 1;

	Device.Statistic.RenderDUMP_Glows.End();
}

void CGlowManager::Render()
{
	if (Selected.empty()) return;

	Device.Statistic.RenderDUMP_Glows.Begin();
	float	_width		= float(RImplementation.getTarget()->get_width());
	float	_width_2	= _width / 2.f;
	float	_height_2	= float(RImplementation.getTarget()->get_height())/2.f;
	float	fov_scale	= _width / (Device.fFOV/90.f);
	{
		// 0. save main view and disable
		CObject*	o_main		= g_pGameLevel->CurrentViewEntity();
		BOOL		o_enable	= FALSE;
		if (o_main){
			o_enable		=	o_main->getEnabled();
			o_main->setEnabled	(FALSE);
		}

		// 1. Test some number of glows
		Fvector &start	= Device.vCameraPosition;
		for (int i=0; i<ps_r1_GlowsPerFrame; i++,dwTestID++)
		{
			u32	ID		= dwTestID%Selected.size();
			CGlow&	G	= *Selected[ID];
			if (G.dwFrame=='test')	break;

			G.dwFrame	=	'test';
			Fvector		dir;
			dir.sub		(G.spatial.center,start); float range = dir.magnitude();
			dir.div		(range);
			G.bTestResult = g_pGameLevel->ObjectSpace.RayTest(start,dir,range,Collide::rqtBoth,&G.RayCache);
		}
		
		// 1.5 restore main view
		if (o_main)				o_main->setEnabled	(o_enable);

		// 2. Sort by shader
		std::sort		(Selected.begin(),Selected.end(),glow_compare);
		
		FVF::TL		*	pv;
		FVF::TL			TL;
		
		u32				pos = 0, count;
		ref_shader		T;

		float		dlim2	= MAX_GlowsDist2;
		for (;pos<Selected.size();) 
		{
			T		= Selected[pos]->shader;
			count	= 0;
			while	((pos+count<Selected.size()) && (Selected[pos+count]->shader==T)) count++;
			
			u32		vOffset;
			u32		end		= pos+count;
			FVF::TL	*	pvs		= pv = (FVF::TL*) RCache.Vertex.Lock(count*4,hGeom->vb_stride,vOffset);
			for (; pos<end; pos++)
			{
				// Cull invisible 
				CGlow&	G					= *Selected[pos];
				if (G.fade<=1.f)			continue;

				// Now perform dotproduct if need it
				float	scale	= 1.f, dist_sq;
				Fvector	dir;
				dir.sub			(Device.vCameraPosition,G.position);
				dist_sq			= dir.square_magnitude();
				if (G.direction.square_magnitude()>EPS)	{
					dir.div			(_sqrt(dist_sq));
					scale			= dir.dotproduct(G.direction);
				}
				if (G.fade*scale<=1.f)		continue;

				// Now fade glows directly in front of us
				TL.transform	(G.spatial.center,Device.mFullTransform);
				if (TL.p.z<0 || TL.p.w<0)	continue;
				float size		=	fov_scale * G.spatial.radius / TL.p.w;
				float snear		=	TL.p.z;	clamp	(snear,0.f,.5f);
				scale			*=	snear*2.f;
				if (G.fade*scale<=1.f)		continue;
				
				// Convert to screen coords
				float cx        = (1+TL.p.x)*_width_2;
				float cy        = (1+TL.p.y)*_height_2;
				u32 C			= iFloor(G.fade*scale*(1-(dist_sq/dlim2)));
				u32 clr			= color_rgba(C,C,C,C);
				
				pv->set(cx - size, cy + size, TL.p.z, TL.p.w, clr, 0, 1); pv++;
				pv->set(cx - size, cy - size, TL.p.z, TL.p.w, clr, 0, 0); pv++;
				pv->set(cx + size, cy + size, TL.p.z, TL.p.w, clr, 1, 1); pv++;
				pv->set(cx + size, cy - size, TL.p.z, TL.p.w, clr, 1, 0); pv++;
			}
			int vCount				= int(pv-pvs);
			RCache.Vertex.Unlock	(vCount,hGeom->vb_stride);
			if (vCount) {
				RCache.set_Shader		(T);
				RCache.set_Geometry		(hGeom);
				RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
			}
		}
	}
	Selected.clear							();
	Device.Statistic.RenderDUMP_Glows.End	();
}
