#include "stdafx.h"
#include "igame_level.h"
#include "igame_objectpool.h"
#include "xr_object.h"

IGame_ObjectPool::IGame_ObjectPool(void)
{
}

IGame_ObjectPool::~IGame_ObjectPool(void)
{
	R_ASSERT			(map_POOL.empty());
}

void IGame_ObjectPool::load	()
{
	R_ASSERT			(map_POOL.empty());
	if (strstr(Core.Params,"-noprefetch"))	return;

	u32	mem_0			= Memory.mem_usage();
	CTimer T;			T.Start		();

	CInifile::Root&	R	= pSettings->sections();
	int	p_count			= 0;

	::Render->model_Logging	(FALSE);
	for (CInifile::RootIt	S	= R.begin(); S!=R.end(); S++)
	{
		if (pSettings->line_exist(*S->Name,"$prefetch"))
		{
			Msg					("* prefetching: %s",	*S->Name);
			int		count		=	pSettings->r_s32	(*S->Name,"$prefetch");
			R_ASSERT2			((count>0) && (count<=128), "Too many objects for prefetching");
			CLASS_ID CLS		=	pSettings->r_clsid	(*S->Name,"class");
			p_count				+=	count;

			for (int c=0; c<count; c++)
			{
				CObject* pObject	= (CObject*) NEW_INSTANCE(CLS);
				pObject->Load		(*S->Name);
				VERIFY2				(*pObject->cNameSect(),*S->Name);
				map_POOL.insert		(mk_pair(pObject->cNameSect(),pObject));
			}
		}
	}
	::Render->model_Logging	(TRUE);

	float	p_time		= 1000.f*T.GetElapsed_sec();
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

void IGame_ObjectPool::unload	( )
{
	// Clear POOL
	for (POOL_IT it=map_POOL.begin(); it!=map_POOL.end(); it++)	
		xr_delete	(it->second);
	map_POOL.clear(); 
}

CObject*	IGame_ObjectPool::create			( LPCSTR	name	)
{
	string256			l_name;
	POOL_IT	it			=	map_POOL.find	(shared_str(strlwr(strcpy(l_name,name))));
	if (it!=map_POOL.end())
	{
		// Instance found
		CObject* O			=	it->second;
		map_POOL.erase		(it);
		return				O;
	} else {
		// Create and load _new instance
		Msg					("! Uncached loading '%s'...",name);
		CLASS_ID CLS		=	pSettings->r_clsid		(name,"class");
		CObject* O			=	(CObject*) NEW_INSTANCE	(CLS);
		O->Load				(name);
		return				O;
	}
}

void		IGame_ObjectPool::destroy			( CObject*	O		)
{
	map_POOL.insert			(mk_pair(O->cNameSect(),O));
}
