#include "stdafx.h"
#include "xr_creator.h"
#include "xr_object.h"
#include "xr_object_list.h"
#include "xr_ini.h"

void CObjectList::Load()
{
	R_ASSERT	(map_POOL.empty() && map_NETID.empty() && objects.empty() && destroy_queue.empty());

	u32	mem_0			= Memory.mem_usage();
	CTimer T;			T.Start		();

	CInifile::Root&	R	= pSettings->Sections();
	int	p_count			= 0;
	for (CInifile::RootIt	S	= R.begin(); S!=R.end(); S++)
	{
		if (pSettings->LineExists(S->Name,"$prefetch"))
		{
			int		count		=	pSettings->ReadINT(S->Name,"$prefetch");
			R_ASSERT2			((count>0) && (count<=128), "Too many objects for prefetching");
			CLASS_ID CLS		=	pSettings->ReadCLSID(S->Name,"class");
			p_count				+=	count;

			for (int c=0; c<count; c++)
			{
				CObject* pObject	= (CObject*) NEW_INSTANCE(CLS);
				pObject->Load		(S->Name);
				map_POOL.insert		(make_pair(CLS,pObject));
			}
		}
	}

	float	p_time		= 1000.f*T.Get();
	u32		p_mem		= Memory.mem_usage() - mem_0;
	if (p_count)
	{
		Msg					("* [Object-prefetch] objects: %d",		p_count);
		Msg					("* [Object-prefetch] time:    %d ms",	iFloor(p_time));
		Msg					("* [Object-prefetch] memory:  %dKb",	p_mem/1024);
		Msg					("* [Object-prefetch] average: %f ms, %d bytes", p_time/p_count, p_mem/p_count);
	}
}


void CObjectList::Unload	( )
{
	for (u32 i=0; i<objects.size(); i++) 
	{
		Destroy	( objects[i] );
	}
	objects.clear();
}
