#include "stdafx.h"
#include "xr_creator.h"
#include "xr_object.h"
#include "xr_object_list.h"

void CObjectList::Load()
{
	R_ASSERT			(map_POOL.empty() && map_NETID.empty() && objects.empty() && destroy_queue.empty());

	u32	mem_0			= Memory.mem_usage();
	CTimer T;			T.Start		();

	CInifile::Root&	R	= pSettings->sections();
	int	p_count			= 0;
	for (CInifile::RootIt	S	= R.begin(); S!=R.end(); S++)
	{
		if (pSettings->line_exist(S->Name,"$prefetch"))
		{
			int		count		=	pSettings->r_s32(S->Name,"$prefetch");
			R_ASSERT2			((count>0) && (count<=128), "Too many objects for prefetching");
			CLASS_ID CLS		=	pSettings->r_clsid(S->Name,"class");
			p_count				+=	count;

			for (int c=0; c<count; c++)
			{
				CObject* pObject	= (CObject*) NEW_INSTANCE(CLS);
				pObject->Load		(S->Name);
				map_POOL.insert		(make_pair(pObject->cNameSect(),pObject));
			}
		}
	}

	float	p_time		= 1000.f*T.GetAsync();
	u32		p_mem		= Memory.mem_usage() - mem_0;
	if (p_count)
	{
		float 	a_time		= float(p_time)/float(p_count);
		Msg					("* [Object-prefetch] objects: %d",		p_count);
		Msg					("* [Object-prefetch] time:    %d ms",	iFloor(p_time));
		Msg					("* [Object-prefetch] memory:  %dKb",	p_mem/1024);
		Msg					("* [Object-prefetch] average: %2.2f ms, %d bytes", a_time, p_mem/p_count);
	}
}


void CObjectList::Unload	( )
{
	// Destroy objects
	// Msg	("!!!------------- Unload, %d objects",objects.size());
	while (objects.size())
	{
		CObject*	O	= objects.back();
		// Msg	("!!!------------- Unload: %s",O->cName());
		O->net_Destroy	(   );
		Destroy			( O );
	}
	objects.clear();

	// Clear POOL
	for (POOL_IT it=map_POOL.begin(); it!=map_POOL.end(); it++)	
		xr_delete	(it->second);
	map_POOL.clear();
}
