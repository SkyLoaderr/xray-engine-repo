////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "action_manager_stalker.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionBase
//////////////////////////////////////////////////////////////////////////

CStalkerActionBase::CStalkerActionBase		(CAI_Stalker *object, CConditionStorage *storage, LPCSTR action_name) :
	inherited			(object,action_name),
	m_storage			(storage)
{
}

IC	void CStalkerActionBase::set_property	(_condition_type condition_id, _value_type value)
{
	VERIFY						(m_storage);
	CConditionStorage::iterator	I = m_storage->find(condition_id);
	VERIFY						(m_storage->end() != I);
	(*I).second					= value;
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

CStalkerActionDead::CStalkerActionDead	(CAI_Stalker *object, CConditionStorage *storage, LPCSTR action_name) :
	inherited				(object,storage,action_name)
{
}

void CStalkerActionDead::execute		()
{
	inherited::execute		();
	m_object->CMovementManager::enable_movement(false);
	set_property			(CActionManagerStalker::eWorldPropertyDead,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFreeNoALife
//////////////////////////////////////////////////////////////////////////

CStalkerActionFreeNoALife::CStalkerActionFreeNoALife	(CAI_Stalker *object, CConditionStorage *storage, LPCSTR action_name) :
	inherited				(object,storage,action_name)
{
}

void CStalkerActionFreeNoALife::execute		()
{
	inherited::execute		();
	m_object->play			(eStalkerSoundHumming,60000,10000);
	CStalkerMovementManager	*cast = dynamic_cast<CStalkerMovementManager*>(m_object);
	VERIFY					(cast);
	cast->update			(
		0,
		0,
		0,
		0,
		CMovementManager::ePathTypeGamePath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeWalk,
		eMentalStateFree
		);
	m_object->CSightManager::update				(eLookTypeSearch);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}
