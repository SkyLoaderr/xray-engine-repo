////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_misc.cpp
//	Created 	: 27.02.2003
//  Modified 	: 27.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../actor.h"
#include "../../bolt.h"
#include "../../ai_script_actions.h"
#include "../../ef_storage.h"

void CAI_Stalker::vfSetParameters(
	PathManagers::CAbstractVertexEvaluator *tpNodeEvaluator, 
	Fvector *tpDesiredPosition, 
	bool bSearchNode, 
	EObjectAction tWeaponState, 
	EPathType tGlobalPathType, 
	EDetailPathType tPathType, 
	EBodyState tBodyState, 
	EMovementType tMovementType, 
	EMentalState tMentalState, 
	ELookType tLookType
)
{
	R_ASSERT		(eLookTypePoint != tLookType);
	vfSetParameters	(tpNodeEvaluator,tpDesiredPosition,bSearchNode,tWeaponState,tGlobalPathType,tPathType,tBodyState,tMovementType,tMentalState, tLookType, Fvector().set(0,0,0));
}

void CAI_Stalker::vfSetParameters(
	PathManagers::CAbstractVertexEvaluator	*tpNodeEvaluator, 
	Fvector									*tpDesiredPosition, 
	bool									bSearchNode, 
	EObjectAction							tWeaponState, 
	EPathType								tGlobalPathType,
	EDetailPathType							tPathType, 
	EBodyState								tBodyState, 
	EMovementType							tMovementType, 
	EMentalState							tMentalState, 
	ELookType								tLookType, 
	const Fvector							&tPointToLook, 
	u32										dwLookOverDelay
)
{
	CMovementManager::set_path_type(tGlobalPathType);
	CDetailPathManager::set_path_type(tPathType);
	m_tBodyState	= tBodyState;
	m_tMovementType = tMovementType;
	m_tMentalState	= tMentalState;
	bool			bLookChanged = (m_tLookType != tLookType);
	m_tLookType		= tLookType;

	update_path		();

	m_fCurSpeed		= 1.f;

	if (!CDetailPathManager::path().empty() && ((CDetailPathManager::path().size() - 1) > CDetailPathManager::curr_travel_point_index())) {
		if (GetScriptControl() && GetCurrentAction() && (_abs(GetCurrentAction()->m_tMovementAction.m_fSpeed) > EPS_L))
			m_fCurSpeed = GetCurrentAction()->m_tMovementAction.m_fSpeed;
		else {
		// if linear speed is too big for a turn 
		// then decrease linear speed and 
		// increase angular speed
//			if ((CDetailPathManager::path().size() - 2) > CDetailPathManager::m_current_travel_point) {
//				Fvector tPoint1, tPoint2;
//				float	yaw1, pitch1, yaw2, pitch2;
////				tPoint1.sub(CDetailPathManager::path()[CDetailPathManager::m_current_travel_point].P,CDetailPathManager::path()[CDetailPathManager::m_current_travel_point + 1].P);
////				tPoint2.sub(CDetailPathManager::path()[CDetailPathManager::m_current_travel_point + 1].P,CDetailPathManager::path()[CDetailPathManager::m_current_travel_point + 2].P);
////				tPoint1.getHP(yaw1,pitch1);
////				tPoint2.getHP(yaw2,pitch2);
//				if (ps_Size() > 2) {
//					tPoint1.sub(ps_Element(ps_Size() - 2).Position(),ps_Element(ps_Size() - 3).Position());
//					tPoint2.sub(ps_Element(ps_Size() - 1).Position(),ps_Element(ps_Size() - 2).Position());
//					tPoint1.getHP(yaw1,pitch1);
//					tPoint2.getHP(yaw2,pitch2);
//					//Msg("%f -> %f",yaw1/PI*180.f,yaw2/PI*180.f);
//					//GetDirectionAngles(yaw1,pitch1);
//					if (!angle_difference(yaw1,yaw2,1*PI_DIV_6)) {
//						m_tMovementType = eMovementTypeWalk;
//						if (m_tMentalState == eMentalStatePanic)
//							m_tMentalState = eMentalStateDanger;
//					}
//				}
//			}

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
					switch (m_tMentalState) {
						case eMentalStateDanger : {
							m_fCurSpeed *= IsLimping() ? m_fDamagedWalkFactor : m_fWalkFactor;
							break;
						}
						case eMentalStateFree : {
							m_fCurSpeed *= IsLimping() ? m_fDamagedWalkFreeFactor : m_fWalkFreeFactor;
							break;
						}
						case eMentalStatePanic : {
							Debug.fatal("");
							break;
						}
					}
					m_body.speed	= PI_MUL_2;
					m_head.speed	= 3*PI_DIV_2;
					break;
				}
				case eMovementTypeRun : {
					switch (m_tMentalState) {
						case eMentalStateDanger : {
							m_fCurSpeed *= IsLimping() ? m_fDamagedRunFactor : m_fRunFactor;
							break;
						}
						case eMentalStateFree : {
							m_fCurSpeed *= IsLimping() ? m_fDamagedRunFreeFactor : m_fRunFreeFactor;
							break;
						}
						case eMentalStatePanic : {
							m_fCurSpeed *= IsLimping() ? m_fDamagedPanicFactor : m_fPanicFactor;
							break;
						}
					}
					m_body.speed	= PI_MUL_2;
					m_head.speed	= 3*PI_DIV_2;
					break;
				}
				default : m_fCurSpeed = 0.f;
			}
		}
	}
	else {
		m_tMovementType = eMovementTypeStand;
		m_body.speed	= PI_MUL_2;
		m_head.speed	= 3*PI_DIV_2;
		m_fCurSpeed		= 0.f;
	}
	
	switch (m_tLookType) {
		case eLookTypePathDirection : {
			SetDirectionLook();
			break;
		}
		case eLookTypeDirection : {
			tPointToLook.getHP	(m_head.target.yaw,m_head.target.pitch);
			m_head.target.yaw		*= -1;
			m_head.target.pitch		*= -1;
			break;
		}
		case eLookTypeSearch : {
			SetLessCoverLook(level_vertex(),true);
			break;
		}
		case eLookTypeDanger : {
			SetLessCoverLook(level_vertex(),PI,true);
			break;
		}
		case eLookTypePoint : {
			SetPointLookAngles(tPointToLook,m_head.target.yaw,m_head.target.pitch);
			break;
		}
		case eLookTypeFirePoint : {
			SetFirePointLookAngles(tPointToLook,m_head.target.yaw,m_head.target.pitch);
			break;
		}
		case eLookTypeLookOver : {
			if (bLookChanged)
				m_dwLookChangedTime = Level().timeServer();
			Fvector tTemp;
			tTemp.sub	(tPointToLook,eye_matrix.c);
			tTemp.getHP	(m_head.target.yaw,m_head.target.pitch);
			VERIFY					(_valid(m_head.target.yaw));
			VERIFY					(_valid(m_head.target.pitch));
			if (Level().timeServer() - m_dwLookChangedTime > dwLookOverDelay)
				if (Level().timeServer() - m_dwLookChangedTime < 2*dwLookOverDelay)
					m_head.target.yaw += PI_DIV_6*2;
				else {
					if (Level().timeServer() - m_dwLookChangedTime >= 3*dwLookOverDelay)
						m_dwLookChangedTime = Level().timeServer();
					m_head.target.yaw -= PI_DIV_6*2;
				}
			m_head.target.yaw *= -1;
			m_head.target.pitch *= -1;
			break;
		}
		case eLookTypeLookFireOver : {
			if (bLookChanged)
				m_dwLookChangedTime = Level().timeServer();
			Fvector tTemp;
			Center(tTemp);
			tTemp.sub	(tPointToLook,tTemp);
			tTemp.getHP	(m_head.target.yaw,m_head.target.pitch);
			VERIFY					(_valid(m_head.target.yaw));
			VERIFY					(_valid(m_head.target.pitch));
			if (Level().timeServer() - m_dwLookChangedTime > dwLookOverDelay)
				if (Level().timeServer() - m_dwLookChangedTime < 2*dwLookOverDelay)
					m_head.target.yaw += PI_DIV_6*2;
				else {
					if (Level().timeServer() - m_dwLookChangedTime >= 3*dwLookOverDelay)
						m_dwLookChangedTime = Level().timeServer();
					m_head.target.yaw -= PI_DIV_6*2;
				}
			m_head.target.yaw *= -1;
			m_head.target.pitch *= -1;
			break;
		}
		default : NODEFAULT;
	}
	
	if (speed() < EPS_L)
		if (angle_difference(m_body.target.yaw,m_head.target.yaw) > 2*PI_DIV_6)
			m_body.target.yaw = m_head.target.yaw;
	
	set_desirable_speed	(m_fCurSpeed);

	if (!GetScriptControl())
		vfSetWeaponState(tWeaponState);
}

void CAI_Stalker::vfSelectItemToTake(CInventoryItem *&tpItemToTake)
{
	tpItemToTake = 0;
	if (m_tpItemsToTake.empty())
		return;
	tpItemToTake = m_tpItemsToTake[0];
	float fDistSqr = Position().distance_to_sqr(tpItemToTake->Position());
	for (int i=1, n = (int)m_tpItemsToTake.size(); i<n; ++i)
		if (!m_tpItemsToTake[i]->H_Parent() && (Position().distance_to_sqr(m_tpItemsToTake[i]->Position()) < fDistSqr) && m_tpItemsToTake[i]->level_vertex() && (ai().level_graph().inside(m_tpItemsToTake[i]->level_vertex(),m_tpItemsToTake[i]->Position()))) {
			fDistSqr = Position().distance_to_sqr(m_tpItemsToTake[i]->Position());
			tpItemToTake = m_tpItemsToTake[i];
		}
}

void CAI_Stalker::vfCheckForItems()
{
	for (u32 i=0, n=m_tpaVisibleObjects.size(); i<n; ++i) {
		CInventoryItem	*tpInventoryItem	= dynamic_cast<CInventoryItem*>	(m_tpaVisibleObjects[i]);
		CBolt			*tpBolt				= dynamic_cast<CBolt*>			(m_tpaVisibleObjects[i]);
#pragma todo("Dima to Dima : Check if rukzak is not full!!")
		if (tpInventoryItem && !tpBolt)
			m_tpItemsToTake.push_back(tpInventoryItem);
	}
	for (int i=0; i<(int)m_tpItemsToTake.size(); ++i)
		if (m_tpItemsToTake[i]->H_Parent())
			m_tpItemsToTake.erase(m_tpItemsToTake.begin() + i--);
}

void CAI_Stalker::vfUpdateSearchPosition()
{
	if (!g_Alive())
		return;
	
	INIT_SQUAD_AND_LEADER;
	
	if (u32(-1) != m_dwParticularState)
		return;

//	if (this != Leader)	{
//		CAI_Stalker *tpLeader			= dynamic_cast<CAI_Stalker*>(Leader);
//		if (tpLeader) {
//			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
//			m_tNextGP					= tpLeader->m_tNextGP;
//		}
//	}
//	else
//		if ((Level().timeServer() >= m_dwTimeToChange) && (ai().cross_table().vertex(level_vertex_id()) == m_tNextGP)) {
//			m_tNextGP					= ai().cross_table().vertex(level_vertex_id());
//			vfChooseNextGraphPoint		();
//			m_tNextGraphPoint.set		(ai().m_tpaGraph[m_tNextGP].tLocalPoint);
//			m_dwTimeToChange			= Level().timeServer() + 10000;
//		}
}

void CAI_Stalker::vfUpdateParameters(bool &A, bool &B, bool &C, bool &D, bool &E, bool &F, bool &G, bool &H, bool &I, bool &J, bool &K, bool &L, bool &M)
{
	// ref_sound
	int					iIndex;
	SelectSound			(iIndex);
	
	A					= (iIndex > -1) && ((m_tpaDynamicSounds[iIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON);
	B					= (iIndex > -1) && !A;
	bool _A				= (m_iSoundIndex > -1) && ((m_tpaDynamicSounds[m_iSoundIndex].eSoundType & SOUND_TYPE_WEAPON) == SOUND_TYPE_WEAPON);
	bool _B				= (m_iSoundIndex > -1) && !A;
	
	if (A || B)
		m_iSoundIndex = iIndex;
	else
		if (((!A && _A) || (!B && _B)) && (!m_tpaDynamicSounds[m_iSoundIndex].tpEntity || m_tpaDynamicSounds[m_iSoundIndex].tpEntity->g_Alive()) && (Level().timeServer() - m_tpaDynamicSounds[m_iSoundIndex].dwTime < m_dwInertion) && (m_tpaDynamicSounds[m_iSoundIndex].dwNodeID < ai().level_graph().header().vertex_count())) {
//		if ((!m_tpaDynamicSounds[m_iSoundIndex].tpEntity || m_tpaDynamicSounds[m_iSoundIndex].tpEntity->g_Alive()) && (Level().timeServer() - m_tpaDynamicSounds[m_iSoundIndex].dwTime < m_dwInertion)) {
#ifndef SILENCE
			Msg("Inertion sound from %s",m_tpaDynamicSounds[m_iSoundIndex].tpEntity ? m_tpaDynamicSounds[m_iSoundIndex].tpEntity->cName() : "world");
#endif
			A = _A;
			B = _B;
		}
		else
			m_iSoundIndex = -1;
	J					= A || B;
	
	// victory probability
	C = D = E = F = G	= false;
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	if (bfIsAnyAlive(VisibleEnemies)) {
 		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
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
	for (int i=0, n=VisibleEnemies.size(); i<n; ++i) {
		float			yaw1, pitch1, yaw2, pitch2, fYawFov, fPitchFov, fRange;
		Fvector			tPosition = VisibleEnemies[i].key->Position();

		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(VisibleEnemies[i].key);
		if (tpCustomMonster) {
			yaw1		= -tpCustomMonster->m_head.current.yaw;
			pitch1		= -tpCustomMonster->m_head.current.pitch;
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
		
		if (tPosition.distance_to(Position()) > fRange)
			continue;

		fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/tPosition.distance_to(Position()))))/2.f);
		fPitchFov		= angle_normalize_signed(fYawFov*1.f);
		tPosition.sub	(Position());
		tPosition.mul	(-1);
		tPosition.getHP	(yaw2,pitch2);
		yaw1			= angle_normalize_signed(yaw1);
		pitch1			= angle_normalize_signed(pitch1);
		yaw2			= angle_normalize_signed(yaw2);
		pitch2			= angle_normalize_signed(pitch2);
		if (0 != (I = ((angle_difference(yaw1,yaw2) <= fYawFov) && (angle_difference(pitch1,pitch2) <= fPitchFov))))
			break;
	}
	
	// is enemy expedient?
	H = false;
	ai().ef_storage().m_tpCurrentMember = this;
	for ( i=0, n=VisibleEnemies.size(); i<n; ++i) {
		if (0 == (ai().ef_storage().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
			continue;
		if ((E || F || G) && (true == (H = !!ai().ef_storage().m_pfExpediency->dwfGetDiscreteValue())))
			break;
		else
			if (-1 != ifFindHurtIndex(ai().ef_storage().m_tpCurrentEnemy))
				H = true;
	}
	H = true;
	
	// is there any items to pick up?
	L = false;
	if (7 != m_dwParticularState)
		vfCheckForItems();
	
	M = !!m_tpItemsToTake.size();
}

void CAI_Stalker::vfValidatePosition(Fvector &/**tPosition/**/, u32 /**dwNodeID/**/)
{
//	if (ai().level_graph().valid_vertex_id(dwNodeID <= 0) || (ai().level_graph().check_position_in_direction(dwNodeID,ai().level_graph().vertex_position(dwNodeID),tPosition) == u32(-1)))
//		m_tSavedEnemyPosition = ai().level_graph().vertex_position(dwNodeID);
}

bool CAI_Stalker::bfIfHuman(CEntity *tpEntity)
{
	if (!tpEntity)
		tpEntity = m_tEnemy.m_enemy;
	if (!tpEntity)
		return(false);
	switch (tpEntity->SUB_CLS_ID) {
		case CLSID_OBJECT_ACTOR :
		case CLSID_AI_SCIENTIST :
		case CLSID_AI_SOLDIER :
		case CLSID_AI_STALKER_MILITARY :
		case CLSID_AI_STALKER : 
			return(true);
		default : return(false);
	}
}
