// level_map_locations.cpp:  отображение объектов на карте
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "map_location.h"
#include "character_info.h"
#include "inventoryowner.h"
#include "entity.h"
#include "level.h"

void CLevel::AddMapLocation(const SMapLocation& map_location, EMapLocationFlags location_type)
{
	SMapLocation* pMapLocation = NULL;
	
	if(map_location.attached_to_object)
		pMapLocation = GetMapLocationByID(map_location.object_id);

	if(!pMapLocation)
	{
		pMapLocation = xr_new<SMapLocation>(map_location);
		m_MapLocationVector.push_back(pMapLocation);
	}

	pMapLocation->type_flags.set(location_type, TRUE);
}

void CLevel::RemoveMapLocations			()
{
	for(std::size_t i = 0; i < m_MapLocationVector.size(); i++)
		xr_delete(m_MapLocationVector[i]);
	
	m_MapLocationVector.clear();
}


class RemoveLocationByObjectPred
{
public:
	RemoveLocationByObjectPred(u16 id, EMapLocationFlags loctype){object_id = id; location_type = loctype;}
	bool operator() (SMapLocation* map_location)
	{
		if(map_location->object_id == object_id)
		{
			map_location->type_flags.set(location_type, FALSE);
			return true;
		}
		else
			return false;
	}
private:
	u16 object_id;
	EMapLocationFlags location_type;
};

void CLevel::RemoveMapLocationByID(u16 object_id, EMapLocationFlags location_type)
{
	RemoveLocationByObjectPred pred(object_id, location_type);
	std::for_each(m_MapLocationVector.begin(), m_MapLocationVector.end(), pred);
}


class RemoveLocationByInfoPred
{
public:
	RemoveLocationByInfoPred(INFO_INDEX id){info_portion_id = id;}
	bool operator() (SMapLocation* map_location)
	{
		if(map_location->info_portion_id == info_portion_id)
		{
			map_location->type_flags.set(eMapLocationInfoPortion, FALSE);
			return true;
		}
		else
			return false;
	}
private:
	INFO_INDEX info_portion_id;
};


void CLevel::RemoveMapLocationByInfo(INFO_INDEX info_portion_id)
{
	RemoveLocationByInfoPred pred(info_portion_id);
	std::for_each(m_MapLocationVector.begin(), m_MapLocationVector.end(), pred);
}

struct FindLocationByID{
	u16 m_id;
	FindLocationByID(u16 id):m_id(id){};
	bool operator == (const SMapLocation* loc){
		return (loc->object_id == m_id);
	}
};

SMapLocation*  CLevel::GetMapLocationByID(u16 object_id)
{
	FindLocationByID F(object_id);
	LOCATIONS_PTR_VECTOR_IT it = std::find_if(m_MapLocationVector.begin(),m_MapLocationVector.end(),F);
	if( it!=m_MapLocationVector.end() )
		return *it;
	else
		return NULL;
}


void  CLevel::AddEntityMapLocation		(const CGameObject* object, EMapLocationFlags location_type)
{
	const CEntity* entity = smart_cast<const CEntity*>(object);
	if(!entity) return;

	SMapLocation map_location;
	map_location.attached_to_object = true;
	map_location.object_id = entity->ID();
	map_location.icon_width = map_location.icon_height = 1;	

	const CInventoryOwner* pInvOwner = smart_cast<const CInventoryOwner*>(entity);
	if(pInvOwner)
	{
		map_location.icon_x = pInvOwner->CharacterInfo().MapIconX();
		map_location.icon_y = pInvOwner->CharacterInfo().MapIconY();
		map_location.name = pInvOwner->CharacterInfo().Name();
	}
	else
	{
		map_location.icon_x = entity->GetMapIconX();
		map_location.icon_y = entity->GetMapIconY();
		map_location.name = entity->cName();
	}
	map_location.text = "";
	AddMapLocation(map_location, location_type);
}

void  CLevel::AddObjectMapLocationIcon	(const CGameObject* object, EMapLocationFlags location_type, 
										 LPCSTR name, LPCSTR text, 
										 int icon_x, int icon_y)
{
	SMapLocation map_location;
	map_location.attached_to_object = true;
	map_location.object_id = object->ID();
	map_location.icon_width = map_location.icon_height = 1;	
	map_location.icon_x = icon_x;
	map_location.icon_y = icon_y;
	map_location.name = name;
	map_location.text = text;
	AddMapLocation(map_location, location_type);
}



class RemoveLocationByFlags
{
public:
	bool operator() (SMapLocation* map_location)
	{
		if(0 == map_location->type_flags.get())
		{
			xr_delete(map_location);
			return true;
		}
		else
			return false;
	}
};


void   CLevel::UpdateMapLocation			()
{
	LOCATIONS_PTR_VECTOR_IT new_end =  std::remove_if(m_MapLocationVector.begin(), m_MapLocationVector.end(), RemoveLocationByFlags());
	m_MapLocationVector.erase(new_end, m_MapLocationVector.end());

	// Проапдейтить анимацию у оставшихся
	std::for_each(m_MapLocationVector.begin(), m_MapLocationVector.end(), std::mem_fun(&SMapLocation::UpdateAnimation));
}