////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_script.cpp
//	Created 	: 29.09.2003
//  Modified 	: 29.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker script functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\ai_script_actions.h"
#include "..\\..\\weapon.h"

void CAI_Stalker::UseObject(const CObject *tpObject)
{
#pragma todo("Dima to Dima : Use object specified by script")
}

ALife::EStalkerRank	CAI_Stalker::GetRank() const
{
#pragma todo("Dima to Dima : Return correct stalker rank")
	return			(eStalkerRankNovice);
}

CWeapon	*CAI_Stalker::GetCurrentWeapon() const
{
	return			(dynamic_cast<CWeapon*>(m_inventory.ActiveItem()));
}

u32 CAI_Stalker::GetWeaponAmmo() const
{
	if (!GetCurrentWeapon())
		return		(0);
	return			(GetCurrentWeapon()->GetAmmoCurrent());	
}

CInventoryItem *CAI_Stalker::GetCurrentEquipment() const
{
#pragma todo("Dima to Dima : Return correct equipment")
	return			(0);
}

CInventoryItem *CAI_Stalker::GetMedikit() const
{
#pragma todo("Dima to Dima : Return correct medikit")
	return			(0);
}

CInventoryItem *CAI_Stalker::GetFood() const
{
#pragma todo("Dima to Dima : Return correct food")
	return			(0);
}

void CAI_Stalker::ResetScriptData(void *P)
{
	inherited::ResetScriptData	(P);
	
	m_tpCurrentGlobalAnimation = 0;

	if (P)
		vfSetParameters(0,0,false,eObjectActionIdle,m_tPathType,m_tBodyState,eMovementTypeStand,m_tMentalState,m_tLookType);
}

bool CAI_Stalker::bfAssignMovement(CEntityAction *tpEntityAction)
{
	if (!CScriptMonster::bfAssignMovement(tpEntityAction))
		return		(false);
	
	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	CWatchAction	&l_tWatchAction		= tpEntityAction->m_tWatchAction;
	CAnimationAction&l_tAnimationAction	= tpEntityAction->m_tAnimationAction;
	CObjectAction	&l_tObjectAction	= tpEntityAction->m_tObjectAction;

	vfSetParameters	(0,
		&l_tMovementAction.m_tDestinationPosition,
		false,
		l_tObjectAction.m_tGoalType,
		l_tMovementAction.m_tPathType,
		l_tMovementAction.m_tBodyState,
		l_tMovementAction.m_tMovementType,
		l_tAnimationAction.m_tMentalState,
		l_tWatchAction.m_tWatchType,
		l_tWatchAction.m_tWatchVector,
		0
	);
	return			(true);
}

bool CAI_Stalker::bfAssignWatch(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignWatch(tpEntityAction))
		return		(false);
	
	CWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;

	float			yaw = 0, pitch = 0;

	switch (l_tWatchAction.m_tGoalType) {
		case CWatchAction::eGoalTypeObject : {
			l_tWatchAction.m_tpObjectToWatch->Center(l_tWatchAction.m_tWatchVector);
			if (eLookTypeFirePoint == l_tWatchAction.m_tWatchType)
				SetFirePointLookAngles(l_tWatchAction.m_tWatchVector,yaw,pitch);
			else
				SetPointLookAngles(l_tWatchAction.m_tWatchVector,yaw,pitch);
			break;
											 }
		case CWatchAction::eGoalTypeDirection : {
			if (eLookTypeDirection == l_tWatchAction.m_tWatchType) {
				l_tWatchAction.m_tWatchVector.getHP(yaw,pitch);
				yaw				*= -1;
				pitch			*= -1;
			}
			else
				if (eLookTypeFirePoint == l_tWatchAction.m_tWatchType)
					SetFirePointLookAngles(l_tWatchAction.m_tWatchVector,yaw,pitch);
				else
					SetPointLookAngles(l_tWatchAction.m_tWatchVector,yaw,pitch);
			break;
												}
		case CWatchAction::eGoalTypeWatchType : {
			break;
												}
		default : NODEFAULT;
	}

	if ((CWatchAction::eGoalTypeWatchType != l_tWatchAction.m_tGoalType) && (_abs(yaw - r_current.yaw) < EPS_L) && (_abs(pitch - r_current.pitch) < EPS_L))
		l_tWatchAction.m_bCompleted = true;
	else
		l_tWatchAction.m_bCompleted = false;
	
	return		(!l_tWatchAction.m_bCompleted);
}
