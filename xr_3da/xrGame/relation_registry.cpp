//////////////////////////////////////////////////////////////////////////
// relation_registry->registry().cpp:	реестр для хранения данных об отношении персонажа к 
//							другим персонажам
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"
#include "character_community.h"
#include "alife_registry_wrappers.h"

#define GAME_RELATIONS_SECT "game_relations"


//////////////////////////////////////////////////////////////////////////

SRelation::SRelation()
{
	m_iGoodwill = NEUTRAL_GOODWILL;
}

SRelation::~SRelation()
{
}

//////////////////////////////////////////////////////////////////////////

void RELATION_DATA::clear	()
{
	personal.clear();
	communities.clear();
}

void RELATION_DATA::load (IReader& stream)
{
	load_data(personal, stream);
	load_data(communities, stream);
}
void RELATION_DATA::save (IWriter& stream)
{
	save_data(personal, stream);
	save_data(communities, stream);
}

//////////////////////////////////////////////////////////////////////////


CHARACTER_GOODWILL SRelation::Goodwill() const
{
	return m_iGoodwill;
}
void SRelation::SetGoodwill(CHARACTER_GOODWILL new_goodwill)
{
	m_iGoodwill = new_goodwill;
}

//////////////////////////////////////////////////////////////////////////

RELATION_REGISTRY::RELATION_REGISTRY  ()
{
	relation_registry	= xr_new<CRelationRegistryWrapper>();
	m_iOurCommunity		= NO_COMMUNITY_INDEX;
}

RELATION_REGISTRY::~RELATION_REGISTRY ()
{
	xr_delete			(relation_registry);
}

void RELATION_REGISTRY::Init			(u16 id, CHARACTER_COMMUNITY_INDEX comm_index)
{
	relation_registry->registry().init(id);
	m_iOurCommunity = comm_index;
}


CHARACTER_GOODWILL	 RELATION_REGISTRY::GetAttitude	(u16 person_id, CHARACTER_COMMUNITY_INDEX comm_index) const 
{
	CHARACTER_GOODWILL presonal_goodwill		= GetGoodwill(person_id); VERIFY(presonal_goodwill != NO_GOODWILL);
	CHARACTER_GOODWILL community_goodwill		= GetCommunityGoodwill(comm_index); VERIFY(community_goodwill != NO_GOODWILL);
	CHARACTER_GOODWILL community_to_community	= CHARACTER_COMMUNITY::relation(m_iOurCommunity, comm_index);
	CHARACTER_GOODWILL attitude = presonal_goodwill + community_goodwill + community_to_community;

	return attitude;
}

void				 RELATION_REGISTRY::SetRelationType		(u16 person_id, CHARACTER_COMMUNITY_INDEX comm_index, ALife::ERelationType new_relation)
{
	static int goodwill_enemy	= pSettings->r_s16(GAME_RELATIONS_SECT, "goodwill_enemy");
	static int goodwill_neutral = pSettings->r_s16(GAME_RELATIONS_SECT, "goodwill_neutal");
	static int goodwill_friend	= pSettings->r_s16(GAME_RELATIONS_SECT, "goodwill_friend");

	switch(new_relation)
	{
	case ALife::eRelationTypeEnemy:
        SetGoodwill(person_id, goodwill_enemy);
		break;
	case ALife::eRelationTypeNeutral:
		SetGoodwill(person_id, goodwill_neutral);
		break;
	case ALife::eRelationTypeFriend:
		SetGoodwill(person_id, goodwill_friend);
		break;
	default:
		NODEFAULT;
	}
}

ALife::ERelationType RELATION_REGISTRY::GetRelationType		(u16 person_id, CHARACTER_COMMUNITY_INDEX comm_index) const 
{
	static int attitude_neutral = pSettings->r_s16(GAME_RELATIONS_SECT, "attitude_neutal_threshold");
	static int attitude_friend = pSettings->r_s16(GAME_RELATIONS_SECT, "attitude_friend_threshold");

	CHARACTER_GOODWILL attitude = GetAttitude(person_id, comm_index);

	if(attitude == NO_GOODWILL)
		return ALife::eRelationTypeNeutral;

	if(attitude<attitude_neutral)
		return ALife::eRelationTypeEnemy;
	else if(attitude<attitude_friend)
		return ALife::eRelationTypeNeutral;
	else
		return ALife::eRelationTypeFriend;
}

//////////////////////////////////////////////////////////////////////////

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetGoodwill			(u16 person_id) const 
{
	const RELATION_DATA* relation_data = relation_registry->registry().objects_ptr();

	if(relation_data)
	{
		PERSONAL_RELATION_MAP::const_iterator it = relation_data->personal.find(person_id);
		if(relation_data->personal.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetGoodwill 	(u16 person_id, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry->registry().objects();
	relation_data.personal[person_id].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ChangeGoodwill 	(u16 person_id, CHARACTER_GOODWILL delta_goodwill)
{
	RELATION_DATA& relation_data = relation_registry->registry().objects();
	relation_data.personal[person_id].SetGoodwill(relation_data.personal[person_id].Goodwill() + delta_goodwill);
}

//////////////////////////////////////////////////////////////////////////

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetCommunityGoodwill (CHARACTER_COMMUNITY_INDEX comm_index) const 
{
	const RELATION_DATA* relation_data = relation_registry->registry().objects_ptr();

	if(relation_data)
	{
		COMMUNITY_RELATION_MAP::const_iterator it = relation_data->communities.find(comm_index);
		if(relation_data->communities.end() != it)
		{
			const SRelation& relation = (*it).second;
			return relation.Goodwill();
		}
	}
	//если отношение еще не задано, то возвращаем нейтральное
	return NEUTRAL_GOODWILL;
}

void RELATION_REGISTRY::SetCommunityGoodwill 	(CHARACTER_COMMUNITY_INDEX comm_index, CHARACTER_GOODWILL goodwill)
{
	RELATION_DATA& relation_data = relation_registry->registry().objects();
	relation_data.communities[comm_index].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ChangeCommunityGoodwill 	(CHARACTER_COMMUNITY_INDEX comm_index, CHARACTER_GOODWILL delta_goodwill)
{
	RELATION_DATA& relation_data = relation_registry->registry().objects();
	relation_data.communities[comm_index].SetGoodwill(relation_data.communities[comm_index].Goodwill() + delta_goodwill);
}



//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::ClearRelations	()
{
	const RELATION_DATA* relation_data = relation_registry->registry().objects_ptr();
	if(relation_data)
	{
		relation_registry->registry().objects().clear();
	}
}