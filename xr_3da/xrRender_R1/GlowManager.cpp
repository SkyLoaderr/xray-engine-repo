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
CGlow::~CGlow()
{
	shader.destroy	();
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

	hGeom.create	(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);
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

IC void FillSprite	(FVF::LIT*& pv, const Fvector& pos, float r, u32 clr)
{
	const Fvector& T 	= Device.vCameraTop;
	const Fvector& R 	= Device.vCameraRight;
	Fvector		Vr, Vt;
	Vr.mul 		(R,r);
	Vt.mul		(T,r);

	Fvector 	a,b,c,d;
	a.sub		(Vt,Vr);
	b.add		(Vt,Vr);
	c.invert	(a);
	d.invert	(b);
	pv->set		(d.x+pos.x,d.y+pos.y,d.z+pos.z, clr, 0.f,1.f);	pv++;
	pv->set		(a.x+pos.x,a.y+pos.y,a.z+pos.z, clr, 0.f,0.f);	pv++;
	pv->set		(c.x+pos.x,c.y+pos.y,c.z+pos.z, clr, 1.f,1.f);	pv++;
	pv->set		(b.x+pos.x,b.y+pos.y,b.z+pos.z,	clr, 1.f,0.f);	pv++;
}

void CGlowManager::Render			()
{
	if (Selected.empty())					return		;
	RCache.set_xform_world					(Fidentity)	;

	Device.Statistic.RenderDUMP_Glows.Begin	();
	render_sw								();
	Device.Statistic.RenderDUMP_Glows.End	();
}

void CGlowManager::render_sw		()
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
		G.bTestResult = g_pGameLevel->ObjectSpace.RayTest(start,dir,range,collide::rqtBoth,&G.RayCache);
	}

	// 1.5 restore main view
	if (o_main)				o_main->setEnabled	(o_enable);

	// 2. Sort by shader
	std::sort		(Selected.begin(),Selected.end(),glow_compare);

	FVF::LIT		*pv;

	u32				pos = 0, count;
	ref_shader		T;

	Fplane			NP;
	NP.build		(Device.vCameraPosition,Device.vCameraDirection);

	float		dlim2	= MAX_GlowsDist2;
	for (;pos<Selected.size();) 
	{
		T		= Selected[pos]->shader;
		count	= 0;
		while	((pos+count<Selected.size()) && (Selected[pos+count]->shader==T)) count++;

		u32		vOffset;
		u32		end		= pos+count;
		FVF::LIT* pvs	= pv = (FVF::LIT*) RCache.Vertex.Lock(count*4,hGeom->vb_stride,vOffset);
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

			// near fade
			float dist_np	= NP.distance(G.position)-VIEWPORT_NEAR;
			float snear		= dist_np/0.15f;	clamp	(snear,0.f,1.f);
			scale			*=	snear;
			if (G.fade*scale<=1.f)		continue;

			u32 C			= iFloor(G.fade*scale*(1-(dist_sq/dlim2)));
			u32 clr			= color_rgba(C,C,C,C);
			FillSprite		(pv,G.position,G.radius,clr);
		}
		int vCount				= int(pv-pvs);
		RCache.Vertex.Unlock	(vCount,hGeom->vb_stride);
		if (vCount) {
			RCache.set_Shader		(T);
			RCache.set_Geometry		(hGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		}
	}
	Selected.clear				();
}
