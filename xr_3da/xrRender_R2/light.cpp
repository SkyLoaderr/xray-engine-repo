#include "StdAfx.h"
#include "light.h"

light::light(void)
{
	flags.type		= POINT;
	flags.bStatic	= false;
	flags.bActive	= false;
	flags.bShadow	= false;
	position.set	(0,0,0);
	direction.set	(0,-1,0);
	range			= 8.f;
	cone			= deg2rad(60.f);
	color.set		(1,1,1,1);
}

light::~light(void)
{
}

void light::set_active(bool a)
{
	if (a)
	{
		if (flags.bActive)	return;
		flags.bActive		= true;
		RImplementation.Lights.Activate		(this);	
	}
	else
	{
		if (!flags.bActive)	return;
		flags.bActive		= false;
		RImplementation.Lights.Deactivate	(this);
	}
}
