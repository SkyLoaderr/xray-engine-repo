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
#include "../../torch.h"
#include "../../inventory.h"

void CAI_Stalker::UseObject(const CObject *tpObject)
{
#pragma todo("Dima to Dima : Use object specified by script")
}

ALife::EStalkerRank	CAI_Stalker::GetRank() const
{
#pragma todo("Dima to Dima : Return correct stalker rank")
	return			(ALife::eStalkerRankNovice);
}

CWeapon	*CAI_Stalker::GetCurrentWeapon() const
{
	return			(dynamic_cast<CWeapon*>(inventory().ActiveItem()));
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
}

bool CAI_Stalker::bfAssignMovement(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMovement(tpEntityAction))
		return		(false);
	
	CScriptMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	CScriptWatchAction		&l_tWatchAction		= tpEntityAction->m_tWatchAction;
	CScriptAnimationAction	&l_tAnimationAction	= tpEntityAction->m_tAnimationAction;
	CScriptObjectAction		&l_tObjectAction	= tpEntityAction->m_tObjectAction;

#ifdef OLD_OBJECT_HANDLER
	CObjectHandler::set_dest_state	(l_tObjectAction.m_tGoalType);
#else
	CObjectHandlerGOAP::set_goal	(l_tObjectAction.m_tGoalType);
#endif
	set_path_type					(ePathTypeLevelPath);
	set_detail_path_type			(l_tMovementAction.m_tPathType);
	set_body_state					(l_tMovementAction.m_tBodyState);
	set_movement_type				(l_tMovementAction.m_tMovementType);
	set_mental_state				(l_tAnimationAction.m_tMentalState);
	setup							(l_tWatchAction.m_tWatchType,&l_tWatchAction.m_tWatchVector,0);
	CStalkerMovementManager::update	(Device.dwTimeDelta);
	CSightManager::update			(Device.dwTimeDelta);

	return							(true);
}

bool CAI_Stalker::bfAssignWatch(CEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignWatch(tpEntityAction))
		return		(false);
	
	CScriptWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;

//	float			&yaw = m_head.target.yaw, &pitch = m_head.target.pitch;

	switch (l_tWatchAction.m_tGoalType) {
		case CScriptWatchAction::eGoalTypeObject : {
			if (!xr_strlen(l_tWatchAction.m_bone_to_watch))
				l_tWatchAction.m_tpObjectToWatch->Center(l_tWatchAction.m_tWatchVector);
			else {
				CBoneInstance	&l_tBoneInstance = PKinematics(l_tWatchAction.m_tpObjectToWatch->Visual())->LL_GetBoneInstance(PKinematics(l_tWatchAction.m_tpObjectToWatch->Visual())->LL_BoneID(l_tWatchAction.m_bone_to_watch));
				Fmatrix			l_tMatrix;

				l_tMatrix		= l_tBoneInstance.mTransform;
				l_tMatrix.mulA	(l_tWatchAction.m_tpObjectToWatch->XFORM());

				l_tWatchAction.m_tWatchVector	= l_tMatrix.c;
			}
			setup(l_tWatchAction.m_tWatchType,&l_tWatchAction.m_tWatchVector);
			break;
		}
		case CScriptWatchAction::eGoalTypeDirection : {
			setup(l_tWatchAction.m_tWatchType,&l_tWatchAction.m_tWatchVector);
			break;
		}
		case CScriptWatchAction::eGoalTypeWatchType : {
			break;
		}
		case CScriptWatchAction::eGoalTypeCurrent : {
			l_tWatchAction.m_tWatchType	= SightManager::eSightTypeCurrentDirection;
			l_tWatchAction.m_bCompleted = true;
			return						(false);
		}
		default : NODEFAULT;
	}

	if ((CScriptWatchAction::eGoalTypeWatchType != l_tWatchAction.m_tGoalType) && (angle_difference(m_head.target.yaw,m_head.current.yaw) < EPS_L) && (angle_difference(m_head.target.pitch,m_head.current.pitch) < EPS_L))
		l_tWatchAction.m_bCompleted = true;
	else
		l_tWatchAction.m_bCompleted = false;
	
	return		(!l_tWatchAction.m_bCompleted);
}

bool CAI_Stalker::bfAssignObject(CEntityAction *tpEntityAction)
{
	CScriptObjectAction	&l_tObjectAction	= tpEntityAction->m_tObjectAction;
	CInventoryItem	*l_tpInventoryItem	= dynamic_cast<CInventoryItem*>(l_tObjectAction.m_tpObject);

	if (!inherited::bfAssignObject(tpEntityAction) || !l_tObjectAction.m_tpObject || !l_tpInventoryItem) {
		if (!inventory().ActiveItem()) {
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionIdle);
#endif
		}
		else {
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionIdle,inventory().ActiveItem());
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionIdle,inventory().ActiveItem());
#endif
		}

#ifdef OLD_OBJECT_HANDLER
		return	((l_tObjectAction.m_bCompleted = (CObjectHandler::goal_reached())) == false);
#else
		return	((l_tObjectAction.m_bCompleted = (CObjectHandlerGOAP::goal_reached())) == false);
#endif
	}

	if (!l_tpInventoryItem->H_Parent())
		return			(true);

	CWeapon				*l_tpWeapon				= dynamic_cast<CWeapon*>(inventory().ActiveItem());
	CWeaponMagazined	*l_tpWeaponMagazined	= dynamic_cast<CWeaponMagazined*>(inventory().ActiveItem());

	if (l_tpWeaponMagazined)
		l_tpWeaponMagazined->SetQueueSize		(l_tObjectAction.m_dwQueueSize);

	switch (l_tObjectAction.m_tGoalType) {
		case eObjectActionIdle : {
			if (!l_tpWeapon)
				return	((l_tObjectAction.m_bCompleted = true) == false);
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionIdle,l_tpInventoryItem);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionIdle,l_tpInventoryItem);
#endif
//			inventory().Action	(kWPN_FIRE,	CMD_STOP);
#ifdef OLD_OBJECT_HANDLER
			return	((l_tObjectAction.m_bCompleted = (CObjectHandler::goal_reached())) == false);
#else
			return	((l_tObjectAction.m_bCompleted = (CObjectHandlerGOAP::goal_reached())) == false);
#endif
			break;
		}
		case eObjectActionFire1 : {
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionFire1,l_tpInventoryItem);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionFire1,l_tpInventoryItem);
#endif
//			if (!l_tpWeapon)
//				return	((l_tObjectAction.m_bCompleted = true) == false);
			if (inventory().ActiveItem() && l_tpWeapon) {
				if (l_tpWeapon->GetAmmoElapsed()) {
//					if (l_tpWeapon->GetAmmoMagSize() > 1)
//						l_tpWeaponMagazined->SetQueueSize(l_tObjectAction.m_dwQueueSize);
//					else
//						l_tpWeaponMagazined->SetQueueSize(1);
//					inventory().Action(kWPN_FIRE,	CMD_START);
				}
				else {
//					inventory().Action(kWPN_FIRE,	CMD_STOP);
					if (l_tpWeapon->GetAmmoCurrent()) {
//						CObjectHandlerGOAP::set_goal	(eObjectActionFire1,l_tObjectAction.m_tpObject);
//						inventory().Action(kWPN_RELOAD, CMD_START);
					}
					else
						l_tObjectAction.m_bCompleted = true;
				}
			}
			break;
		}
		case eObjectActionFire2 : {
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionFire2,l_tpInventoryItem);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionFire2,l_tpInventoryItem);
#endif
//			if (!l_tpWeapon)
//				return	((l_tObjectAction.m_bCompleted = true) == false);
			if (inventory().ActiveItem()) {
				if (l_tpWeapon->GetAmmoElapsed()) {
//					if (l_tpWeapon->GetAmmoMagSize() > 1)
//						l_tpWeaponMagazined->SetQueueSize(l_tObjectAction.m_dwQueueSize);
//					else
//						l_tpWeaponMagazined->SetQueueSize(1);
//					inventory().Action(kWPN_FIRE,	CMD_START);
				}
				else {
//					inventory().Action(kWPN_FIRE,	CMD_STOP);
					if (l_tpWeapon->GetAmmoCurrent()) {
//						CObjectHandlerGOAP::set_goal	(eObjectActionFire1,l_tObjectAction.m_tpObject);
//						inventory().Action(kWPN_RELOAD, CMD_START);
					}
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
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionReload1,l_tpInventoryItem);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionReload1,l_tpInventoryItem);
#endif
			if (inventory().ActiveItem()->ID() == l_tObjectAction.m_tpObject->ID()) {
//				inventory().Action(kWPN_FIRE,	CMD_STOP);
				if (CWeapon::eReload != l_tpWeapon->STATE) {
//					inventory().Action(kWPN_RELOAD,	CMD_START);
				}
				else
					l_tObjectAction.m_bCompleted = true;
			}
			else
				ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"cannot reload active item because it is not selected!");

//			if (inventory().ActiveItem()) {
//				inventory().Action(kWPN_FIRE,	CMD_STOP);
//				if (CWeapon::eReload != l_tpWeapon->STATE)
//					inventory().Action(kWPN_RELOAD,	CMD_START);
//				else
//					l_tObjectAction.m_bCompleted = true;
//			}
//			else
//				ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"cannot reload active item because it is not selected!");
			break;
		}
		case eObjectActionActivate : {
			CTorch			*torch = dynamic_cast<CTorch*>(l_tObjectAction.m_tpObject);
			if (torch) {
				torch->Switch(true);
				break;
			}
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionIdle,l_tpInventoryItem);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionIdle,l_tpInventoryItem);
#endif
//				inventory().Slot(l_tpInventoryItem);
//				inventory().Activate(l_tpInventoryItem->GetSlot());
//			if (inventory().ActiveItem() && (inventory().ActiveItem()->ID() == l_tpInventoryItem->ID()))
//				if (l_tpWeapon && (CWeapon::eIdle == l_tpWeapon->STATE))
//				l_tObjectAction.m_bCompleted = true;
#ifdef OLD_OBJECT_HANDLER
			return	((l_tObjectAction.m_bCompleted = (CObjectHandler::goal_reached())) == false);
#else
			return	((l_tObjectAction.m_bCompleted = (CObjectHandlerGOAP::goal_reached())) == false);
#endif

			break;
		}
		case eObjectActionDeactivate : {
			CTorch			*torch = dynamic_cast<CTorch*>(l_tObjectAction.m_tpObject);
			if (torch) {
				torch->Switch(false);
				break;
			}
//				inventory().Activate(u32(-1));
#ifdef OLD_OBJECT_HANDLER
			CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
			CObjectHandlerGOAP::set_goal	(eObjectActionIdle);
#endif
#ifdef OLD_OBJECT_HANDLER
			return	((l_tObjectAction.m_bCompleted = (CObjectHandler::goal_reached())) == false);
#else
			return	((l_tObjectAction.m_bCompleted = (CObjectHandlerGOAP::goal_reached())) == false);
#endif
			break;
		}
		case eObjectActionUse : {
			l_tObjectAction.m_bCompleted = true;
			break;
		}
		case eObjectActionTake : {
			if (inventory().GetItemFromInventory(*l_tObjectAction.m_tpObject->cName())) {
				ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"item is already in the inventory!");
				return	((l_tObjectAction.m_bCompleted = true) == false);
			}
			feel_touch_new(l_tObjectAction.m_tpObject);
			l_tObjectAction.m_bCompleted = true;
			break;
		}
		case eObjectActionDrop : {
			if (!inventory().GetItemFromInventory(*l_tObjectAction.m_tpObject->cName())) {
				ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"item is not in the inventory!");
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

	if (xr_strlen(tpEntityAction->m_tAnimationAction.m_caAnimationToPlay)) {
#ifdef _DEBUG
//		Msg				("%6d Assigning animation : %s",Level().timeServer(),*tpEntityAction->m_tAnimationAction.m_caAnimationToPlay);
#endif
		m_tpCurrentTorsoAnimation = m_tpCurrentLegsAnimation = 0;
	}

	return				(true);
}
