#include "stdafx.h"
#include "xrSheduler.h"

ISheduled::ISheduled	()	
{
	shedule.t_min		= 20;
	shedule.t_max		= 1000;
	shedule.b_locked	= FALSE;
}

ISheduled::~ISheduled	()
{
}

void	ISheduled::shedule_register			()
{
	Engine.Sheduler.Register				(this);
}

void	ISheduled::shedule_unregister		()
{
	Engine.Sheduler.Unregister				(this);
}
