#include "stdafx.h"
#include "map_location.h"
#include "map_spot.h"

#include "level.h"
#include "../xr_object.h"
#include "ai_space.h"
#include "game_graph.h"
#include "xrServer.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMapWnd.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "relation_registry.h"
#include "InventoryOwner.h"
//////////////////////////////////////////////////
CMapLocation::CMapLocation(LPCSTR type, u16 object_id)
{
	m_flags.zero			();
	m_level_spot			= NULL;
	m_level_spot_pointer	= NULL;
	m_minimap_spot			= NULL;
	m_minimap_spot_pointer	= NULL;
	m_objectID				= object_id;
	m_actual_time			= 0;

	LoadSpot				(type, false);
	m_refCount				= 1;
}

CMapLocation::~CMapLocation()
{
	xr_delete(m_level_spot);
	xr_delete(m_level_spot_pointer);
	xr_delete(m_minimap_spot);
	xr_delete(m_minimap_spot_pointer);

}

CUIXml	g_uiSpotXml;
bool	g_uiSpotXmlInited = false;
void CMapLocation::LoadSpot(LPCSTR type, bool bReload)
{
	if(!g_uiSpotXmlInited){
		bool xml_result			= g_uiSpotXml.Init(CONFIG_PATH, UI_PATH, "map_spots.xml");
		R_ASSERT3(xml_result, "xml file not found", "map_spots.xml");
		g_uiSpotXmlInited = true;
	}

	XML_NODE* node = NULL;
	string512 path_base, path;
//	strconcat(path_base,"map_spots:",type);
	strcpy(path_base,type);
	R_ASSERT3(g_uiSpotXml.NavigateToNode(path_base,0), "XML node not found in file map_spots.xml", path_base);
	LPCSTR s = g_uiSpotXml.ReadAttrib(path_base, 0, "hint", "no hint");
	SetHint(s);
	
	s = g_uiSpotXml.ReadAttrib(path_base, 0, "store", NULL);
	if(s)
		m_flags.set( eSerailizable, TRUE);

	s = g_uiSpotXml.ReadAttrib(path_base, 0, "no_offline", NULL);
	if(s)
		m_flags.set( eHideInOffline, TRUE);

	m_ttl = g_uiSpotXml.ReadAttribInt(path_base, 0, "ttl", 0);
	if(m_ttl>0){
		m_flags.set( eTTL, TRUE);
		m_actual_time = Device.dwTimeGlobal+m_ttl*1000;
	}

	s = g_uiSpotXml.ReadAttrib(path_base, 0, "pos_to_actor", NULL);
	if(s)
		m_flags.set( ePosToActor, TRUE);


	strconcat(path,path_base,":level_map");
	node = g_uiSpotXml.NavigateToNode(path,0);
	if(node){
		LPCSTR str = g_uiSpotXml.ReadAttrib(path, 0, "spot", "");
		if( xr_strlen(str) ){
			if(!bReload)
				m_level_spot = xr_new<CMapSpot>(this);
			m_level_spot->Load(&g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_level_spot) );
		}

		str = g_uiSpotXml.ReadAttrib(path, 0, "pointer", "");
		if( xr_strlen(str) ){
			if(!bReload)
				m_level_spot_pointer = xr_new<CMapSpotPointer>(this);
			m_level_spot_pointer->Load(&g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_level_spot_pointer) );
		}
	};

	strconcat(path,path_base,":mini_map");
	node = g_uiSpotXml.NavigateToNode(path,0);
	if(node){
		LPCSTR str = g_uiSpotXml.ReadAttrib(path, 0, "spot", "");
		if( xr_strlen(str) ){
			if(!bReload)
				m_minimap_spot = xr_new<CMiniMapSpot>(this);
			m_minimap_spot->Load(&g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_minimap_spot) );
		}

		str = g_uiSpotXml.ReadAttrib(path, 0, "pointer", "");
		if( xr_strlen(str) ){
			if(!bReload)
				m_minimap_spot_pointer = xr_new<CMapSpotPointer>(this);
			m_minimap_spot_pointer->Load(&g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_minimap_spot_pointer) );
		}
	};
}

Fvector2 CMapLocation::Position()
{
	Fvector2 pos;
	pos.set(0.0f,0.0f);

	if(m_flags.test( ePosToActor)){
		m_position_global = Level().CurrentEntity()->Position();
		pos.set(m_position_global.x, m_position_global.z);
		return pos;
	}

	CObject* pObject =  Level().Objects.net_Find(m_objectID);
	if(!pObject){
		if(ai().get_alife())		
		{
			CSE_ALifeDynamicObject* O = ai().alife().objects().object(m_objectID,true);
			if(O){
				m_position_global = O->Position();
				pos.set(m_position_global.x, m_position_global.z);
			}
		}
	
	}else{
		m_position_global = pObject->Position();
		pos.set(m_position_global.x, m_position_global.z);
	}

	return pos;
}

Fvector2 CMapLocation::Direction()
{
	Fvector2 res;
	res.set(0.0f,0.0f);

	if(Level().CurrentViewEntity()&&Level().CurrentViewEntity()->ID()==m_objectID ){
		res.set(Device.vCameraDirection.x,Device.vCameraDirection.z);
	}else{
		CObject* pObject =  Level().Objects.net_Find(m_objectID);
		if(!pObject)
			res.set(0.0f, 0.0f);
		else{
			const Fvector& op = pObject->Direction();
			res.set(op.x, op.z);
		}
	}

	if(m_flags.test(ePosToActor)){
		CObject* pObject =  Level().Objects.net_Find(m_objectID);
		if(pObject){
			Fvector2 dcp,obj_pos;
			dcp.set(Device.vCameraPosition.x, Device.vCameraPosition.z);
			obj_pos.set(pObject->Position().x, pObject->Position().z);
			res.sub(obj_pos, dcp);
			res.normalize_safe();
		}
		
	}
	
	return res;
}

shared_str CMapLocation::LevelName()
{
	if(ai().get_alife() && ai().get_game_graph())		
	{
		CSE_Abstract* E = Level().Server->game->get_entity_from_eid(m_objectID); //VERIFY(E);
		if(!E){
			Msg("! Critical: SMapLocation binded to non-existent object id=%d",m_objectID);
			return "ERROR";
		}
		
		CSE_ALifeObject* AO = smart_cast<CSE_ALifeObject*>(E);
		if(AO)	
			return  ai().game_graph().header().level(ai().game_graph().vertex(AO->m_tGraphID)->level_id()).name();
		else	
			return Level().name();
	}
	else
		return Level().name();
}

bool CMapLocation::Update() //returns actual
{
	if(	m_flags.test(eTTL) ){
		if( m_actual_time < Device.dwTimeGlobal)
			return false;
	}

	CObject* pObject =  Level().Objects.net_Find(m_objectID);
	
	//mp
	if ( GameID()!=GAME_SINGLE && (pObject) ) return true;
	
	//single
	if(pObject) return true; // online

	if(ai().get_alife())		
	{
		return ( NULL != ai().alife().objects().object(m_objectID,true) );
	}


	return false;
}

void CMapLocation::UpdateSpot(CUICustomMap* map, CMapSpot* sp )
{
	if( map->MapName()==LevelName() ){
		CSE_ALifeDynamicObject* obj = NULL;
		
		if(ai().get_alife()){
			obj = ai().alife().objects().object(m_objectID);
			VERIFY(obj);
		}
		
		if(	m_flags.test(eHideInOffline) && 
			ai().get_alife() &&
			!obj->m_bOnline )
			return;

		if(ai().get_alife() && FALSE == obj->m_flags.test(CSE_ALifeObject::flVisibleForMap))
			return;

		//update spot position
		Fvector2 position = Position();

		m_position_on_map =	map->ConvertRealToLocal(position);

		sp->SetWndPos(m_position_on_map);
		Frect wnd_rect = sp->GetWndRect();

		if( map->IsRectVisible(wnd_rect) ) {

			//update heading if needed
			if( sp->Heading() ){
				Fvector2 dir_global = Direction();
				float h = dir_global.getH();
				float h_ = map->GetHeading()+h;
				sp->SetHeading( h_ );
			}

			Frect clip_rect = map->GetClipperRect();
			sp->SetClipRect( clip_rect );
			map->AttachChild(sp);
		}
		if( GetSpotPointer(sp) && map->NeedShowPointer(wnd_rect)){
			UpdateSpotPointer( map, GetSpotPointer(sp) );
		}

	};
}

void CMapLocation::UpdateSpotPointer(CUICustomMap* map, CMapSpotPointer* sp )
{
	float		heading;
	Fvector2	pointer_pos;
	map->GetPointerTo(m_position_on_map, sp->GetWidth()/2, pointer_pos, heading);

	sp->SetWndPos(pointer_pos);
	sp->SetHeading(heading);

	Frect clip_rect = map->GetClipperRect();
	sp->SetClipRect( clip_rect );
	map->AttachChild(sp);
}

void CMapLocation::UpdateMiniMap(CUICustomMap* map)
{
	CMapSpot* sp = m_minimap_spot;
	if(!sp) return;
	UpdateSpot(map, sp);

}

void CMapLocation::UpdateLevelMap(CUICustomMap* map)
{
	CMapSpot* sp = m_level_spot;
	if(!sp) return;
	UpdateSpot(map, sp);
}

u16	CMapLocation::AddRef() 
{
	++m_refCount; 
	if(	m_flags.test(eTTL) ){
		m_actual_time = Device.dwTimeGlobal+m_ttl*1000;
	}

	return m_refCount;
};

CMapSpotPointer* CMapLocation::GetSpotPointer(CMapSpot* sp)
{
	R_ASSERT(sp);
	if(sp==m_level_spot)
		return m_level_spot_pointer;
	else
	if(sp==m_minimap_spot)
		return m_minimap_spot_pointer;

	return NULL;
}

CRelationMapLocation::CRelationMapLocation			(const shared_str& type, u16 object_id, u16 pInvOwnerActorID, u16 pInvOwnerEntityID)
:CMapLocation(*type,object_id)
{
	m_curr_spot_name	= type;
	m_pInvOwnerEntityID	= pInvOwnerEntityID;
	m_pInvOwnerActorID	= pInvOwnerActorID;
}

CRelationMapLocation::~CRelationMapLocation			()
{}

bool CRelationMapLocation::Update()
{
	if (false==inherited::Update() ) return false;

	ALife::ERelationType relation = ALife::eRelationTypeFriend;

	if(ai().get_alife())		
	{
		CSE_ALifeTraderAbstract*	pEnt = NULL;
		CSE_ALifeTraderAbstract*	pAct = NULL;
		pEnt = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(m_pInvOwnerEntityID,true));
		pAct = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(m_pInvOwnerActorID,true));
		if(!pEnt || !pAct)	return false;
		relation =  RELATION_REGISTRY().GetRelationType(pEnt, pAct);
	}else{
		CInventoryOwner*			pEnt = NULL;
		CInventoryOwner*			pAct = NULL;

		pEnt = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(m_pInvOwnerEntityID));
		pAct = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(m_pInvOwnerActorID));
		if(!pEnt || !pAct)	return false;
		relation =  RELATION_REGISTRY().GetRelationType(pEnt, pAct);
	}

	const shared_str& sname = RELATION_REGISTRY().GetSpotName(relation);
	if(m_curr_spot_name != sname){
		LoadSpot(*sname, true);
		m_curr_spot_name = sname;
	}
	return true;
}

