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
	svis[0].invalidate				();
	svis[1].invalidate				();
	svis[2].invalidate				();
	svis[3].invalidate				();
	svis[4].invalidate				();
	svis[5].invalidate				();
#endif
}

Fvector	light::spatial_sector_point	()	
{ 
	return position; 
}

//////////////////////////////////////////////////////////////////////////
#if RENDER==R_R2
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

#endif
