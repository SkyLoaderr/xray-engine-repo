////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_script.cpp
//	Created 	: 29.09.2003
//  Modified 	: 29.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker script functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../ai_script_actions.h"
#include "../../weapon.h"
#include "../../WeaponMagazined.h"

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
	CMemoryManager::Init		();
	if (P)
		vfSetParameters			(0,0,false,MonsterSpace::eObjectActionIdle,ePathTypeNoPath,eDetailPathTypeSmooth,m_tBodyState,eMovementTypeStand,m_tMentalState,eLookTypeDirection);
}

bool CAI_Stalker::bfAssignMovement(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMovement(tpEntityAction))
		return		(false);
	
	CMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	CWatchAction	&l_tWatchAction		= tpEntityAction->m_tWatchAction;
	CAnimationAction&l_tAnimationAction	= tpEntityAction->m_tAnimationAction;
	CObjectAction	&l_tObjectAction	= tpEntityAction->m_tObjectAction;

	vfSetParameters	(0,
		&l_tMovementAction.m_tDestinationPosition,
		false,
		l_tObjectAction.m_tGoalType,
		CMovementManager::path_type(),
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

	float			yaw = m_head.target.yaw, pitch = m_head.target.pitch;

	switch (l_tWatchAction.m_tGoalType) {
		case CWatchAction::eGoalTypeObject : {
			if (!xr_strlen(l_tWatchAction.m_bone_to_watch))
				l_tWatchAction.m_tpObjectToWatch->Center(l_tWatchAction.m_tWatchVector);
			else {
				CBoneInstance	&l_tBoneInstance = PKinematics(l_tWatchAction.m_tpObjectToWatch->Visual())->LL_GetBoneInstance(PKinematics(l_tWatchAction.m_tpObjectToWatch->Visual())->LL_BoneID(*l_tWatchAction.m_bone_to_watch));
				Fmatrix			l_tMatrix;

				l_tMatrix		= l_tBoneInstance.mTransform;
				l_tMatrix.mulA	(l_tWatchAction.m_tpObjectToWatch->XFORM());

				l_tWatchAction.m_tWatchVector	= l_tMatrix.c;
			}
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
		case CWatchAction::eGoalTypeCurrent : {
			l_tWatchAction.m_tWatchType	= eLookTypeDirection;
			l_tWatchAction.m_tWatchVector.setHP(-m_body.current.yaw,m_body.current.pitch);
			m_body.target				= m_body.current;
			m_head.target				= m_head.current;
			l_tWatchAction.m_bCompleted = true;
			return						(false);
		}
		default : NODEFAULT;
	}

	if ((CWatchAction::eGoalTypeWatchType != l_tWatchAction.m_tGoalType) && (angle_difference(yaw,m_head.current.yaw) < EPS_L) && (angle_difference(pitch,m_head.current.pitch) < EPS_L))
		l_tWatchAction.m_bCompleted = true;
	else
		l_tWatchAction.m_bCompleted = false;
	
	return		(!l_tWatchAction.m_bCompleted);
}

bool CAI_Stalker::bfAssignObject(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

	CObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
	if (!l_tObjectAction.m_tpObject)
		return	((l_tObjectAction.m_bCompleted = true) == false);

	CInventoryItem		*l_tpInventoryItem		= dynamic_cast<CInventoryItem*>(l_tObjectAction.m_tpObject);
	if (!l_tpInventoryItem)
		return	((l_tObjectAction.m_bCompleted = true) == false);

	CWeapon				*l_tpWeapon				= dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	CWeaponMagazined	*l_tpWeaponMagazined	= dynamic_cast<CWeaponMagazined*>(m_inventory.ActiveItem());

	switch (l_tObjectAction.m_tGoalType) {
		case eObjectActionIdle : {
			if (!l_tpWeapon)
				return	((l_tObjectAction.m_bCompleted = true) == false);
			m_inventory.Action	(kWPN_FIRE,	CMD_STOP);
			l_tObjectAction.m_bCompleted = true;
			break;
		}
		case eObjectActionFire1 : {
			if (!l_tpWeapon)
				return	((l_tObjectAction.m_bCompleted = true) == false);
			if (m_inventory.ActiveItem()) {
				if (l_tpWeapon->GetAmmoElapsed()) {
					if (l_tpWeapon->GetAmmoMagSize() > 1)
						l_tpWeaponMagazined->SetQueueSize(l_tObjectAction.m_dwQueueSize);
					else
						l_tpWeaponMagazined->SetQueueSize(1);
					m_inventory.Action(kWPN_FIRE,	CMD_START);
				}
				else {
					m_inventory.Action(kWPN_FIRE,	CMD_STOP);
					if (l_tpWeapon->GetAmmoCurrent())
						m_inventory.Action(kWPN_RELOAD, CMD_START);
					else
						l_tObjectAction.m_bCompleted = true;
				}
			}
			break;
		}
		case eObjectActionFire2 : {
			if (!l_tpWeapon)
				return	((l_tObjectAction.m_bCompleted = true) == false);
			if (m_inventory.ActiveItem()) {
				if (l_tpWeapon->GetAmmoElapsed()) {
					if (l_tpWeapon->GetAmmoMagSize() > 1)
						l_tpWeaponMagazined->SetQueueSize(l_tObjectAction.m_dwQueueSize);
					else
						l_tpWeaponMagazined->SetQueueSize(1);
					m_inventory.Action(kWPN_FUNC,	CMD_START);
				}
				else {
					m_inventory.Action(kWPN_FUNC,	CMD_STOP);
					if (l_tpWeapon->GetAmmoCurrent())
						m_inventory.Action(kWPN_RELOAD, CMD_START);
					else
						l_tObjectAction.m_bCompleted = true;
				}
			}
			break;
		}
		case eObjectActionReload2 :
		case eObjectActionReload1 : {
			if (!l_tpWeapon)
				return	((l_tObjectAction.m_bCompleted = true) == false);
			if (m_inventory.ActiveItem()) {
				m_inventory.Action(kWPN_FIRE,	CMD_STOP);
				if (CWeapon::eReload != l_tpWeapon->STATE)
					m_inventory.Action(kWPN_RELOAD,	CMD_START);
				else
					l_tObjectAction.m_bCompleted = true;
			}
			else
				LuaOut(Lua::eLuaMessageTypeError,"cannot reload active item because it is not selected!");
			break;
		}
		case eObjectActionActivate : {
			CInventoryItem	*l_tpInventoryItem = dynamic_cast<CInventoryItem*>(l_tObjectAction.m_tpObject);
			if (l_tpInventoryItem) {
				m_inventory.Slot(l_tpInventoryItem);
				m_inventory.Activate(l_tpInventoryItem->GetSlot());
				if (l_tpWeapon && (CWeapon::eShowing != l_tpWeapon->STATE))
					l_tObjectAction.m_bCompleted = true;
			}
			else
				LuaOut(Lua::eLuaMessageTypeError,"cannot activate non-inventory object!");
			break;
		}
		case eObjectActionDeactivate : {
			CInventoryItem	*l_tpInventoryItem = dynamic_cast<CInventoryItem*>(l_tObjectAction.m_tpObject);
			if (l_tpInventoryItem) {
				m_inventory.Activate(u32(-1));
				l_tObjectAction.m_bCompleted = true;
			}
			else
				LuaOut(Lua::eLuaMessageTypeError,"cannot activate non-inventory object!");
			break;
		}
		case eObjectActionUse : {
			l_tObjectAction.m_bCompleted = true;
			break;
		}
		case eObjectActionTake : {
			if (m_inventory.GetItemFromInventory(l_tObjectAction.m_tpObject->cName())) {
				LuaOut(Lua::eLuaMessageTypeError,"item is already in the inventory!");
				return	((l_tObjectAction.m_bCompleted = true) == false);
			}
			feel_touch_new(l_tObjectAction.m_tpObject);
			l_tObjectAction.m_bCompleted = true;
			break;
		}
		case eObjectActionDrop : {
			if (!m_inventory.GetItemFromInventory(l_tObjectAction.m_tpObject->cName())) {
				LuaOut(Lua::eLuaMessageTypeError,"item is not in the inventory!");
				return	((l_tObjectAction.m_bCompleted = true) == false);
			}
			DropItemSendMessage(l_tObjectAction.m_tpObject);
			break;
		}
		default : NODEFAULT;
	}

	return	(true);
}

bool CAI_Stalker::bfAssignAnimation(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignAnimation(tpEntityAction))
		return			(false);

	if (xr_strlen(tpEntityAction->m_tAnimationAction.m_caAnimationToPlay))
		m_tpCurrentTorsoAnimation = m_tpCurrentLegsAnimation = 0;

	return				(true);
}
