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

	s_spot_s		= NULL;
	s_spot_uns		= NULL;
	s_point_s		= NULL;
	s_point_uns		= NULL;
}

light::~light(void)
{
}

#if RENDER==R_R2 
void light::set_texture		(LPCSTR name)
{
	if (NULL==name)
	{
		// default shaders
		s_spot_s.destroy	();
		s_spot_uns.destroy	();
		s_point_s.destroy	();
		s_point_uns.destroy	();
		return;
	}

#pragma todo				("Only shadowed spot implements projective texture")
	string128				temp;
	s_spot_s.create			(RImplementation.Target.b_accum_spot_s,strconcat(temp,"r2\\accum_spot_s_",name),name);
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

void	light::set_position	(const Fvector& P)
{ 
	if (position.similar(P))	return;
	position.set				(P);

	spatial.center				= P;
	spatial.radius				= range;
	spatial_move				();
}
