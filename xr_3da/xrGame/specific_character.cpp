//////////////////////////////////////////////////////////////////////////
// specific_character.cpp:	игровая информация для о конкретном 
//							персонажей в игре
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "specific_character.h"
#include "PhraseDialog.h"

//////////////////////////////////////////////////////////////////////////
SSpecificCharacterData::SSpecificCharacterData()
{
	m_sGameName		= NULL;
	m_iStartDialog	= NO_DIALOG;
	m_iActorDialog	= NO_DIALOG; 

	m_iIconX = m_iIconY = -1;
	m_iMapIconX = m_iMapIconY = -1;

	m_Rank = NO_RANK;
	m_Reputation = NO_REPUTATION;
}

SSpecificCharacterData::~SSpecificCharacterData()
{
}




//////////////////////////////////////////////////////////////////////////

CSpecificCharacter::CSpecificCharacter()
{
	m_iOwnIndex = NO_SPECIFIC_CHARACTER;
}


CSpecificCharacter::~CSpecificCharacter()
{
}


void CSpecificCharacter::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "specific_character";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("profiles", "specific_characters_files");
}

void CSpecificCharacter::Load(PROFILE_ID id)
{
	Load(id_to_index::IdToIndex(id));
}

void CSpecificCharacter::Load(PROFILE_INDEX index)
{
	m_iOwnIndex = index;
	inherited_shared::load_shared(m_iOwnIndex, NULL);
}


void CSpecificCharacter::load_shared	(LPCSTR)
{
	CUIXml uiXml;
	const id_to_index::ITEM_DATA& item_data = id_to_index::GetByIndex(m_iOwnIndex);

	string128 xml_file_full;
	strconcat(xml_file_full, *ref_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* item_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(item_node, "specific_character id=", *ref_str(item_data.id));

	uiXml.SetLocalRoot(item_node);


	/////////////////////////////////////////////////
	

	LPCSTR start_dialog = uiXml.Read("start_dialog", 0, NULL);
	if(start_dialog)
		data()->m_iStartDialog	= CPhraseDialog::IdToIndex(start_dialog);
	else
		data()->m_iStartDialog	= NO_DIALOG;

	LPCSTR actor_dialog = uiXml.Read("actor_dialog", 0, NULL);
	if(actor_dialog)
		data()->m_iActorDialog	= CPhraseDialog::IdToIndex(actor_dialog);
	else
		data()->m_iActorDialog	= NO_DIALOG;

	data()->m_iIconX		= uiXml.ReadAttribInt("icon", 0, "x");
	data()->m_iIconY		= uiXml.ReadAttribInt("icon", 0, "y");
	data()->m_iMapIconX		= uiXml.ReadAttribInt("map_icon", 0, "x");
	data()->m_iMapIconY		= uiXml.ReadAttribInt("map_icon", 0, "y");

	//игровое имя персонажа
	data()->m_sGameName		= uiXml.Read("name", 0, NULL);

	data()->m_Community		= uiXml.Read("team", 0, *NO_COMMUNITY);
	R_ASSERT3(data()->m_Community != NO_COMMUNITY, "not all fields fulfiled for specific character", *IndexToId(m_iOwnIndex));
	data()->m_Rank			= uiXml.ReadInt("rank", 0, NO_RANK);
	R_ASSERT3(data()->m_Rank != NO_RANK, "not all fields fulfiled for specific character", *IndexToId(m_iOwnIndex));
	data()->m_Reputation	= uiXml.ReadInt("reputation", 0, NO_REPUTATION);
	R_ASSERT3(data()->m_Reputation != NO_REPUTATION, "not all fields fulfiled for specific character", *IndexToId(m_iOwnIndex));
}


LPCSTR CSpecificCharacter::Name() const 
{
	return	*data()->m_sGameName;
}
CHARACTER_RANK CSpecificCharacter::Rank() const 
{
	return	data()->m_Rank;
}
LPCSTR CSpecificCharacter::Community() const 
{
	return	*data()->m_Community;
}

CHARACTER_REPUTATION	 CSpecificCharacter::Reputation	() const 
{
	return data()->m_Reputation;
}
