#pragma once
#include "map_location.h"
#include "object_interfaces.h"

struct SLocationKey : public IPureSerializeObject<IReader,IWriter> {
	shared_str		spot_type;
	u16				object_id;
	CMapLocation*	location;
	bool			actual;
	SLocationKey (shared_str s, u16 id):spot_type(s),object_id(id),location(NULL),actual(true){};

	bool operator < (const SLocationKey& key)const{
		return  actual > key.actual;} //move non-actual to tail
	
	virtual void save(IWriter &stream)
	{
		stream.w		(&object_id,sizeof(object_id));
		stream.w_u16	(location->RefCount());
		stream.w_stringZ(location->GetHint());
		stream.w_stringZ(spot_type);
		
	}
	
	virtual void load(IReader &stream)
	{
		stream.r		(&object_id,sizeof(object_id));
		u16 c =			stream.r_u16();
		string512 hint;
		stream.r_stringZ(hint);
		stream.r_stringZ(spot_type);

		location  = xr_new<CMapLocation>(*spot_type, object_id);
		location->SetHint(hint);
		location->SetRefCount(c);
	}
};

DEFINE_VECTOR (SLocationKey, Locations, Locations_it);

class CMapManager
{
	Locations				m_locations;
	void					force_clean(Locations_it& it);
public:

							CMapManager					();
							~CMapManager				();
	void					Update						();
	void					Cleanup						();

	Locations&				Locations					() {return m_locations;}
	CMapLocation*			AddMapLocation				(LPCSTR spot_type, u16 id);
	void					RemoveMapLocation			(LPCSTR spot_type, u16 id);
	u16						HasMapLocation				(LPCSTR spot_type, u16 id);
	void					RemoveMapLocationByObjectID (u16 id); //call on destroy object
};