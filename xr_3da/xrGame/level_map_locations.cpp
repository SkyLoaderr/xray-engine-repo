// level_map_locations.cpp:  отображение объектов на карте
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "map_location.h"
#include "character_info.h"
#include "inventoryowner.h"
#include "entity.h"
#include "level.h"

void CLevel::AddMapLocation(const SMapLocation& map_location)
{
	SMapLocation* pMapLocation = xr_new<SMapLocation>();
	*pMapLocation = map_location;
	m_MapLocationVector.push_back(pMapLocation);
}



class RemoveLocationByObjectPred
{
public:
	RemoveLocationByObjectPred(u16 id){object_id = id;}
	bool operator() (SMapLocation* map_location)
	{
		if(map_location->object_id == object_id)
			return true;
		else
			return false;
	}
private:
	u16 object_id;
};

bool CLevel::RemoveMapLocationByID(u16 object_id)
{
	RemoveLocationByObjectPred pred(object_id);
	LOCATIONS_PTR_VECTOR_IT last_it =  std::remove_if(m_MapLocationVector.begin(), m_MapLocationVector.end(), pred);

	if(m_MapLocationVector.end()!=last_it)
	{
		m_MapLocationVector.erase(last_it, m_MapLocationVector.end());
		return true;
	}
	return false;
}


class RemoveLocationByInfoPred
{
public:
	RemoveLocationByInfoPred(int index){info_portion_index = index;}
	bool operator() (SMapLocation* map_location)
	{
		if(map_location->info_portion_index == info_portion_index)
			return true;
		else
			return false;
	}
private:
	int info_portion_index;
};


bool CLevel::RemoveMapLocationByInfo(int info_portion_index)
{
	RemoveLocationByInfoPred pred(info_portion_index);
	LOCATIONS_PTR_VECTOR_IT last_it =  std::remove_if(m_MapLocationVector.begin(), m_MapLocationVector.end(), pred);

	if(m_MapLocationVector.end()!=last_it)
	{
		m_MapLocationVector.erase(last_it, m_MapLocationVector.end());
		return true;
	}
	return false;
}
void  CLevel::AddObjectMapLocation		(const CGameObject* object)
{
	const CEntity* entity = dynamic_cast<const CEntity*>(object);
	if(!entity) return;

	SMapLocation map_location;
	map_location.attached_to_object = true;
	map_location.object_id = entity->ID();
	map_location.icon_width = map_location.icon_height = 1;	

	const CInventoryOwner* pInvOwner = dynamic_cast<const CInventoryOwner*>(entity);
	if(pInvOwner)
	{
		map_location.icon_x = pInvOwner->CharacterInfo().MapIconX();
		map_location.icon_y = pInvOwner->CharacterInfo().MapIconY();
		map_location.name.SetText(pInvOwner->CharacterInfo().Name());
	}
	else
	{
		map_location.icon_x = entity->GetMapIconX();
		map_location.icon_y = entity->GetMapIconY();
		map_location.name.SetText(*entity->cName());
	}
	map_location.text.SetText("");
	AddMapLocation(map_location);
}