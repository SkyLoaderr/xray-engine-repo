#include "stdafx.h"
#include "map_manager.h"
#include "alife_registry_wrappers.h"

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


void SLocationKey::save(IWriter &stream)
{
	stream.w		(&object_id,sizeof(object_id));
	stream.w_u16	(location->RefCount());
	stream.w_stringZ(location->GetHint());
	stream.w_stringZ(spot_type);
}
	
void SLocationKey::load(IReader &stream)
{
	stream.r		(&object_id,sizeof(object_id));
	u16 c =			stream.r_u16();
	xr_string		hint;
	stream.r_stringZ(hint);
	stream.r_stringZ(spot_type);

	location  = xr_new<CMapLocation>(*spot_type, object_id);
	location->SetHint(hint.c_str());
	location->SetRefCount(c);
}

void CMapLocationRegistry::save(IWriter &stream)
{
	stream.w_u32			((u32)objects().size());
	iterator				I = m_objects.begin();
	iterator				E = m_objects.end();
	for ( ; I != E; ++I) {
		u32					size = 0;
		Locations::iterator	i = (*I).second.begin();
		Locations::iterator	e = (*I).second.end();
		for ( ; i != e; ++i) {
			VERIFY			((*i).location);
			if ((*i).location->Serializable())
				++size;
		}
		stream.w			(&(*I).first,sizeof((*I).first));
		stream.w_u32		(size);
		i					= (*I).second.begin();
		for ( ; i != e; ++i)
			if ((*i).location->Serializable())
				(*i).save	(stream);
	}
}


CMapManager::CMapManager()
{
	m_locations = xr_new<CMapLocationWrapper>();
}

CMapManager::~CMapManager()
{
	xr_delete(m_locations);
}

void CMapManager::initialize(u16 id)
{
	m_locations->registry().init(id);// actor's id
}

CMapLocation* CMapManager::AddMapLocation(LPCSTR spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it == Locations().end() ){
		CMapLocation* l = xr_new<CMapLocation>(*key.spot_id, key.object_id);
		Locations().push_back( SLocationKey(key.spot_id, key.object_id) );
		Locations().back().location = l;
		return l;
	}else
		(*it).location->AddRef();

	return (*it).location;
}


void CMapManager::RemoveMapLocation(LPCSTR spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it!=Locations().end() ){

		if( 1==(*it).location->RefCount() ){
			xr_delete				((*it).location);		
			Locations().erase		(it);
		}else
			(*it).location->Release();
	}
}

void CMapManager::RemoveMapLocationByObjectID(u16 id) //call on destroy object
{
	FindLocationByID key(id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	while( it!= Locations().end() ){
		xr_delete				((*it).location);		
		Locations().erase		(it);

		it = std::find_if(Locations().begin(),Locations().end(),key);
	}
}

u16 CMapManager::HasMapLocation(LPCSTR spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it!=Locations().end() )
		return (*it).location->RefCount();
	
	return 0;
}

void CMapManager::Cleanup()//force
{
	Locations_it it = Locations().begin();
	for(; it!=Locations().end();++it){
		xr_delete		((*it).location);	
	}
	Locations().clear();
}

void CMapManager::Update()
{
	Locations_it it = Locations().begin();
	for(; it!=Locations().end();++it){
		(*it).actual = (*it).location->Update();
	}
	std::sort( Locations().begin(),Locations().end() );

	while( (!Locations().empty())&&(!Locations().back().actual) ){
		xr_delete(Locations().back().location);
		Locations().pop_back();
	}
}

Locations&	CMapManager::Locations	() 
{
	return m_locations->registry().objects();
}
