//////////////////////////////////////////////////////////////////////////
// specific_character.cpp:	игровая информация для о конкретном 
//							персонажей в игре
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "specific_character.h"

#ifdef  XRGAME_EXPORTS
#include "PhraseDialog.h"


//////////////////////////////////////////////////////////////////////////



SSpecificCharacterData::SSpecificCharacterData()
{
	m_sGameName.clear();
	m_sBioText.clear();
	m_sVisual.clear();
	m_sSupplySpawn.clear();
	m_sNpcConfigSect.clear();


	m_StartDialog	= NULL;
	m_ActorDialogs.clear(); 

	m_iIconX = m_iIconY = -1;
	m_iMapIconX = m_iMapIconY = -1;

	m_Rank = NO_RANK;
	m_Reputation = NO_REPUTATION;

	m_bNoRandom = false;
	m_bDefaultForCommunity = false;
}

SSpecificCharacterData::~SSpecificCharacterData()
{
}

#endif


//////////////////////////////////////////////////////////////////////////

CSpecificCharacter::CSpecificCharacter()
{
	m_OwnId = NULL;
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

void CSpecificCharacter::Load(SPECIFIC_CHARACTER_ID id)
{
	R_ASSERT(id.size());
	m_OwnId = id;
	inherited_shared::load_shared(m_OwnId, NULL);
}


void CSpecificCharacter::load_shared	(LPCSTR)
{
	CUIXml uiXml;
	const id_to_index::ITEM_DATA& item_data = *id_to_index::GetById(m_OwnId);

	string_path xml_file_full;
	strconcat	(xml_file_full, *shared_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init(CONFIG_PATH, GAME_PATH, xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* item_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	R_ASSERT3(item_node, "specific_character id=", *item_data.id);

	uiXml.SetLocalRoot(item_node);



	int norandom = uiXml.ReadAttribInt(item_node, "no_random", 0);
	if (1 == norandom) 
		data()->m_bNoRandom = true;
	else
		data()->m_bNoRandom = false;

	int team_default = uiXml.ReadAttribInt(item_node, "team_default", 0);
	if (1 == team_default) 
		data()->m_bDefaultForCommunity = true;
	else
		data()->m_bDefaultForCommunity = false;

	R_ASSERT3(!(data()->m_bNoRandom && data()->m_bDefaultForCommunity), 
		"cannot set 'no_random' and 'team_default' flags simultaneously, profile id", *shared_str(item_data.id));
	
	/////////////////////////////////////////////////

#ifdef  XRGAME_EXPORTS

	LPCSTR start_dialog = uiXml.Read("start_dialog", 0, NULL);
	if(start_dialog)
	{
		data()->m_StartDialog	= start_dialog;
//		if(!data()->m_StartDialog || !data()->m_StartDialog.size() )
//			Debug.fatal("start dialog %s doesn't exists in specific character id=%s", start_dialog,  *shared_str(item_data.id));
	}
	else
		data()->m_StartDialog	= NULL;

	int dialogs_num = uiXml.GetNodesNum(uiXml.GetLocalRoot(), "actor_dialog");
	data()->m_ActorDialogs.clear();
	for(int i=0; i<dialogs_num; ++i)
	{
		shared_str dialog_name = uiXml.Read(uiXml.GetLocalRoot(), "actor_dialog", i);
		data()->m_ActorDialogs.push_back(dialog_name);
	}

	data()->m_iIconX		= uiXml.ReadAttribInt("icon", 0, "x");
	data()->m_iIconY		= uiXml.ReadAttribInt("icon", 0, "y");
	data()->m_iMapIconX		= uiXml.ReadAttribInt("map_icon", 0, "x");
	data()->m_iMapIconY		= uiXml.ReadAttribInt("map_icon", 0, "y");

	//игровое имя персонажа
	data()->m_sGameName		= uiXml.Read("name", 0, "");
	data()->m_sBioText		= uiXml.Read("bio", 0, "");

#endif

	data()->m_sVisual		= uiXml.Read("visual", 0, "");
	

#ifdef  XRGAME_EXPORTS
	data()->m_sSupplySpawn	= uiXml.Read("supplies", 0, "");
	
	if(!data()->m_sSupplySpawn.empty())
	{
		xr_string &str = data()->m_sSupplySpawn;
		xr_string::size_type pos = str.find("\\n");

		while (xr_string::npos != pos)
		{
			str.replace(pos, 2, "\n");
			pos = str.find("\\n", pos + 1);
		}
	}

	data()->m_sNpcConfigSect = uiXml.Read("npc_config", 0, "");
	data()->m_sSndConfigSect = uiXml.Read("snd_config", 0, "");

#endif

	data()->m_Classes.clear();
	int classes_num = uiXml.GetNodesNum (uiXml.GetLocalRoot(), "class");
	for(int i=0; i<classes_num; i++)
	{
		LPCSTR char_class = uiXml.Read	("class", 0, "");
		if(char_class)
		{
			char* buf_str = xr_strdup(char_class);
			xr_strlwr(buf_str);
			data()->m_Classes.push_back(buf_str);
			xr_free(buf_str);
		}
	}


#ifdef  XRGAME_EXPORTS

	LPCSTR team = uiXml.Read("community", 0, NULL);
	R_ASSERT3(team != NULL, "'community' field not fulfiled for specific character", *m_OwnId);
	
	char* buf_str = xr_strdup(team);
	xr_strlwr(buf_str);
	data()->m_Community.set(buf_str);
	xr_free(buf_str);
	
	if(data()->m_Community.index() == NO_COMMUNITY_INDEX)
		Debug.fatal("wrong 'community' '%s' in specific character %s ", team, *m_OwnId);

	data()->m_Rank			= uiXml.ReadInt("rank", 0, NO_RANK);
	R_ASSERT3(data()->m_Rank != NO_RANK, "'rank' field not fulfiled for specific character", *m_OwnId);
	data()->m_Reputation	= uiXml.ReadInt("reputation", 0, NO_REPUTATION);
	R_ASSERT3(data()->m_Reputation != NO_REPUTATION, "'reputation' field not fulfiled for specific character", *m_OwnId);

#endif
}


#ifdef  XRGAME_EXPORTS

LPCSTR CSpecificCharacter::Name() const 
{
	return	data()->m_sGameName.c_str();
}

LPCSTR CSpecificCharacter::Bio() const 
{
	return	data()->m_sBioText.c_str();
}

const CHARACTER_COMMUNITY& CSpecificCharacter::Community() const 
{
	return	data()->m_Community;
}

LPCSTR CSpecificCharacter::SupplySpawn	() const 
{
	return data()->m_sSupplySpawn.c_str();
}

LPCSTR CSpecificCharacter::NpcConfigSect () const 
{
	return data()->m_sNpcConfigSect.c_str();
}

LPCSTR CSpecificCharacter::SndConfigSect () const 
{
	return data()->m_sSndConfigSect.c_str();
}

#endif


CHARACTER_RANK_VALUE CSpecificCharacter::Rank() const 
{
	return	data()->m_Rank;
}

CHARACTER_REPUTATION_VALUE CSpecificCharacter::Reputation	() const 
{
	return data()->m_Reputation;
}

LPCSTR CSpecificCharacter::Visual		() const 
{
	return data()->m_sVisual.c_str();
}