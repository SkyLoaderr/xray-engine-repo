#ifndef __XR_OBJECT_LIST_H__
#define __XR_OBJECT_LIST_H__

#include "xrSheduler.h"

// refs
class ENGINE_API CObject;
class ENGINE_API CInifile;
class ENGINE_API NET_Packet;

class	ENGINE_API 				CObjectList
{
public:
	typedef vector<CObject*>::iterator	OBJ_IT;

	// data
	vector<CObject*>			objects;
	map<u32,CObject*>			map_NETID;

	CSheduler					sheduled;

	// methods
								CObjectList			( );
								~CObjectList		( );

	CObject*					FindObjectByName	( char* name );
	CObject*					FindObjectByCLS_ID	( CLASS_ID cls );

	VOID						Unload				( );

	CObject*					LoadOne				( CInifile *ini, const char* name);

	VOID						DestroyObject		( CObject* object );

	VOID						OnMove				( );

	VOID						net_Register		(CObject* O);
	VOID						net_Unregister		(CObject* O);

	VOID						net_Export			(NET_Packet* P);
	VOID						net_Import			(NET_Packet* P);
	CObject*					net_Find			(u32 ID);
};

#endif //__XR_OBJECT_LIST_H__
