#ifndef __XR_OBJECT_LIST_H__
#define __XR_OBJECT_LIST_H__

// refs
class	ENGINE_API	CObject;
class	NET_Packet	;

class	ENGINE_API 				CObjectList
{
private:
	// data
	xr_map<u32,CObject*>		map_NETID			;
	xr_vector<CObject*>			destroy_queue		;
	xr_vector<CObject*>			objects_active		;
	xr_vector<CObject*>			objects_sleeping	;
	xr_vector<CObject*>			objects_active_dup	;
public:
	// methods
								CObjectList			( );
								~CObjectList		( );

	CObject*					FindObjectByName	( shared_str	name	);
	CObject*					FindObjectByName	( LPCSTR		name	);
	CObject*					FindObjectByCLS_ID	( CLASS_ID		cls		);

	void						Load				( );
	void						Unload				( );

	CObject*					Create				( LPCSTR		name	);
	void						Destroy				( CObject*		O		);

	void						SingleUpdate		( CObject*		O		);
	void						Update				( );

	void						net_Register		( CObject*		O		);
	void						net_Unregister		( CObject*		O		);

	u32							net_Export			( NET_Packet*	P,		u32 _start, u32 _count	);	// return next start
	void						net_Import			( NET_Packet*	P		);
	CObject*					net_Find			( u32 ID				);

	void						o_remove			( xr_vector<CObject*>&	v,  CObject*	O);
	void						o_activate			( CObject*		O		);
	void						o_sleep				( CObject*		O		);
	IC u32						o_count				()	{ return objects_active.size()+objects_sleeping.size(); };
	IC CObject*					o_get_by_iterator	(u32 _it)	{
		if (_it<objects_active.size())	return objects_active	[_it];
		else							return objects_sleeping	[_it-objects_active.size()];
	}
};

#endif //__XR_OBJECT_LIST_H__
