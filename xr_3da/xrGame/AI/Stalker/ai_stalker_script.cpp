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

void CAI_Stalker::AddAction(const CEntityAction *tpEntityAction)
{
	m_tpActionQueue.push_back(xr_new<CEntityAction>(*tpEntityAction));
}

CEntityAction *CAI_Stalker::GetCurrentAction()
{
	if (m_tpActionQueue.empty())
		return(0);
	else
		return(m_tpActionQueue.back());
}

#pragma todo("Dima to Dima : Recover code from fatal error : C1055")

void CAI_Stalker::ResetScriptData(bool bResetPath)
{
	m_tpCurrentGlobalAnimation = 0;
	if (bResetPath)
		vfSetParameters(0,0,false,eObjectActionIdle,m_tPathType,m_tBodyState,eMovementTypeStand,m_tMentalState,m_tLookType);
}

void CAI_Stalker::ProcessScripts()
{
	CEntityAction	*l_tpEntityAction = 0;
	while (!m_tpActionQueue.empty()) {
		l_tpEntityAction= m_tpActionQueue.back();
//		R_ASSERT	(l_tpEntityAction);
		if (!l_tpEntityAction->CheckIfActionCompleted())
			break;
				
		ResetScriptData(false);
		xr_delete	(m_tpActionQueue.back());
		m_tpActionQueue.erase(m_tpActionQueue.begin());
	}
	
	if (m_tpActionQueue.empty()) {
		Msg			("* Object %s has an empty script queue!",cName());
		ResetScriptData();
		return;
	}

	vfAssignWatch	(l_tpEntityAction);
	vfAssignAnimation(l_tpEntityAction);
	vfAssignSound	(l_tpEntityAction);
	vfAssignObject	(l_tpEntityAction);
	vfAssignMovement(l_tpEntityAction);
}

void CAI_Stalker::vfAssignWatch(CEntityAction *tpEntityAction)
{
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
}

void CAI_Stalker::vfAssignAnimation(CEntityAction *tpEntityAction)
{
}

void CAI_Stalker::vfAssignSound(CEntityAction *tpEntityAction)
{
	CSoundAction	&l_tSoundAction = tpEntityAction->m_tSoundAction;
	if (l_tSoundAction.m_bCompleted)
		return;
	if (l_tSoundAction.m_tpSound) {
		switch (l_tSoundAction.m_tGoalType) {
			case CSoundAction::eGoalTypeSoundAttached : {
				l_tSoundAction.m_tSoundPosition.set(Position());
				break;
			}
			case CSoundAction::eGoalTypeSoundPosition : {
				break;
			}
			default : NODEFAULT;
		}
		if (!l_tSoundAction.m_tpSound->feedback)
			if (!l_tSoundAction.m_bStartedToPlay)
				::Sound->play_at_pos(*l_tSoundAction.m_tpSound,this,l_tSoundAction.m_tSoundPosition);
			else {
				l_tSoundAction.m_bCompleted = true;
				::Sound->destroy(*l_tSoundAction.m_tpSound);
				l_tSoundAction.m_tpSound = 0;
			}
		else
			l_tSoundAction.m_tpSound->feedback->set_position(l_tSoundAction.m_tSoundPosition);
	}
}

void CAI_Stalker::vfAssignObject(CEntityAction *tpEntityAction)
{
}

void CAI_Stalker::vfAssignMovement(CEntityAction *tpEntityAction)
{
	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	CWatchAction	&l_tWatchAction		= tpEntityAction->m_tWatchAction;
	CAnimationAction&l_tAnimationAction	= tpEntityAction->m_tAnimationAction;
	CObjectAction	&l_tObjectAction	= tpEntityAction->m_tObjectAction;
	
	if (l_tMovementAction.m_bCompleted)
		return;

//	R_ASSERT		(l_tpGameObject);
	
	switch (l_tMovementAction.m_tGoalType) {
		case CMovementAction::eGoalTypeObject : {
			CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
			l_tMovementAction.m_tDestinationPosition = l_tMovementAction.m_tpObjectToGo->Position();
			if (Position().distance_to(l_tMovementAction.m_tDestinationPosition) < EPS_L)
				l_tMovementAction.m_bCompleted = true;
			else
				if (!l_tpGameObject)
					if (getAI().bfInsideNode(AI_Node,l_tMovementAction.m_tDestinationPosition))
						l_tMovementAction.m_tNodeID	= AI_NodeID;
					else
						if ((int(AI_Path.DestNode) > 0) && getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),l_tMovementAction.m_tDestinationPosition))
							l_tMovementAction.m_tNodeID	= AI_Path.DestNode;
						else
							l_tMovementAction.m_tNodeID	= getAI().q_Node(AI_NodeID,l_tMovementAction.m_tDestinationPosition,false);
				else
					l_tMovementAction.m_tNodeID	= l_tpGameObject->AI_NodeID;
			break;
		}
		case CMovementAction::eGoalTypePatrolPath : {
#pragma todo("Dima to Dima : Implement patrol paths")
			break;
		}
		case CMovementAction::eGoalTypePosition : {
			if (getAI().bfInsideNode(AI_Node,l_tMovementAction.m_tDestinationPosition))
				l_tMovementAction.m_tNodeID	= AI_NodeID;
			else
				if ((int(AI_Path.DestNode) > 0) && getAI().bfInsideNode(getAI().Node(AI_Path.DestNode),l_tMovementAction.m_tDestinationPosition))
					l_tMovementAction.m_tNodeID	= AI_Path.DestNode;
				else
					l_tMovementAction.m_tNodeID	= getAI().q_Node(AI_NodeID,l_tMovementAction.m_tDestinationPosition,false);
			break;
		}
		default : NODEFAULT;
	}

	if (int(l_tMovementAction.m_tNodeID) > 0)
		AI_Path.DestNode =  l_tMovementAction.m_tNodeID;
	else {
		Msg		("! Cannot find a node for the specified position [%f][%f][%f]",VPUSH(l_tMovementAction.m_tDestinationPosition));
		l_tMovementAction.m_tDestinationPosition = Position();
		l_tMovementAction.m_tNodeID = AI_NodeID;
	}

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
}