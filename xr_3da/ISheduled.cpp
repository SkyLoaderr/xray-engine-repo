#include "stdafx.h"
#include "xrSheduler.h"

ISheduled::ISheduled	()	
{
	shedule.t_min		= 20;
	shedule.t_max		= 1000;
	shedule.b_locked	= FALSE;
	shedule_Register	();
}

ISheduled::~ISheduled	()
{
	shedule_Unregister	();
}

void	ISheduled::shedule_Register			()
{
	Engine.Sheduler.Register				(this);
}

void	ISheduled::shedule_Unregister		()
{
	Engine.Sheduler.Unregister				(this);
}
