#include "StdAfx.h"
#include "light.h"

light::light(void)
{
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
