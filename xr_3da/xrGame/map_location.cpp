#include "stdafx.h"
#include "map_location.h"
#include "map_spot.h"

#include "level.h"
#include "../xr_object.h"
#include "ai_space.h"
#include "game_graph.h"
#include "xrServer.h"
#include "xrServer_Objects_ALife.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMapWnd.h"


//////////////////////////////////////////////////
CMapLocation::CMapLocation(LPCSTR type, u16 object_id)
{
	ZeroMemory(m_hint, sizeof(m_hint) );
	m_flags.zero			();
	m_level_spot			= NULL;
	m_zonemap_spot			= NULL;
	m_objectID				= object_id;

	LoadSpot				(type);
	m_refCount				= 1;
}

CMapLocation::~CMapLocation()
{
	xr_delete(m_level_spot);
	xr_delete(m_zonemap_spot);
	VERIFY(m_refCount==0);
}

void CMapLocation::LoadSpot(LPCSTR type)
{
	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, "map_spots.xml");
	R_ASSERT3(xml_result, "xml file not found", "map_spots.xml");

	XML_NODE* node = NULL;
	string512 path_base, path;
//	strconcat(path_base,"map_spots:",type);
	strcpy(path_base,type);
	R_ASSERT3(uiXml.NavigateToNode(path_base,0), "XML node not found in file map_spots.xml", path_base);
	LPCSTR hint = uiXml.ReadAttrib(path_base, 0, "hint", "no hint");
	SetHint(hint);
	
	strconcat(path,path_base,":level_map");
	node = uiXml.NavigateToNode(path,0);
	if(node){
		m_level_spot = xr_new<CLevelMapSpot>(this);
		m_level_spot->Load(&uiXml,path);
	};

	strconcat(path,path_base,":zone_map");
	node = uiXml.NavigateToNode(path,0);
	if(node){
		m_zonemap_spot = xr_new<CZoneMapSpot>(this);
		m_zonemap_spot->Load(&uiXml,path);
	};
}

Fvector2 CMapLocation::Position()
{
	Fvector2 pos;
	CObject* pObject =  Level().Objects.net_Find(m_objectID);
	const Fvector& op = pObject->Position();
	pos.set(op.x, op.z);
	return pos;
}

Fvector2 CMapLocation::Direction()
{
	Fvector2 dir;
	CObject* pObject =  Level().Objects.net_Find(m_objectID);
	const Fvector& op = pObject->Direction();
	dir.set(op.x, op.z);
	return dir;
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
	return true;
}



/*


//////////////////////////////////////////////////////////////////////////

SMapLocation::SMapLocation():animation(&icon_color)
{
	info_portion_id				= NO_INFO_INDEX;
	level_name					= NULL;
	x							= y = 0;
	name						= NULL;
	text						= NULL;
	attached_to_object			= false;
	object_id					= 0xffff;
	marker_show					= false;
	big_icon					= false;
	global_map_text				= false;
	icon_color					= 0xffffffff;
	dynamic_manifestation		= false;
	type_flags.zero				();
	animation.Cyclic			(false);
	m_iconsShader				= NULL;
}

SMapLocation::SMapLocation(const SMapLocation& map_location)
{
	*this  = map_location;
	animation.SetColorToModify(&icon_color);
}

//////////////////////////////////////////////////////////////////////////

shared_str SMapLocation::LevelName()
{
	if(!attached_to_object)
		return level_name;
	else
	{
		if(ai().get_alife() && ai().get_game_graph())		
		{
			CSE_Abstract* E = Level().Server->game->get_entity_from_eid(object_id); //VERIFY(E);
			if(!E){
				Msg("! Critical: SMapLocation binded to non-existent object id=%d",object_id);
				type_flags.zero				();
				return level_name;
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
}

//////////////////////////////////////////////////////////////////////////

void SMapLocation::UpdateAnimation()
{
	animation.Update();
}

//////////////////////////////////////////////////////////////////////////

void SMapLocation::SetColorAnimation(const shared_str &animationName)
{
	animation.SetColorAnimation(animationName);
	animation.Reset();
}

//////////////////////////////////////////////////////////////////////////

ref_shader&	SMapLocation::GetShader(){
	if (xr_strlen(shader_name) > 0)
        if(!m_iconsShader)
		{
			m_iconsShader.create("hud\\default", *shader_name);
		}

	return m_iconsShader;
}
*/