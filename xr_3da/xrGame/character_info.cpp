//////////////////////////////////////////////////////////////////////////
// character_info.h			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_info.h"
#include "ui/xrXMLParser.h"

CCharacterInfo::CCharacterInfo()
{
	m_sGameName		= "noname";
	m_sTeamName		= "NA";
	m_sRank			= "novice";
}

//////////////////////////////////////////////////////////////////////////

CCharacterInfo::~CCharacterInfo()
{
}

//////////////////////////////////////////////////////////////////////////

bool CCharacterInfo::Load(LPCSTR name_id, LPCSTR xml_file)
{
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init("$game_data$", xml_file);
	R_ASSERT2(xml_result, "xml file not found");
	if (!xml_result) return false;

	int charInfosCount = xml_doc.GetNodesNum("", 0, "character");
	
	for (int i = 0; i < charInfosCount; ++i)
	{
		if (0 == xr_strcmp(xml_doc.ReadAttrib("character", i, "name_id", ""), name_id))
		{
			XML_NODE* tab_node	= xml_doc.NavigateToNode("character", i);
			xml_doc.SetLocalRoot(tab_node);

			m_sGameName			= xml_doc.Read("name", 0, NULL);
			m_sTeamName			= xml_doc.Read("team", 0, NULL);
			m_sRank				= xml_doc.Read("rank", 0, NULL);
			m_sVisualName		= xml_doc.Read("visual", 0, NULL);
			m_iIconX			= xml_doc.ReadAttribInt("icon", 0, "x");
			m_iIconY			= xml_doc.ReadAttribInt("icon", 0, "y");
			m_iMapIconX			= xml_doc.ReadAttribInt("map_icon", 0, "x");
			m_iMapIconY			= xml_doc.ReadAttribInt("map_icon", 0, "y");
			return true;
		}
	}

	return false;
}


LPCSTR CCharacterInfo::Name() const 
{
	return	*m_sGameName;
}
LPCSTR CCharacterInfo::Rank() const 
{
	return	*m_sRank;
}
LPCSTR CCharacterInfo::Community() const 
{
	return	*m_sTeamName;
}

ALife::ERelationType  CCharacterInfo::GetRelation	(u16 person_id) const 
{
	RELATION_MAP::const_iterator it = m_RelationMap.find(person_id);
	if(m_RelationMap.end() != it)
		return (*it).second;
	else
		return ALife::eRelationTypeDummy;
}
void CCharacterInfo::SetRelation	(u16 person_id, ALife::ERelationType new_relation)
{
	m_RelationMap[person_id] = new_relation;
}

void CCharacterInfo::ClearRelations	()
{
	m_RelationMap.clear();
}