//////////////////////////////////////////////////////////////////////////
// relation_registry.cpp:	реестр для хранения данных об отношении персонажа к 
//							другим персонажам
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"

#define GAME_RELATIONS_SECT "game_relations"



//////////////////////////////////////////////////////////////////////////

SRelation::SRelation()
{
}
SRelation::~SRelation()
{
}

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
}
RELATION_REGISTRY::~RELATION_REGISTRY ()
{
}

void RELATION_REGISTRY::Init			(u16 id)
{
	relation_registry.init(id);
}


CHARACTER_ATTITUDE	 RELATION_REGISTRY::GetAttitude			(u16 person_id) const 
{
	return GetGoodwill(person_id);
}

void				 RELATION_REGISTRY::SetRelationType		(u16 person_id, ALife::ERelationType new_relation)
{
	static goodwill_enemy	= pSettings->r_s16(GAME_RELATIONS_SECT, "goodwill_enemy");
	static goodwill_neutral = pSettings->r_s16(GAME_RELATIONS_SECT, "goodwill_neutal");
	static goodwill_friend	= pSettings->r_s16(GAME_RELATIONS_SECT, "goodwill_friend");

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

ALife::ERelationType RELATION_REGISTRY::GetRelationType		(u16 person_id) const 
{
	static attitude_neutral = pSettings->r_s16(GAME_RELATIONS_SECT, "attitude_neutal_threshold");
	static attitude_friend = pSettings->r_s16(GAME_RELATIONS_SECT, "attitude_friend_threshold");

	CHARACTER_ATTITUDE attitude = GetAttitude(person_id);

	if(attitude == NO_ATTITUDE)
		return ALife::eRelationTypeNeutral;

	if(attitude<attitude_neutral)
		return ALife::eRelationTypeEnemy;
	else if(attitude<attitude_friend)
		return ALife::eRelationTypeNeutral;
	else
		return ALife::eRelationTypeFriend;
}

CHARACTER_GOODWILL	 RELATION_REGISTRY::GetGoodwill			(u16 person_id) const 
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

void RELATION_REGISTRY::SetGoodwill 	(u16 person_id, CHARACTER_GOODWILL goodwill)
{
	RELATION_MAP& relation_map = relation_registry.objects();
	relation_map[person_id].SetGoodwill(goodwill);
}

void RELATION_REGISTRY::ChangeGoodwill 	(u16 person_id, CHARACTER_GOODWILL delta_goodwill)
{
	RELATION_MAP& relation_map = relation_registry.objects();
	relation_map[person_id].SetGoodwill(relation_map[person_id].Goodwill() + delta_goodwill);
}


void RELATION_REGISTRY::ClearRelations	()
{
	const RELATION_MAP* relation_map = relation_registry.objects_ptr();
	if(relation_map)
	{
		relation_registry.objects().clear();
	}
}