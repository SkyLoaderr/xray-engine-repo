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
		if (bActive)		return;
		bActive				= true;
		RImplementation.Lights.Activate		(this);	
	}
	else
	{
		if (!bActive)		return;
		bActive				= false;
		RImplementation.Lights.Deactivate	(this);
	}
}
