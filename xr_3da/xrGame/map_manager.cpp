#include "stdafx.h"
#include "map_manager.h"

struct FindLocationBySpotID{
	shared_str	spot_id;
	u16			object_id;
	FindLocationBySpotID(shared_str s, u16 id):spot_id(s),object_id(id){}
	bool operator () (const SLocationKey& key){
		return (spot_id==key.spot_type)&&(object_id==key.object_id);
	}
};
struct FindLocationByID{
	u16			object_id;
	FindLocationByID(u16 id):object_id(id){}
	bool operator () (const SLocationKey& key){
		return (object_id==key.object_id);
	}
};


CMapManager::CMapManager()
{
}

CMapManager::~CMapManager()
{
}

CMapLocation* CMapManager::AddMapLocation(LPCSTR spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(m_locations.begin(),m_locations.end(),key);
	if( it == m_locations.end() ){
		CMapLocation* l = xr_new<CMapLocation>(*key.spot_id, key.object_id);
		m_locations.push_back( SLocationKey(key.spot_id, key.object_id) );
		m_locations.back().location = l;
		return l;
	}else
		(*it).location->AddRef();

	return (*it).location;
}

void CMapManager::force_clean(Locations_it& it)
{
	while( 0 != (*it).location->Release() ){};
	
	xr_delete				((*it).location);		
	m_locations.erase		(it);
}

void CMapManager::RemoveMapLocation(LPCSTR spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(m_locations.begin(),m_locations.end(),key);
	if( it!=m_locations.end() ){

		if( 1==(*it).location->RefCount() )
			force_clean( it );
		else
			(*it).location->Release();
	}
}

void CMapManager::RemoveMapLocationByObjectID(u16 id) //call on destroy object
{
	FindLocationByID key(id);
	Locations_it it = std::find_if(m_locations.begin(),m_locations.end(),key);
	while( it!= m_locations.end() ){
		force_clean( it );
		it = std::find_if(m_locations.begin(),m_locations.end(),key);
	}
}

u16 CMapManager::HasMapLocation(LPCSTR spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(m_locations.begin(),m_locations.end(),key);
	if( it!=m_locations.end() )
		return (*it).location->RefCount();
	
	return 0;
}

void CMapManager::Cleanup()//force
{
	while( !m_locations.empty() )
		force_clean( m_locations.begin()+m_locations.size()-1 );
}

void CMapManager::Update()
{
	Locations_it it = m_locations.begin();
	for(; it!=m_locations.end();++it){
		(*it).actual = (*it).location->Update();
	}
	std::sort( m_locations.begin(),m_locations.end() );
	while( (!m_locations.empty())&&(!m_locations.back().actual) )
		force_clean( m_locations.begin()+m_locations.size()-1 );

}
