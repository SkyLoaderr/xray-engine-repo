//////////////////////////////////////////////////////////////////////////
// character_info.cpp			игровая информация для персонажей в игре
// 
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_info.h"
#include "PhraseDialog.h"
#include "ui/xrXMLParser.h"

#include "alife_registry_container_composition.h"

//////////////////////////////////////////////////////////////////////////

//возможное отклонение от значения репутации
//заданого в профиле и для конкретного персонажа
#define REPUTATION_DELTA	10
#define RANK_DELTA			10

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
	m_iCharacterIndex = NO_SPECIFIC_CHARACTER;
	m_Community		= NO_COMMUNITY;
	m_Rank			= NO_RANK;
	m_Reputation	= NO_REPUTATION;
}

SCharacterProfile::~SCharacterProfile()
{
}



//////////////////////////////////////////////////////////////////////////

CCharacterInfo::CCharacterInfo()
{
	m_iProfileIndex = NO_PROFILE;
	m_iSpecificCharacterIndex = NO_SPECIFIC_CHARACTER;
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
	InitSpecificCharacter(data()->m_iCharacterIndex);
}

void CCharacterInfo::SetSpecificCharacter ()
{
	R_ASSERT(m_iSpecificCharacterIndex != NO_SPECIFIC_CHARACTER);

	m_SpecificCharacter.Load(m_iSpecificCharacterIndex);
	if(Rank() == NO_RANK)
		SetRank(m_SpecificCharacter.Rank());
	if(Reputation() == NO_REPUTATION)
		SetReputation(m_SpecificCharacter.Reputation());

	if(ai().get_alife())
	{
		//запомнить, то что мы использовали индекс
		int a = 1;
		ai().alife().registry(specific_characters).add(m_iSpecificCharacterIndex, a, true);
	}
}


void CCharacterInfo::InitSpecificCharacter (SPECIFIC_CHARACTER_INDEX new_index)
{
	if (NO_SPECIFIC_CHARACTER != m_iSpecificCharacterIndex) 
	{
		//удалить использованный индекс из реестра
		if(ai().get_alife())
			ai().alife().registry(specific_characters).remove(m_iSpecificCharacterIndex, true);
	}

	m_iSpecificCharacterIndex = new_index;


	if(NO_SPECIFIC_CHARACTER != m_iSpecificCharacterIndex)
	{
		SetSpecificCharacter();
	}
	//проверяем все информации о персонаже, запоминаем подходящие,
	//а потом делаем случайный выбор
	else
	{
		SPECIFIC_CHARACTER_INDEX first_found_index = NO_SPECIFIC_CHARACTER;
		for(SPECIFIC_CHARACTER_INDEX i=0; i<=CSpecificCharacter::GetMaxIndex(); i++)
		{
			CSpecificCharacter spec_char;
			spec_char.Load(i);

			if(data()->m_Community == NO_COMMUNITY || !xr_strcmp(spec_char.Community(), data()->m_Community))
			{
				if(data()->m_Rank == NO_RANK || _abs(spec_char.Rank() - data()->m_Rank)<RANK_DELTA)
				{
					if(data()->m_Reputation == NO_REPUTATION || _abs(spec_char.Reputation() - data()->m_Reputation)<REPUTATION_DELTA)
					{
						if(NO_SPECIFIC_CHARACTER == first_found_index)
						{
							first_found_index = i;
						}
						
						int* count = NULL;
						if(ai().get_alife())
							count = ai().alife().registry(specific_characters).object(i, true);
						//если индекс еще не был использован
						if(NULL == count)
							m_CheckedCharacters.push_back(i);
					}
				}
			}
		}
		R_ASSERT3(NO_SPECIFIC_CHARACTER != first_found_index, 
			"No specific character found for profile id", *IndexToId(m_iProfileIndex));
	
		
		if(m_CheckedCharacters.empty())
			m_iSpecificCharacterIndex = first_found_index;
		else
			m_iSpecificCharacterIndex = m_CheckedCharacters[Random.randI(m_CheckedCharacters.size())];

		SetSpecificCharacter();
	}
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
	data()->m_Community		= uiXml.Read("team", 0, NO_COMMUNITY);
	data()->m_Rank			= uiXml.ReadInt("rank", 0, NO_RANK);
	data()->m_Reputation	= uiXml.ReadInt("reputation", 0, NO_REPUTATION);

	 LPCSTR spec_char = uiXml.Read("specific_character", 0, NULL);
	 if(!spec_char)
         data()->m_iCharacterIndex = NO_SPECIFIC_CHARACTER;
	 else
		 data()->m_iCharacterIndex = CSpecificCharacter::IdToIndex(spec_char);
}


LPCSTR CCharacterInfo::Name() const
{
	return	m_SpecificCharacter.Name();
}
CHARACTER_RANK CCharacterInfo::Rank() const
{
	return	m_CurrentRank;
}
LPCSTR CCharacterInfo::Community() const
{
	return	m_SpecificCharacter.Community();
}

CHARACTER_REPUTATION CCharacterInfo::Reputation() const
{
	return m_CurrentReputation;
}

void CCharacterInfo::SetRank (CHARACTER_RANK rank)
{
	m_CurrentRank = rank;
}
void CCharacterInfo::SetReputation (CHARACTER_REPUTATION reputation)
{
	m_CurrentReputation = reputation;
}

void CCharacterInfo::SetCommunity	(CHARACTER_COMMUNITY community)
{
	m_CurrentCommunity = community;
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

int	 CCharacterInfo::TradeIconX() const
{
	return m_SpecificCharacter.TradeIconX();
}
int	 CCharacterInfo::TradeIconY() const
{
	return m_SpecificCharacter.TradeIconY();
}
int	 CCharacterInfo::MapIconX()	 const
{
	return m_SpecificCharacter.MapIconX();
}
int	 CCharacterInfo::MapIconY()	 const
{
	return m_SpecificCharacter.MapIconY();
}

PHRASE_DIALOG_INDEX	CCharacterInfo::StartDialog	()	const
{
	return m_SpecificCharacter.data()->m_iStartDialog;
}
PHRASE_DIALOG_INDEX	CCharacterInfo::ActorDialog	()	const
{
	return m_SpecificCharacter.data()->m_iActorDialog;
}

void CCharacterInfo::load	(IReader& stream)
{
	m_SpecificCharacter.data()->m_iStartDialog = stream.r_s16();
}
void CCharacterInfo::save	(NET_Packet& stream)
{
	stream.w_s16((s16)StartDialog());
}