#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"
#include "../controlled_entity.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingControlled class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingControlled::CBitingControlled(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBitingControlled::Init()
{
	inherited::Init ();
}

void CBitingControlled::Run()
{
	CControlledEntityBase *entity = dynamic_cast<CControlledEntityBase *>(pMonster);
	VERIFY(entity);

	float dist = pMonster->Position().distance_to(entity->get_data().m_object->Position());

	switch (entity->get_data().m_task) {
		case eTaskFollow:
			
			if (dist < 5.0f) {
				pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			} else {
				pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
				pMonster->MoveToTarget(entity->get_data().m_object);
			}

			break;
		case eTaskAttack:

			break;
		default:

			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	} 
}


