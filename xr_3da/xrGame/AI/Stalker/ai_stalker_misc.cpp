////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\actor.h"
#include "..\\..\\bolt.h"

void CAI_Stalker::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EStateType tStateType, ELookType tLookType)
{
	VERIFY(tLookType != eLookTypePoint);
	Fvector tDummy;
	vfSetParameters(tpNodeEvaluator,tpDesiredPosition,bSearchNode,tWeaponState,tPathType,tBodyState,tMovementType,tStateType, tLookType,tDummy);
}

void CAI_Stalker::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, bool bSearchNode, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, EStateType tStateType, ELookType tLookType, Fvector &tPointToLook, u32 dwLookOverDelay)
{
	m_tPathType		= tPathType;
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tStateType	= tStateType;
	bool			bLookChanged = (m_tLookType != tLookType);
	m_tLookType		= tLookType;

	vfChoosePointAndBuildPath(tpNodeEvaluator,tpDesiredPosition, bSearchNode);

	m_fCurSpeed		= 1.f;

	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart)) {
		// if linear speed is too big for a turn 
		// then decrease linear speed and 
		// increase angular speed
//		if ((AI_Path.TravelPath.size() - 2) > AI_Path.TravelStart) {
//			Fvector tPoint1, tPoint2;
//			float	yaw1, pitch1, yaw2, pitch2;
////			tPoint1.sub(AI_Path.TravelPath[AI_Path.TravelStart].P,AI_Path.TravelPath[AI_Path.TravelStart + 1].P);
////			tPoint2.sub(AI_Path.TravelPath[AI_Path.TravelStart + 1].P,AI_Path.TravelPath[AI_Path.TravelStart + 2].P);
////			tPoint1.getHP(yaw1,pitch1);
////			tPoint2.getHP(yaw2,pitch2);
//			if (ps_Size() > 2) {
//				tPoint1.sub(ps_Element(ps_Size() - 2).vPosition,ps_Element(ps_Size() - 3).vPosition);
//				tPoint2.sub(ps_Element(ps_Size() - 1).vPosition,ps_Element(ps_Size() - 2).vPosition);
//				tPoint1.getHP(yaw1,pitch1);
//				tPoint2.getHP(yaw2,pitch2);
//				//Msg("%f -> %f",yaw1/PI*180.f,yaw2/PI*180.f);
//				//GetDirectionAngles(yaw1,pitch1);
//				if (!getAI().bfTooSmallAngle(yaw1,yaw2,1*PI_DIV_6)) {
//					m_tMovementType = eMovementTypeWalk;
//					if (m_tStateType == eStateTypePanic)
//						m_tStateType = eStateTypeDanger;
//				}
//			}
//		}

		switch (m_tBodyState) {
			case eBodyStateCrouch : {
				m_fCurSpeed *= m_fCrouchFactor;
				//Movement.ActivateBox(1);
				break;
			}
			case eBodyStateStand : {
				//Movement.ActivateBox(0);
				break;
			}
			default : NODEFAULT;
		}
		switch (m_tMovementType) {
			case eMovementTypeWalk : {
				switch (m_tStateType) {
					case eStateTypeDanger : {
						m_fCurSpeed *= m_fWalkFactor;
						break;
					}
					case eStateTypeNormal : {
						m_fCurSpeed *= m_fWalkFreeFactor;
						break;
					}
					case eStateTypePanic : {
						m_fCurSpeed *= m_fPanicFactor;
						break;
					}
				}
				r_torso_speed	= PI_MUL_2;
				r_head_speed	= 3*PI_DIV_2;
				break;
			}
			case eMovementTypeRun : {
				switch (m_tStateType) {
					case eStateTypeDanger : {
						m_fCurSpeed *= m_fRunFactor;
						break;
					}
					case eStateTypeNormal : {
						m_fCurSpeed *= m_fRunFreeFactor;
						break;
					}
					case eStateTypePanic : {
						m_fCurSpeed *= m_fPanicFactor;
						break;
					}
				}
//				r_torso_speed	= PI;
//				r_head_speed	= 3*PI_DIV_4;
				r_torso_speed	= PI_MUL_2;
				r_head_speed	= 3*PI_DIV_2;
				break;
			}
			default : m_fCurSpeed = 0.f;
		}
	}
	else {
		m_tMovementType = eMovementTypeStand;
		r_torso_speed	= PI_MUL_2;
		r_head_speed	= 3*PI_DIV_2;
		m_fCurSpeed		= 0.f;
	}
	
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
			tTemp.sub	(tPointToLook,eye_matrix.c);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			r_target.yaw *= -1;
			r_target.pitch *= -1;
			break;
		}
		case eLookTypeFirePoint : {
			Fvector tTemp;
			clCenter(tTemp);
			tTemp.sub	(tPointToLook,tTemp);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			r_target.yaw *= -1;
			r_target.pitch *= -1;
			break;
		}
		case eLookTypeLookOver : {
			if (bLookChanged)
				m_dwLookChangedTime = Level().timeServer();
			Fvector tTemp;
			tTemp.sub	(tPointToLook,eye_matrix.c);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			if (Level().timeServer() - m_dwLookChangedTime > dwLookOverDelay)
				if (Level().timeServer() - m_dwLookChangedTime < 2*dwLookOverDelay)
					r_target.yaw += PI_DIV_6*2;
				else
					r_target.yaw -= PI_DIV_6*2;
			r_target.yaw *= -1;
			r_target.pitch *= -1;
			break;
		}
		case eLookTypeLookFireOver : {
			if (bLookChanged)
				m_dwLookChangedTime = Level().timeServer();
			Fvector tTemp;
			clCenter(tTemp);
			tTemp.sub	(tPointToLook,tTemp);
			tTemp.getHP	(r_target.yaw,r_target.pitch);
			if (Level().timeServer() - m_dwLookChangedTime > dwLookOverDelay)
				if (Level().timeServer() - m_dwLookChangedTime < 2*dwLookOverDelay)
					r_target.yaw += PI_DIV_6*2;
				else
					r_target.yaw -= PI_DIV_6*2;
			r_target.yaw *= -1;
			r_target.pitch *= -1;
			break;
		}
		default : NODEFAULT;
	}
	
	if (AI_Path.fSpeed < EPS_L)
		if (!getAI().bfTooSmallAngle(r_torso_target.yaw,r_target.yaw,2*PI_DIV_6))
			r_torso_target.yaw = r_target.yaw;
	
	vfSetWeaponState(tWeaponState);
}

void CAI_Stalker::vfCheckForItems()
{
	float fDistance = ffGetRange();
	m_tpItemToTake = 0;
	for (u32 i=0, n=m_tpaVisibleObjects.size(); i<n; i++) {
		CInventoryItem	*tpInventoryItem	= dynamic_cast<CInventoryItem*>	(m_tpaVisibleObjects[i]);
		CBolt			*tpBolt				= dynamic_cast<CBolt*>			(m_tpaVisibleObjects[i]);

#pragma todo("Check if rukzak is not full!!")
		if (tpInventoryItem && !tpBolt && (tpInventoryItem->Position().distance_to(vPosition) < fDistance) && getAI().bfInsideNode(tpInventoryItem->AI_Node,tpInventoryItem->Position())) {
			fDistance		= tpInventoryItem->Position().distance_to(vPosition);
			m_tpItemToTake	= tpInventoryItem;
		}
	}
}

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	
	INIT_SQUAD_AND_LEADER;
	
	if (m_dwParticularState != u32(-1))
		return;

	if (this != Leader)	{
		CAI_Stalker *tpLeader			= dynamic_cast<CAI_Stalker*>(Leader);
		if (tpLeader) {
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
			m_tNextGP					= tpLeader->m_tNextGP;
		}
	}
	else
		if ((Level().timeServer() >= m_dwTimeToChange) && (getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex == m_tNextGP)) {
			m_tNextGP					= getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
			vfChooseNextGraphPoint		();
			m_tNextGraphPoint.set		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
		}
}

void CAI_Stalker::vfUpdateParameters(bool &A, bool &B, bool &C, bool &D, bool &E, bool &F, bool &G, bool &H, bool &I, bool &J, bool &K, bool &L, bool &M)
{
	// sound
	int					iIndex;
	SelectSound			(iIndex);
	
	A					= (iIndex > -1) && ((m_tpaDynamicSounds[iIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON);
	B					= (iIndex > -1) && !A;
	bool _A				= (m_iSoundIndex > -1) && ((m_tpaDynamicSounds[m_iSoundIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON);
	bool _B				= (m_iSoundIndex > -1) && !A;
	
	if (A || B)
		m_iSoundIndex = iIndex;
	else
		if (((!A && _A) || (!B && _B)) && (!m_tpaDynamicSounds[m_iSoundIndex].tpEntity || m_tpaDynamicSounds[m_iSoundIndex].tpEntity->g_Alive()) && (Level().timeServer() - m_tpaDynamicSounds[m_iSoundIndex].dwTime < m_dwInertion)) {
			A = _A;
			B = _B;
		}
		else
			m_iSoundIndex = -1;
	J					= A || B;
	
	// victory probability
	C = D = E = F = G	= false;
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	if (VisibleEnemies.size()) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4)) {
			case 4 : 
				C = true;
				break;
			case 3 : 
				D = true;
				break;
			case 2 : 
				E = true;
				break;
			case 1 : 
				F = true;
				break;
			case 0 : 
				G = true;
				break;
		}
	}
	K					= C | D | E | F | G;
	
	// does enemy see me?
	SelectEnemy			(m_tEnemy);
	I					= false;
	for (int i=0, n=VisibleEnemies.size(); i<n; i++) {
		float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
		Fvector			tPosition = VisibleEnemies[i].key->Position();

		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(VisibleEnemies[i].key);
		if (tpCustomMonster) {
			yaw1		= -tpCustomMonster->r_current.yaw;
			pitch1		= -tpCustomMonster->r_current.pitch;
			fYawFov		= angle_normalize_signed(tpCustomMonster->ffGetFov()*PI/180.f);
			fRange		= tpCustomMonster->ffGetRange();
		}
		else {
			CActor		*tpActor = dynamic_cast<CActor *>(VisibleEnemies[i].key);
			if (tpActor) {
				yaw1	= tpActor->Orientation().yaw;
				pitch1	= tpActor->Orientation().pitch;
				fYawFov	= angle_normalize_signed(tpActor->ffGetFov()*PI/180.f);
				fRange	= tpActor->ffGetRange();
			}
			else
				continue;
		}
		
		if (tPosition.distance_to(vPosition) > fRange)
			continue;

		fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(.5f/tPosition.distance_to(vPosition))))/2.f);
		fPitchFov		= angle_normalize_signed(fYawFov*.75f);
		tPosition.sub	(vPosition);
		tPosition.mul	(-1);
		tPosition.getHP	(yaw2,pitch2);
		yaw1			= angle_normalize_signed(yaw1);
		pitch1			= angle_normalize_signed(pitch1);
		yaw2			= angle_normalize_signed(yaw2);
		pitch2			= angle_normalize_signed(pitch2);
		if (I = (getAI().bfTooSmallAngle(yaw1,yaw2,fYawFov) && getAI().bfTooSmallAngle(pitch1,pitch2,fPitchFov)))
			break;
	}
	
	// is enemy expedient?
	H = false;
	getAI().m_tpCurrentMember = this;
	for ( i=0, n=VisibleEnemies.size(); i<n; i++) {
		if (!(getAI().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
			continue;
		if ((E || F || G) && (H = !!getAI().pfExpediency.dwfGetDiscreteValue(2)))
			break;
		else
			if (ifFindHurtIndex(getAI().m_tpCurrentEnemy) != -1)
				H = true;
	}
	//H = true;
	
	// is there any items to pick up?
	L = false;
	if (m_dwParticularState != 7)
		vfCheckForItems();
	
	M = !!m_tpItemToTake;
}