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
	right.set		(0,0,0);
	range			= 8.f;
	cone			= deg2rad(60.f);
	color.set		(1,1,1,1);

	frame_render	= 0;

#if RENDER==R_R2
	ZeroMemory		(omnipart,sizeof(omnipart));
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
#if RENDER==R_R2 
	for (int f=0; f<6; f++)	xr_delete(omnipart[f]);
#endif
	// by Dima
	set_active	(false);
	// end
}

#if RENDER==R_R2 
void light::set_texture		(LPCSTR name)
{
	if ((0==name) || (0==name[0]))
	{
		// default shaders
		s_spot.destroy		();
		s_point.destroy		();
		return;
	}

#pragma todo				("Only shadowed spot implements projective texture")
	string256				temp;
	s_spot.create			(RImplementation.Target.b_accum_spot,strconcat(temp,"r2\\accum_spot_",name),name);
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
	VERIFY						(cone < deg2rad(121.f));	// 120 is hard limit for lights
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
	switch(flags.type)	{
	case IRender_Light::REFLECTED	:	
	case IRender_Light::POINT		:	
		{
			spatial.center				= position;
			spatial.radius				= range;
		} 
		break;
	case IRender_Light::SPOT		:	
		{
			// minimal enclosing sphere around cone
			VERIFY2						(cone < deg2rad(121.f), "Too large light-cone angle. Maybe you have passed it in 'degrees'?");
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
	}
	ISpatial::spatial_move			();

#if RENDER==R_R2
	if (flags.bActive) gi_generate	();
	svis.invalidate					();
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

	// build final rotation / translation
	Fvector					L_dir,L_up,L_right;
	L_dir.set				(direction).normalize		();
	L_right.set				(right).normalize			();
	L_up.crossproduct		(L_dir,L_right).normalize	();
	Fmatrix					mR;
	mR.i					= L_right;	mR._14	= 0;
	mR.j					= L_up;		mR._24	= 0;
	mR.k					= L_dir;	mR._34	= 0;
	mR.c					= position;	mR._44	= 1;

	// switch
	switch(flags.type)	{
	case IRender_Light::REFLECTED	:
	case IRender_Light::POINT		:
		{
			// scale of identity sphere
			float		L_R			= range;
			Fmatrix		mScale;		
			mScale.scale			(L_R,L_R,L_R);
			//mScale.translate_over	(position);

			// final xform
			m_xform.mul_43			(mR,mScale);
		}
		break;
	case IRender_Light::SPOT		:
	case IRender_Light::OMNIPART	:
		{
			// scale to account range and angle
			float		s			= 2.f*range*tanf(cone/2.f);	
			Fmatrix		mScale;		mScale.scale(s,s,range);	// make range and radius
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

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static	Fvector cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
static	Fvector cmDir[6]	= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};

void	light::export		(light_Package& package)
{
	if (flags.bShadow)			{
		switch (flags.type)	{
			case IRender_Light::POINT:
				{
					// tough: create/update 6 shadowed lights
					if (0==omnipart[0])	for (int f=0; f<6; f++)	omnipart[f] = xr_new<light> ();
					for (int f=0; f<6; f++)	{
						light*	L			= omnipart[f];
						Fvector				R;
						R.crossproduct		(cmNorm[f],cmDir[f]);
						L->set_type			(IRender_Light::OMNIPART);
						L->set_shadow		(true);
						L->set_position		(position);
						L->set_rotation		(cmDir[f],	R);
						L->set_cone			(PI_DIV_2);
						L->set_color		(color);
						L->s_spot			= s_spot;
						L->s_point			= s_point;
						package.v_shadowed.push_back	(L);
					}
				}
				break;
			case IRender_Light::SPOT:
				package.v_shadowed.push_back			(this);
				break;
		}
	}	else	{
		switch (flags.type)	{
			case IRender_Light::POINT:		package.v_point.push_back	(this);	break;
			case IRender_Light::SPOT:		package.v_spot.push_back	(this);	break;
		}
	}
}

#endif
