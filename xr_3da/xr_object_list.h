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
	vector<CObject*>			destroy_queue;

	// methods
								CObjectList			( );
								~CObjectList		( );

	CObject*					FindObjectByName	( LPCSTR	name	);
	CObject*					FindObjectByCLS_ID	( CLASS_ID	cls		);

	void						Unload				( );

	CObject*					LoadOne				( LPCSTR	name	);

	void						DestroyObject		( CObject*	O		);
	void						DestroyObject		( u32		ID		);

	void						SingleUpdate		(CObject*	O		);
	void						OnMove				( );

	void						net_Register		(CObject*	O		);
	void						net_Unregister		(CObject*	O		);

	void						net_Export			(NET_Packet* P		);
	void						net_Import			(NET_Packet* P		);
	CObject*					net_Find			(u32 ID				);

	void						SLS_Save			(CFS_Base&	fs		);
	void						SLS_Load			(CStream&	fs		);
};

#endif //__XR_OBJECT_LIST_H__
