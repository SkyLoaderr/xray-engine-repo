#ifndef __XR_OBJECT_LIST_H__
#define __XR_OBJECT_LIST_H__

// refs
class	ENGINE_API	CObject;
class	ENGINE_API	CInifile;
class	ENGINE_API	NET_Packet;

class	ENGINE_API 				CObjectList
{
private:
	// data
	xr_map<u32,CObject*>		map_NETID;
	xr_vector<CObject*>			destroy_queue;
public:
	xr_vector<CObject*>			objects;
public:
	// methods
								CObjectList			( );
								~CObjectList		( );

	CObject*					FindObjectByName	( shared_str	name	);
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

	u32							net_Export			( NET_Packet* P,	u32 _start, u32 _count	);	// return next start
	void						net_Import			( NET_Packet* P		);
	CObject*					net_Find			( u32 ID			);
};

#endif //__XR_OBJECT_LIST_H__
