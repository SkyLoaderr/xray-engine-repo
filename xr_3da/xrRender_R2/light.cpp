#include "StdAfx.h"
#include "light.h"

light::light(void)
{
	spatial.type	= STYPE_LIGHTSOURCE;
	flags.type		= POINT;
	flags.bStatic	= false;
	flags.bActive	= false;
	flags.bShadow	= false;
	position.set	(0,0,0);
	direction.set	(0,-1,0);
	range			= 8.f;
	cone			= deg2rad(60.f);
	color.set		(1,1,1,1);

	frame_render	= 0;
	frame_sleep		= 0;

#if RENDER==R_R2
	s_spot			= NULL;
	s_point			= NULL;
	vis.frame2test	= 0;	// xffffffff;
	vis.query_id	= 0;
	vis.query_order	= 0;
	vis.visible		= true;
	vis.pending		= false;
#endif
}

light::~light(void)
{
	// by Dima
	set_active	(false);
	// end
}

#if RENDER==R_R2 
void light::set_texture		(LPCSTR name)
{
	if (NULL==name)
	{
		// default shaders
		s_spot.destroy		();
		s_point.destroy		();
		return;
	}

#pragma todo				("Only shadowed spot implements projective texture")
	string128				temp;
	s_spot.create			(RImplementation.Target.b_accum_spot,strconcat(temp,"r2\\accum_spot_",name),name);
}
#endif

#if RENDER==R_R1
void light::set_texture		(LPCSTR name)
{
}
#endif

void light::set_active		(bool a)
{
	if (a)
	{
		if (flags.bActive)					return;
		flags.bActive						= true;
		spatial_register					();
		//Msg									("!!! L-register: %X",u32(this));
	}
	else
	{
		if (!flags.bActive)					return;
		flags.bActive						= false;
		spatial_unregister					();
		//Msg									("!!! L-unregister: %X",u32(this));
	}
}

void	light::set_position		(const Fvector& P)
{ 
	if (position.similar(P))	return;
	frame_sleep					=	Device.dwFrame + ps_r__LightSleepFrames;
	position.set				(P);
	spatial_move				();
}

void	light::set_range		(float R)			{
	if (fsimilar(range,R))		return;
	range						= R;
	spatial_move				();
};

void	light::set_cone			(float angle)		{
	if (fsimilar(cone,angle))	return;
	VERIFY						(cone < deg2rad(120.f));
	cone						= angle;
	spatial_move				();
}
void	light::set_rotation		(const Fvector& D, const Fvector& R)	{ 
	Fvector	old_D		= direction;
	direction.normalize	(D);
	right.normalize_safe(R);
	if (!fsimilar(1.f, old_D.dotproduct(D)))	spatial_move	();
}

void	light::spatial_move			()
{
	//spatial.center				=	position;
	//spatial.radius				=	range;
	switch(flags.type)	{
	case IRender_Light::REFLECTED	:	
	case IRender_Light::POINT		:	
	case IRender_Light::SPOT		:	
		{
			spatial.center				= position;
			spatial.radius				= range;
		} 
		break;
		/*
	case IRender_Light::SPOT		:	
		{
			// minimal enclosing sphere around cone
			if (cone>=PI_DIV_2)			{
				// obtused-angled
				spatial.center.mad			(position,direction,range);
				spatial.radius				= range * tanf(cone/2.f);
			} else {
				// acute-angled
				spatial.radius				= range / (2.f * _sqr(_cos(cone/2.f)));
				spatial.center.mad			(position,direction,spatial.radius);
			}
		}
		break;
		*/
	}
	ISpatial::spatial_move			();

#if RENDER==R_R2
	if (flags.bActive) gi_generate	();
#endif
}

Fvector	light::spatial_sector_point	()	
{ 
	return position; 
}

//////////////////////////////////////////////////////////////////////////
#if RENDER==R_R2
const	u32	delay_small_min			= 1;
const	u32	delay_small_max			= 3;
const	u32	delay_large_min			= 10;
const	u32	delay_large_max			= 20;
const	u32	cullfragments			= 4;

void	light::vis_prepare			()
{
	if (indirect_photons!=ps_r2_GI_photons)	gi_generate	();

	//	. test is sheduled for future	= keep old result
	//	. test time comes :)
	//		. camera inside light volume	= visible,	shedule for 'small' interval
	//		. perform testing				= ???,		pending

	u32	frame	= Device.dwFrame;
	if (frame	<	vis.frame2test)	return;
	if (Device.vCameraPosition.distance_to(spatial.center)<=spatial.radius)	{
		vis.visible		=	true;
		vis.pending		=	false;
		vis.frame2test	=	frame	+ ::Random.randI(delay_small_min,delay_small_max);
		return;
	}

	// testing
	vis.pending										= true;
	xform_calc										();
	RCache.set_xform_world							(m_xform);
	vis.query_order	= RImplementation.occq_begin	(vis.query_id);
	RImplementation.Target.draw_volume				(this);
	RImplementation.occq_end						(vis.query_id);
}

void	light::vis_update			()
{
	//	. not pending	->>> return (early out)
	//	. test-result:	visible:
	//		. shedule for 'large' interval
	//	. test-result:	invisible:
	//		. shedule for 'next-frame' interval

	if (!vis.pending)	return;

	u32	frame			= Device.dwFrame;
	u32 fragments		= RImplementation.occq_get	(vis.query_id);
	vis.visible			= (fragments > cullfragments);
	vis.pending			= false;
	if (vis.visible)	{
		vis.frame2test	=	frame	+ ::Random.randI(delay_large_min,delay_large_max);
	} else {
		vis.frame2test	=	frame	+ 1;
	}
}

// Xforms
void	light::xform_calc			()
{
	if	(Device.dwFrame == m_xform_frame)	return;
	m_xform_frame	= Device.dwFrame;

	switch(flags.type)	{
	case IRender_Light::REFLECTED	:
	case IRender_Light::POINT		:
		{
			// scale of identity sphere
			float		L_R			= range;
			m_xform.scale			(L_R,L_R,L_R);
			m_xform.translate_over	(position);
		}
		break;
	case IRender_Light::SPOT		:
		{
			// scale to account range and angle
			float		s			= 2.f*range*tanf(cone/2.f);	
			Fmatrix		mScale;		mScale.scale(s,s,range);	// make range and radius

			// build final rotation / translation
			Fvector					L_dir,L_up,L_right;
			L_dir.set				(direction);			L_dir.normalize		();
			L_up.set				(0,1,0);				if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
			L_right.crossproduct	(L_up,L_dir);			L_right.normalize	();
			L_up.crossproduct		(L_dir,L_right);		L_up.normalize		();

			Fmatrix		mR;
			mR.i					= L_right;	mR._14	= 0;
			mR.j					= L_up;		mR._24	= 0;
			mR.k					= L_dir;	mR._34	= 0;
			mR.c					= position;	mR._44	= 1;

			// final xform
			m_xform.mul_43			(mR,mScale);
		}
		break;
	default:
		m_xform.identity	();
		break;
	}
}

void	light::gi_generate	()
{
	indirect.clear		();
	indirect_photons	= ps_r2_ls_flags.test(R2FLAG_GI)?ps_r2_GI_photons:0;

	CRandom				random;
	random.seed			(0x12071980);

	xrXRC&		xrc		= RImplementation.Sectors_xrc;
	CDB::MODEL*	model	= g_pGameLevel->ObjectSpace.GetStaticModel	();
	CDB::TRI*	tris	= g_pGameLevel->ObjectSpace.GetStaticTris	();
	Fvector*	verts	= g_pGameLevel->ObjectSpace.GetStaticVerts	();
	xrc.ray_options		(CDB::OPT_CULL|CDB::OPT_ONLYNEAREST);

	for (int it=0; it<indirect_photons; it++)	{
		Fvector	dir,idir;
		switch	(flags.type)		{
		case IRender_Light::POINT	:	dir.random_dir(random);					break;
		case IRender_Light::SPOT	:	dir.random_dir(direction,cone,random);	break;
		}
		dir.normalize		();
		xrc.ray_query		(model,position,dir,range);
		if (!xrc.r_count()) continue;
		CDB::RESULT *R		= RImplementation.Sectors_xrc.r_begin	();
		CDB::TRI&	T		= tris[R->id];
		Fvector		Tv[3]	= { verts[T.verts[0]],verts[T.verts[1]],verts[T.verts[2]] };
		Fvector		TN;		TN.mknormal		(Tv[0],Tv[1],Tv[2]);
		float		dot		= TN.dotproduct	(idir.invert(dir));

		light_indirect		LI;
		LI.P.mad			(position,dir,R->range);
		LI.D.reflect		(dir,TN);
		LI.E				= dot * ps_r2_GI_refl * (1-R->range/range) / float(indirect_photons);
		if (LI.E < ps_r2_GI_clip)	continue;
		LI.S				= spatial.sector;	//BUG

		indirect.push_back	(LI);
	}
}

#endif
