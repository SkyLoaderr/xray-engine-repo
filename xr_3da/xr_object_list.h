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
	struct str_pred : public binary_function<char*, char*, bool> 
	{	
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};

	typedef vector<CObject*>::iterator				OBJ_IT;
	typedef multimap<LPCSTR,CObject*,str_pred>		POOL;
	typedef POOL::iterator							POOL_IT;
private:
	POOL						map_POOL;
	map<u32,CObject*>			map_NETID;
public:
	// data
	vector<CObject*>			objects;
	vector<CObject*>			destroy_queue;

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
