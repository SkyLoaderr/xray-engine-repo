////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_sound_data_visitor.cpp
//	Created 	: 02.02.2005
//  Modified 	: 02.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker sound data visitor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_sound_data_visitor.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_sound_data.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"

CStalkerSoundDataVisitor::~CStalkerSoundDataVisitor	()
{
}

void CStalkerSoundDataVisitor::visit				(CStalkerSoundData *data)
{
	if (object().tfGetRelationType(&data->object()) == ALife::eRelationTypeEnemy)
		return;

	if (!data->object().memory().enemy().selected())
		return;

	if (object().tfGetRelationType(data->object().memory().enemy().selected()) != ALife::eRelationTypeEnemy)
		return;

//	Msg			("%s : Adding fiction hit by sound info from stalker %s",*object().cName(),*data->object().cName());

	object().memory().hit().add							(
		0.f,
		Fvector().set(0.f,0.f,1.f),
		data->object().memory().enemy().selected(),
		0
	);
	
//	object().agent_manager().member().register_in_combat(m_object);
}
