#pragma once
#include "map_location.h"
#include "object_interfaces.h"
#include "map_location_defs.h"

class CMapLocationWrapper;
class CInventoryOwner;

class CMapManager
{
	CMapLocationWrapper*	m_locations;
public:

							CMapManager					();
							~CMapManager				();
	void					Update						();
	void					initialize					(u16 id);
	Locations&				Locations					();
	CMapLocation*			AddMapLocation				(const shared_str& spot_type, u16 id);
	CMapLocation*			AddRelationLocation			(CInventoryOwner* pInvOwner);
	void					RemoveMapLocation			(const shared_str& spot_type, u16 id);
	u16						HasMapLocation				(const shared_str& spot_type, u16 id);
	void					RemoveMapLocationByObjectID (u16 id); //call on destroy object
};