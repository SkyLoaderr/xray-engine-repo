#ifndef __XR_OBJECT_LIST_H__
#define __XR_OBJECT_LIST_H__

#include "xrSheduler.h"

// refs
class ENGINE_API CObject;
class ENGINE_API CInifile;
class ENGINE_API NET_Packet;

class	ENGINE_API 				CObjectList
{
private:
	// data
	xr_map<u32,CObject*>		map_NETID;
	xr_vector<CObject*>			objects;
	xr_vector<CObject*>			destroy_queue;

public:
	// methods
								CObjectList			( );
								~CObjectList		( );

	CObject*					FindObjectByName	( LPCSTR	name	);
	CObject*					FindObjectByCLS_ID	( CLASS_ID	cls		);

	void						Load				( );
	void						Unload				( );

	CObject*					Create				( LPCSTR	name	);
	void						Destroy				( CObject*	O		);

	void						SingleUpdate		( CObject*	O		);
	void						Update				( );

	void						net_Register		( CObject*	O		);
	void						net_Unregister		( CObject*	O		);

	void						net_Export			( NET_Packet* P		);
	void						net_Import			( NET_Packet* P		);
	CObject*					net_Find			( u32 ID			);

	void						SLS_Save			( IWriter&	fs		);
	void						SLS_Load			( IReader&	fs		);
};

#endif //__XR_OBJECT_LIST_H__
