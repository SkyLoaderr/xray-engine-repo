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

void CAI_Stalker::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType)
{
	VERIFY(tLookType != eLookTypePoint);
	Fvector tDummy;
	vfSetParameters(tpNodeEvaluator,tpDesiredPosition,tWeaponState,tPathType,tBodyState,tMovementType,tLookType,tDummy);
}

void CAI_Stalker::vfSetParameters(IBaseAI_NodeEvaluator *tpNodeEvaluator, Fvector *tpDesiredPosition, EWeaponState tWeaponState, EPathType tPathType, EBodyState tBodyState, EMovementType tMovementType, ELookType tLookType, Fvector &tPointToLook)
{
	m_tPathType		= tPathType;
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tLookType		= tLookType;

	vfChoosePointAndBuildPath(tpNodeEvaluator,tpDesiredPosition);

	m_fCurSpeed		= 1.f;

	if (AI_Path.TravelPath.size() && ((AI_Path.TravelPath.size() - 1) > AI_Path.TravelStart)) {
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
	
	if (m_fCurSpeed < EPS_L)
		if (!getAI().bfTooSmallAngle(r_torso_target.yaw,r_target.yaw,2*PI_DIV_6))
			r_torso_target.yaw = r_target.yaw;
	
	vfSetWeaponState(tWeaponState);
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

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	
	INIT_SQUAD_AND_LEADER;
	
	if (this != Leader)	{
		CAI_Stalker *tpLeader			= dynamic_cast<CAI_Stalker*>(Leader);
		if (tpLeader)
			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
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
	SelectSound			(m_iSoundIndex);
	A					= (m_iSoundIndex > -1) && ((m_tpaDynamicSounds[m_iSoundIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON) && ((m_tpaDynamicSounds[m_iSoundIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_MONSTER);
	B					= (m_iSoundIndex > -1) && !A;
	J					= A || B;
	
	C = D = E = F = G	= false;
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	if (m_tSavedEnemy && (Level().timeServer() - m_dwLostEnemyTime  < 20000))
		VisibleEnemies.insert(m_tSavedEnemy);
	if (VisibleEnemies.size()) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4)) {
			case 0 : 
				C = true;
				break;
			case 1 : 
				D = true;
				break;
			case 2 : 
				E = true;
				break;
			case 3 : 
				F = true;
				break;
			case 4 : 
				G = true;
				break;
		}
	}
	K					= C | D | E | F | G;
	I					= false;
	for (int i=0, n=VisibleEnemies.size(); i<n; i++) {
		float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
		Fvector			tPosition = VisibleEnemies[i].key->Position();

		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(VisibleEnemies[i].key);
		if (tpCustomMonster) {
			yaw1		= tpCustomMonster->r_current.yaw;
			pitch1		= tpCustomMonster->r_current.pitch;
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
	
	H = false;
	getAI().m_tpCurrentMember = this;
	for ( i=0, n=VisibleEnemies.size(); i<n; i++) {
		if (!(getAI().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
			continue;
		if (H	= !!getAI().pfExpediency.dwfGetDiscreteValue(2))
			break;
	}
	
	L = false;

	vfCheckForItems();
	
	M = !!m_tpItemToTake;
}