// LightTrack.cpp: implementation of the CROS_impl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "..\xr_object.h"

#ifdef _EDITOR
#include "igame_persistent.h"
#include "environment.h"
#else
#include "..\igame_persistent.h"
#include "..\environment.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CROS_impl::CROS_impl	()
{
	approximate.set		( 0,0,0 );
	dwFrame				= u32(-1);
	shadow_recv_frame	= u32(-1);
	shadow_recv_slot	= -1;

	result_count		= 0;
	result_iterator		= 0;
	result_frame		= u32(-1);
	result_sun			= 0;
	hemi_value			= 0;
	hemi_smooth			= 0;
	sun_value			= 0;
	sun_smooth			= 0;

#if RENDER==R_R1
	MODE				= IRender_ObjectSpecific::TRACE_ALL		;
#else
	MODE				= IRender_ObjectSpecific::TRACE_HEMI	;
#endif
}

void	CROS_impl::add		(light* source)
{
	// Search
	for (xr_vector<Item>::iterator I=track.begin(); I!=track.end(); I++)	
		if (source == I->source)	{ I->frame_touched = Device.dwFrame; return; }

	// Register _new_
	track.push_back		(Item());
	Item&	L			= track.back();
	L.frame_touched		= Device.dwFrame;
	L.source			= source;
	L.cache.verts[0].set(0,0,0);
	L.cache.verts[1].set(0,0,0);
	L.cache.verts[2].set(0,0,0);
	L.test				= 0.f;
	L.energy			= 0.f;
}

IC bool	pred_energy			(const CROS_impl::Light& L1, const CROS_impl::Light& L2)	{ return L1.energy>L2.energy; }
//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable:4305)
const float		hdir		[lt_hemisamples][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};
#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////
void	CROS_impl::update	(IRenderable* O)
{
	// clip & verify
	if					(dwFrame==Device.dwFrame)			return;
	dwFrame				= Device.dwFrame;
	if					(0==O)								return;
	VERIFY				(dynamic_cast<CROS_impl*>(O->renderable.ROS));

	// select sample
	Fvector	position;	O->renderable.xform.transform_tiny	(position,O->renderable.visual->vis.sphere.P);
	float	radius;		radius = O->renderable.visual->vis.sphere.R;

	// sun-tracing
	if	(MODE & IRender_ObjectSpecific::TRACE_SUN)	{
		if  (--result_sun	< 0)	{
			result_sun		+=		::Random.randI(lt_hemisamples/4,lt_hemisamples/2)	;
			#if RENDER==R_R1
			 light*	sun		=		RImplementation.L_DB->sun_adapted	;
			#else
			 light*	sun		=		RImplementation.Lights.sun_adapted	;
			#endif
			Fvector	direction;	direction.set	(sun->direction).invert().normalize	();
			sun_value		=	(g_pGameLevel->ObjectSpace.RayTest(position,direction,500.f,Collide::rqtBoth,&cache_sun))?1.f:0.f;
		}
	}
	
	// hemi-tracing
	if	(MODE & IRender_ObjectSpecific::TRACE_HEMI)	{
		u32	sample		= 0					;
		if	(result_count<lt_hemisamples)	{ sample=result_count; result_count++;							}
		else								{ sample=(result_iterator%lt_hemisamples); result_iterator++;	}

		// take sample
		Fvector	direction;	direction.set	(hdir[sample][0],hdir[sample][1],hdir[sample][2]).normalize	();
		result[sample]	=	!!g_pGameLevel->ObjectSpace.RayTest(position,direction,500.f,Collide::rqtBoth,&cache[sample]);
	}

	// light-tracing
	BOOL	bTraceLights	= MODE & IRender_ObjectSpecific::TRACE_SUN;
	if		((!O->renderable_ShadowGenerate()) && (!O->renderable_ShadowReceive()))	bTraceLights = FALSE;
	if		(bTraceLights)	{
		// Select nearest lights
		Fvector					bb_size	=	{radius,radius,radius};
		g_SpatialSpace->q_box				(RImplementation.lstSpatial,0,STYPE_LIGHTSOURCE,pos,bb_size);
		for (u32 o_it=0; o_it<RImplementation.lstSpatial.size(); o_it++)	{
			ISpatial*	spatial		= RImplementation.lstSpatial[o_it];
			light*		source		= (light*)	(spatial->dcast_Light());
			VERIFY		(source);	// sanity check
			float	R				= radius+source->range;
			if (position.distance_to(source->position) < R)		add	(source);
		}

		// Trace visibility
		lights.clear	();
		float traceR	= radius*.5f;
		for (s32 id=0; id<s32(track.size()); id++)
		{
			// remove untouched lights
			xr_vector<CROS_impl::Item>::iterator I	= track.begin()+id;
			if (I->frame_touched!=Device.dwFrame)	{ track.erase(I) ; id--	; continue ; }

			// Trace visibility
			Fvector				P,D;
			float		amount	= 0;
			light*		xrL		= I->source;
			Fvector&	LP		= xrL->position;
			P.mad				(pos,P.random_dir(),traceR);		// Random point inside range

			// point/spot
			float	f			=	D.sub(P,LP).magnitude();
			if (g_pGameLevel->ObjectSpace.RayTest(LP,D.div(f),f,Collide::rqtStatic,&I->cache))	amount -=	lt_dec;
			else																				amount +=	lt_inc;
			I->test				+=	amount * dt;	clamp	(I->test,-.5f,1.f);
			I->energy			=	.9f*I->energy + .1f*I->test;

			// 
			float	E			=	I->energy * xrL->color.intensity	();
			if (E > EPS)		{
				// Select light
				lights.push_back			(CROS_impl::Light())		;
				CROS_impl::Light&	L		= lights.back()				;
				L.source					= xrL						;
				L.color.mul_rgb				(xrL->color,I->energy/2)	;
				L.energy					= I->energy/2				;
				if (!xrL->flags.bStatic)	{ L.color.mul_rgb(.5f); L.energy *= .5f; }
			}
		}

		// Sort lights by importance - important for R1-shadows
		std::sort	(lights.begin(),lights.end(), pred_energy);
	}

	// hemi & sun: update and smooth
	float	dt				=	Device.fTimeDelta;
	float	l_f				=	dt*lt_smooth;
	float	l_i				=	1.f-l_f;
	int		_pass			=	0;
	for (int it=0; it<result_count; it++)	if (result[it])	_pass	++;
	hemi_value				=	float	(_pass)/float(result_count?result_count:1);
	hemi_smooth				=	hemi_value*l_f + hemi_smooth*l_i;
	sun_smooth				=	sun_value *l_f + sun_smooth *l_i;

	// Process ambient lighting and approximate average lighting
	// Process our lights to find average luminiscense
	CEnvDescriptor&	desc	=	g_pGamePersistent->Environment.CurrentEnv;
	Fvector			accum	=	{ desc.ambient.x,	 desc.ambient.y,		desc.ambient.z		};
	Fvector			hemi	=	{ desc.hemi_color.x,	desc.hemi_color.y,	desc.hemi_color.z	};
	if (MODE & IRender_ObjectSpecific::TRACE_HEMI	)	hemi.mul(hemi_smooth); else hemi.mul(.2f);
					accum.add	( hemi );
	if (MODE & IRender_ObjectSpecific::TRACE_LIGHTS )	{
		for (u32 lit=0; lit<lights.size(); lit++)	{
			accum.x += lights[lit].color.r*desc.lmap_color.x;
			accum.y += lights[lit].color.g*desc.lmap_color.y;
			accum.z += lights[lit].color.b*desc.lmap_color.z;
		}
	} else 			accum.mad	( desc.lmap_color,	 .1f );
	approximate				=	accum;
}
