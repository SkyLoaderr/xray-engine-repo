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

	s_spot			= NULL;
	s_point			= NULL;
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
	cone						= angle;
	spatial_move				();
}
void	light::set_direction	(const Fvector& D)	{ 
	if (fsimilar(1.f, direction.dotproduct(D)))	return;
	direction.normalize			(D);
	spatial_move				();
}

void	light::spatial_move			()
{
	//spatial.center				= position;
	//spatial.radius				= range;
	if (flags.type == IRender_Light::POINT)				{
		spatial.center				= position;
		spatial.radius				= range;
	} else if (flags.type == IRender_Light::SPOT)		{
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
	ISpatial::spatial_move		();
}

Fvector	light::spatial_sector_point	()	
{ 
	return position; 
}
