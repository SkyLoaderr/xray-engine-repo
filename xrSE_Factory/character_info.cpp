//////////////////////////////////////////////////////////////////////////
// character_info.cpp			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_info.h"

#ifdef XRGAME_EXPORTS
#	include "ui/xrXMLParser.h"
#else
#	include "xrXMLParser.h"
#endif


#ifdef XRGAME_EXPORTS

#include "PhraseDialog.h"
#include "alife_registry_container_composition.h"

#endif


//////////////////////////////////////////////////////////////////////////
SCharacterProfile::SCharacterProfile()
{
	m_CharacterId		= NULL;
	m_Rank				= NO_RANK;
	m_Reputation		= NO_REPUTATION;
}

SCharacterProfile::~SCharacterProfile()
{
}



//////////////////////////////////////////////////////////////////////////

CCharacterInfo::CCharacterInfo()
{
	m_ProfileId = NULL;
	m_SpecificCharacterId = NULL;

#ifdef XRGAME_EXPORTS
	m_CurrentRank.set(NO_RANK);
	m_CurrentReputation.set(NO_REPUTATION);
	m_StartDialog = NULL;
#endif
}


CCharacterInfo::~CCharacterInfo()
{
}


void CCharacterInfo::Load(PROFILE_ID id)
{
	m_ProfileId = id;
	inherited_shared::load_shared(m_ProfileId, NULL);
}

#ifdef XRGAME_EXPORTS

void CCharacterInfo::InitSpecificCharacter (SPECIFIC_CHARACTER_ID new_id)
{
	R_ASSERT(new_id.size());
	m_SpecificCharacterId = new_id;

	m_SpecificCharacter.Load(m_SpecificCharacterId);
	if(Rank().value() == NO_RANK)
		SetRank(m_SpecificCharacter.Rank());
	if(Reputation().value() == NO_REPUTATION)
		SetReputation(m_SpecificCharacter.Reputation());
	if(Community().index() == NO_COMMUNITY_INDEX)
		SetCommunity(m_SpecificCharacter.Community());
	if(!m_StartDialog || !m_StartDialog.size() )
		m_StartDialog = m_SpecificCharacter.data()->m_StartDialog;
}


#endif

void CCharacterInfo::load_shared	(LPCSTR)
{
	CUIXml uiXml;
	const id_to_index::ITEM_DATA& item_data = *id_to_index::GetById(m_ProfileId);

	string128 xml_file_full;
	strconcat(xml_file_full, *shared_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init(CONFIG_PATH, GAME_PATH, xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* item_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(item_node, "profile id=", *item_data.id);

	uiXml.SetLocalRoot(item_node);



	LPCSTR spec_char = uiXml.Read("specific_character", 0, NULL);
	if(!spec_char)
	{
		data()->m_CharacterId	= NULL;
		
		LPCSTR char_class			= uiXml.Read	("class",		0,	NULL);

		if(char_class)
		{
			char* buf_str = xr_strdup(char_class);
			xr_strlwr(buf_str);
			data()->m_Class				= buf_str;
			xr_free(buf_str);
		}
		else
			data()->m_Class				= NO_CHARACTER_CLASS;
			
		data()->m_Rank = uiXml.ReadInt	("rank",		0,	NO_RANK);
		data()->m_Reputation = uiXml.ReadInt	("reputation",	0,	NO_REPUTATION);
	}
	else
		data()->m_CharacterId = spec_char;
}

#ifdef XRGAME_EXPORTS


PROFILE_ID CCharacterInfo::Profile()			const
{
	return m_ProfileId;
}

LPCSTR CCharacterInfo::Name() const
{
	R_ASSERT2(m_SpecificCharacterId.size(), m_SpecificCharacter.Name());
	return	m_SpecificCharacter.Name();
}

LPCSTR CCharacterInfo::Bio() const
{
	R_ASSERT2(m_SpecificCharacterId.size(), m_SpecificCharacter.Bio());
	return 	m_SpecificCharacter.Bio();
}

const CHARACTER_RANK& CCharacterInfo::Rank() const
{
	return	m_CurrentRank;
}

const CHARACTER_COMMUNITY& CCharacterInfo::Community() const
{
	return	m_CurrentCommunity;
}

const CHARACTER_REPUTATION& CCharacterInfo::Reputation() const
{
	return m_CurrentReputation;
}

void CCharacterInfo::SetRank (CHARACTER_RANK_VALUE rank)
{
	m_CurrentRank.set(rank);
}
void CCharacterInfo::SetReputation (CHARACTER_REPUTATION_VALUE reputation)
{
	m_CurrentReputation.set(reputation);
}

void CCharacterInfo::SetCommunity	(const CHARACTER_COMMUNITY& community)
{
	m_CurrentCommunity = community;
}




int	 CCharacterInfo::TradeIconX() const
{
	R_ASSERT(m_SpecificCharacterId.size());
	return m_SpecificCharacter.TradeIconX();
}
int	 CCharacterInfo::TradeIconY() const
{
	R_ASSERT(m_SpecificCharacterId.size());
	return m_SpecificCharacter.TradeIconY();
}
int	 CCharacterInfo::MapIconX()	 const
{
	R_ASSERT(m_SpecificCharacterId.size());
	return m_SpecificCharacter.MapIconX();
}
int	 CCharacterInfo::MapIconY()	 const
{
	R_ASSERT(m_SpecificCharacterId.size());
	return m_SpecificCharacter.MapIconY();
}

PHRASE_DIALOG_ID	CCharacterInfo::StartDialog	()	const
{
	return m_StartDialog;
}
const DIALOG_ID_VECTOR&	CCharacterInfo::ActorDialogs	()	const
{
	R_ASSERT(m_SpecificCharacterId.size());
	return m_SpecificCharacter.data()->m_ActorDialogs;
}

void CCharacterInfo::load	(IReader& stream)
{
	stream.r_stringZ(m_StartDialog);
}
void CCharacterInfo::save	(NET_Packet& stream)
{
	stream.w_stringZ(m_StartDialog);
}

#endif



void CCharacterInfo::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "character";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("profiles", "files");
}