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

const CEntityAction *CAI_Stalker::GetCurrentAction()
{
	if (m_tpActionQueue.empty())
		return(0);
	else
		return(m_tpActionQueue.back());
}

void CAI_Stalker::ResetScriptData()
{
	vfSetParameters	(0,0,false,eWeaponStateIdle,m_tPathType,m_tBodyState,eMovementTypeStand,m_tStateType,m_tLookType);
}

void CAI_Stalker::ProcessScripts()
{
	CEntityAction	*l_tpEntityAction = 0;
	while (!m_tpActionQueue.empty()) {
		l_tpEntityAction= m_tpActionQueue.back();
//		R_ASSERT	(l_tpEntityAction);
		if (!l_tpEntityAction->CheckIfActionCompleted())
			break;
		xr_delete	(m_tpActionQueue.back());
		m_tpActionQueue.erase(m_tpActionQueue.begin());
	}
	if (m_tpActionQueue.empty()) {
		Msg			("* Object %s has an empty script queue!",cName());
		ResetScriptData();
		return;
	}
	CMovementAction	&l_tMovementAction = l_tpEntityAction->m_tMovementAction;

	l_tMovementAction.m_bCompleted = false;
	CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
//	R_ASSERT2		(l_tpGameObject);
	AI_Path.DestNode= l_tpGameObject->AI_NodeID;

	vfSetParameters	(0,&Fvector(l_tpGameObject->Position()),false,eWeaponStateIdle,l_tMovementAction.m_tPathType,l_tMovementAction.m_tBodyState,l_tMovementAction.m_tMovementType,eStateTypeDanger,eLookTypeDirection,Fvector().set(0,0,0),0);
}
