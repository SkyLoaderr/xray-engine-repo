//////////////////////////////////////////////////////////////////////////
// character_info.cpp			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_info.h"
#include "PhraseDialog.h"
#include "ui/xrXMLParser.h"


//////////////////////////////////////////////////////////////////////////


SRelation::SRelation()
{
	m_eRelationType = ALife::eRelationTypeDummy;
}
SRelation::~SRelation()
{
}

ALife::ERelationType SRelation::RelationType() const 
{
	return m_eRelationType;
}
void SRelation::SetRelationType(ALife::ERelationType relation) 
{
	m_eRelationType = relation;
}

int SRelation::Goodwill() const
{
	return m_iGoodwill;
}
void SRelation::SetGoodwill(int new_goodwill)
{
	m_iGoodwill = new_goodwill;
}


//////////////////////////////////////////////////////////////////////////
SCharacterProfile::SCharacterProfile()
{
	m_sGameName		= NULL;
	m_iStartDialog  = NO_DIALOG;

	m_iIconX = m_iIconY = -1;
	m_iMapIconX = m_iMapIconY = -1;
}

SCharacterProfile::~SCharacterProfile()
{
}



//////////////////////////////////////////////////////////////////////////

CCharacterInfo::CCharacterInfo()
{
	m_iProfileIndex = NO_PROFILE;

	m_sTeamName		= "NA";
	m_sRank			= "novice";
}


CCharacterInfo::~CCharacterInfo()
{
}


void CCharacterInfo::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "character";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("profiles", "files");
}

void CCharacterInfo::Load(PROFILE_ID id)
{
	Load(id_to_index::IdToIndex(id));
}

void CCharacterInfo::Load(PROFILE_INDEX index)
{
	m_iProfileIndex = index;
	inherited_shared::load_shared(m_iProfileIndex, NULL);
}


void CCharacterInfo::load_shared	(LPCSTR)
{
	CUIXml uiXml;
	const id_to_index::ITEM_DATA& item_data = id_to_index::GetByIndex(m_iProfileIndex);

	string128 xml_file_full;
	strconcat(xml_file_full, *ref_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* item_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(item_node, "profile id=", *ref_str(item_data.id));

	uiXml.SetLocalRoot(item_node);

	//игровое имя персонажа
	data()->m_sGameName		= uiXml.Read("name", 0, NULL);
	//!!! временно, пока нет соответствующих RPG компонент
	m_sTeamName		= uiXml.Read("team", 0, NULL);
	m_sRank			= uiXml.Read("rank", 0, NULL);
	
	LPCSTR start_dialog = uiXml.Read("start_dialog", 0, NULL);
	if(start_dialog)
		data()->m_iStartDialog	= CPhraseDialog::IdToIndex(start_dialog);
	else
		data()->m_iStartDialog	= NO_DIALOG;

	data()->m_iIconX		= uiXml.ReadAttribInt("icon", 0, "x");
	data()->m_iIconY		= uiXml.ReadAttribInt("icon", 0, "y");
	data()->m_iMapIconX		= uiXml.ReadAttribInt("map_icon", 0, "x");
	data()->m_iMapIconY		= uiXml.ReadAttribInt("map_icon", 0, "y");
}


LPCSTR CCharacterInfo::Name() const 
{
	return	*data()->m_sGameName;
}
LPCSTR CCharacterInfo::Rank() const 
{
	return	*m_sRank;
}
LPCSTR CCharacterInfo::Community() const 
{
	return	*m_sTeamName;
}

ALife::ERelationType  CCharacterInfo::GetRelationType	(u16 person_id) const 
{
	const RELATION_MAP* relation_map = relation_registry.objects_ptr();

	if(relation_map)
	{
		RELATION_MAP::const_iterator it = relation_map->find(person_id);
		if(relation_map->end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.RelationType();
		}
	}
	return ALife::eRelationTypeDummy;
}
void CCharacterInfo::SetRelationType	(u16 person_id, ALife::ERelationType new_relation)
{
	RELATION_MAP& relation_map = relation_registry.objects();
	relation_map[person_id].SetRelationType(new_relation);
}


int  CCharacterInfo::GetGoodwill (u16 person_id) const 
{
	const RELATION_MAP* relation_map = relation_registry.objects_ptr();

	if(relation_map)
	{
		RELATION_MAP::const_iterator it = relation_map->find(person_id);
		if(relation_map->end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
		return NO_GOODWILL;
}
void CCharacterInfo::SetGoodwill	(u16 person_id, int goodwill)
{
	RELATION_MAP& relation_map = relation_registry.objects();
	relation_map[person_id].SetGoodwill(goodwill);
}


void CCharacterInfo::ClearRelations	()
{
	const RELATION_MAP* relation_map = relation_registry.objects_ptr();
	if(relation_map)
	{
		relation_registry.objects().clear();
	}
}