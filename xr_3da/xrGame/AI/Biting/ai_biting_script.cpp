#include "stdafx.h"
#include "ai_biting.h"
#include "../../ai_script_actions.h"
#include "../../phmovementcontrol.h"

//////////////////////////////////////////////////////////////////////////
bool CAI_Biting::bfAssignMovement (CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMovement(tpEntityAction))
		return		(false);

	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	MotionMan.m_tAction = EAction(l_tMovementAction.m_tActState);

	// pre-update path parameters
	CMonsterMovement::Frame_Init();
	CDetailPathManager::set_use_dest_orientation(false);

	SetupVelocityMasks			(l_tMovementAction.m_tActTypeEx == eAT_ForceMovementType);

	Frame_Update				();

	UpdateActionWithPath		();
	MotionMan.ProcessAction		();

	UpdateVelocityWithPath		();

	CMonsterMovement::Frame_Finalize				();
	
//	// Show patrol path
//	HDebug->L_Clear();
//	const CLevel::SPath *P = CPatrolPathManager::get_path();
//	for (u32 i=0; i < P->tpaWayPoints.size(); i++) {
//		HDebug->L_AddPoint(P->tpaWayPoints[i].tWayPoint, 0.35f, D3DCOLOR_XRGB(0,255,255));
//	}
//
//	HDebug->SetActive();

	return			(true);		
}

//////////////////////////////////////////////////////////////////////////

bool CAI_Biting::bfAssignObject(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

	CObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
	if (!l_tObjectAction.m_tpObject)
		return	(false == (l_tObjectAction.m_bCompleted = true));

	CEntityAlive	*l_tpEntity		= dynamic_cast<CEntityAlive*>(l_tObjectAction.m_tpObject);
	if (!l_tpEntity) return	(false == (l_tObjectAction.m_bCompleted = true));

	switch (l_tObjectAction.m_tGoalType) {
		case eObjectActionTake: 
			m_PhysicMovementControl->PHCaptureObject(l_tpEntity);
			break;
		case eObjectActionDrop: 
			m_PhysicMovementControl->PHReleaseObject();
			break;
	}
	l_tObjectAction.m_bCompleted = true;
	return	(true);
}


