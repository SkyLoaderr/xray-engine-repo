////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

void CAI_Stalker::vfSetParameters(IBaseAI_NodeEvaluator &tNodeEvaluator, Fvector *tpDesiredPosition, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType)
{
	VERIFY(tLookType != eLookTypePoint);
	Fvector tDummy;
	vfSetParameters(tNodeEvaluator,tpDesiredPosition,tWeaponState,tPathType,tBodyState,tMovementType,tLookType,tDummy);
}

void CAI_Stalker::vfSetParameters(IBaseAI_NodeEvaluator &tNodeEvaluator, Fvector *tpDesiredPosition, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType, Fvector &tPointToLook)
{
	m_tPathType		= tPathType;
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tLookType		= tLookType;

	vfChoosePointAndBuildPath(tNodeEvaluator,tpDesiredPosition);

	vfSetWeaponState(tWeaponState);
	
	m_fCurSpeed		= 1.f;

	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart)) {
		switch (m_tBodyState) {
			case eBodyStateCrouch : {
				m_fCurSpeed *= m_fCrouchFactor;
				break;
			}
			case eBodyStateStand : {
				break;
			}
			default : NODEFAULT;
		}
		switch (m_tMovementType) {
			case eMovementTypeWalk : {
				m_fCurSpeed *= m_fWalkFactor;
				break;
			}
			case eMovementTypeRun : {
				m_fCurSpeed *= m_fRunFactor;
				break;
			}
			default : m_fCurSpeed = 0.f;
		}
	}
	else
		m_fCurSpeed = 0.f;
	
	switch (m_tLookType) {
		case eLookTypeDirection : {
			SetDirectionLook();
			break;
		}
		case eLookTypeSearch : {
			SetLessCoverLook(AI_Node,true);
			break;
		}
		case eLookTypeDanger : {
			SetLessCoverLook(AI_Node,PI,true);
			break;
		}
		case eLookTypePoint : {
			Fvector tTemp;
			tTemp.sub	(tPointToLook,vPosition);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			r_target.yaw *= -1;
			r_target.pitch *= -1;
			break;
		}
		case eLookTypeFirePoint : {
			Fvector tTemp;
			tTemp.sub	(tPointToLook,vPosition);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			r_target.yaw *= -1;
			r_target.pitch *= -1;
			break;
		}
		default : NODEFAULT;
	}
	
	m_fCurSpeed = 6.5f;
	
	if (m_fCurSpeed < EPS_L)
		r_torso_target.yaw		= r_target.yaw;
}

void CAI_Stalker::vfCheckForItems()
{
	m_tpItemToTake = 0;
	objVisible&	Known	= Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	for (u32 i=0; i<Known.size(); i++) {
		CInventoryItem *tpInventoryItem = dynamic_cast<CInventoryItem*>(Known[i].key);
#pragma todo("Check if rukzak is not full!!")
		if (tpInventoryItem) {
			m_tpItemToTake = tpInventoryItem;
			break;
		}
	}
}