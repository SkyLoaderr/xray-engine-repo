#include "stdafx.h"
#include "xr_creator.h"
#include "xr_object.h"
#include "xr_object_list.h"
#include "xr_ini.h"

CObject*	CObjectList::Create				( LPCSTR	name	)
{
	CLASS_ID CLS		=	pSettings->ReadCLSID(name,"class");
	multimap<CLASS_ID,CObject*>::iterator	it	= map_POOL.find	(CLS);
	if (it!=map_POOL.end())
	{
		// Instance found
		CObject* O				=	it->second;
		R_ASSERT(O->SUB_CLS_ID	==	CLS);
		map_POOL.erase		(it);
		objects.push_back	(O);
		return				O;
	} else {
		// Create and load new instance
		Msg					("! Uncached loading '%s'...",name);
		CObject* O			=	(CObject*) NEW_INSTANCE(CLS);
		O->Load				(name);
		objects.push_back	(O);
		return				O;
	}
}

void		CObjectList::Destroy			( CObject*	O		)
{
	if (0==O)				return;
	net_Unregister			(O);
	OBJ_IT it				=	find		(objects.begin(),objects.end(),O);
	if (it!=objects.end())	objects.erase	(it);

	CLASS_ID CLS			=	pSettings->ReadCLSID(O->cNameSect(),"class");
	R_ASSERT(O->SUB_CLS_ID	==	CLS);
	map_POOL.insert			(make_pair(CLS,O));
}
