////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_fsm.cpp
//	Created 	: 25.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "../ai_monsters_misc.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"
#include "ai_rat_space.h"
#include "../../../skeletonanimated.h"
#include "../../detail_path_manager.h"
#include "../../memory_manager.h"
#include "../../enemy_manager.h"
#include "../../item_manager.h"
#include "../../memory_space.h"
#include "../../ai_object_location.h"
#include "../../movement_manager.h"

using namespace RatSpace;

#undef	WRITE_TO_LOG
#define WRITE_TO_LOG(s) m_bStopThinking = true;
/*	if (!visible_objects().size())\
Msg("* No objects in frustum",visible_objects().size());\
else {\
Msg("* Objects in frustum (%d) :",visible_objects().size());\
for (int i=0; i<(int)visible_objects().size(); ++i)\
Msg("*   %s",*visible_objects()[i]->cName());\
}\
/**
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s\n* Time delta : %7.3f\n* Global time : %7.3f",*cName(),s,m_fTimeUpdateDelta,float(Level().timeServer())/1000.f);\
	m_bStopThinking = true;\
}
/**/

#ifndef DEBUG
	#undef	WRITE_TO_LOG
	#define WRITE_TO_LOG(s) m_bStopThinking = true;
#endif

void CAI_Rat::Think()
{
	m_thinking			= true;
	m_current_update	= Level().timeServer();
	vfUpdateMorale();
	vfUpdateSpawnPosition();
	m_bStopThinking = false;
	do {
		m_ePreviousState = m_eCurrentState;
		switch(m_eCurrentState) {
			case aiRatDie : {
				Death();
				break;
			}
			case aiRatFreeHuntingActive : {
				FreeHuntingActive();
				break;
			}
			case aiRatFreeHuntingPassive : {
				FreeHuntingPassive();
				break;
			}
			case aiRatAttackFire : {
				AttackFire();
				break;
			}
			case aiRatAttackRun : {
				AttackRun();
				break;
			}
			case aiRatUnderFire : {
				UnderFire();
				break;
			}
			case aiRatRetreat : {
				Retreat();
				break;
			}
			case aiRatPursuit : {
				Pursuit();
				break;
			}
			case aiRatFreeRecoil : {
				FreeRecoil();
				break;
			}
			case aiRatReturnHome : {
				ReturnHome();
				break;   
			}
			case aiRatEatCorpse : {
				EatCorpse();
				break;
			}
		}
//		test_movement();
		m_bStateChanged = m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
	m_thinking			= false;
}

void CAI_Rat::Death()
{
	//WRITE_TO_LOG	("Dying...");
	vfSetFire		(false);

	m_bStopThinking = true;

	vfSetFire		(false);
	
	SelectAnimation	(XFORM().k,movement().detail_path_manager().direction(),0);

	if (m_fFood <= 0) {
		if (m_previous_query_time <= GetLevelDeathTime())
			m_previous_query_time = Level().timeServer();
		setVisible(false);
		if (Level().timeServer() - m_previous_query_time > 10000) {
//			setEnabled(false);
//			NET_Packet			P;
//			u_EventGen			(P,GE_DESTROY,ID());
//			u_EventSend			(P);
		}
	}

}

void CAI_Rat::FreeHuntingActive()
{
	WRITE_TO_LOG("Free hunting active");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((memory().enemy().selected() && ((memory().enemy().selected()->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (Position().distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))),aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue,aiRatUnderFire);
	
	if	(
			(m_tLastSound.dwTime >= m_dwLastUpdateTime) && 
			(
				!m_tLastSound.tpEntity || 
				(
					(!memory().item().selected() || (memory().item().selected()->ID() != m_tLastSound.tpEntity->ID())) && 
					(m_tLastSound.tpEntity->g_Team() != g_Team())
				)
			) && 
			!memory().enemy().selected()
		)
	{
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}
    
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(memory().item().selected(),aiRatEatCorpse);
	
	m_tSpawnPosition.set	(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	
	if (bfCheckIfGoalChanged()) {
		if (m_bStateChanged || (Position().distance_to(m_tSpawnPosition) > m_fStableDistance) || (::Random.randF(0,1) > m_fChangeActiveStateProbability))
			if (Position().distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius)
				m_fSpeed = m_fSafeSpeed = m_fMaxSpeed;
			else
				vfChooseNewSpeed();
		else {
			if (can_stand_here())
				vfRemoveActiveMember();
		}
	}

	if (m_bStateChanged || (fis_zero(m_fSpeed) && (angle_difference(movement().m_body.target.yaw,movement().m_body.current.yaw) < PI_DIV_6)))
		vfChooseNewSpeed();

	vfUpdateTime(m_fTimeUpdateDelta);

	vfComputeNewPosition(false);
	
	CSoundPlayer::play	(eRatSoundVoice,45*1000,15*1000);
}

void CAI_Rat::FreeHuntingPassive()
{
	WRITE_TO_LOG("Free hunting passive");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false);

	if (memory().enemy().selected()) {
		m_fGoalChangeTime = 0;
		vfAddActiveMember(true);
		m_bStopThinking = false;
		return;
	}

	if (m_fMorale < m_fMoraleNormalValue) {
		vfAddActiveMember(true);
		m_bStopThinking = false;
		return;
	}
	
	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team()))) {
		vfAddActiveMember(true);
		m_bStopThinking = false;
		return;
	}
	
	m_fSpeed = 0.f;

	vfAddStandingMember();
	vfAddActiveMember();

	CSoundPlayer::play	(eRatSoundVoice,45*1000,15*1000);
}

void CAI_Rat::UnderFire()
{
	WRITE_TO_LOG("UnderFire");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	//	Msg					("%6d : Rat %s, %f -> %f [%f]",Level().timeServer(),*cName(),movement().m_body.current.pitch,movement().m_body.target.pitch,get_custom_pitch_speed(0.f));

	vfSetFire(false);

	if (memory().enemy().selected()) {
		GO_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun);
	}
	else {
		if (m_tLastSound.dwTime >= m_dwLastUpdateTime) {
			if (m_tLastSound.tpEntity && (m_tLastSound.tpEntity->g_Team() != g_Team()) && (!bfCheckIfSoundFrightful())) {
				SWITCH_TO_NEW_STATE(aiRatAttackRun);
			}
			m_previous_query_time = Level().timeServer();
			if (m_bStateChanged) {
				Fvector tTemp;
				tTemp.setHP(-movement().m_body.current.yaw,-movement().m_body.current.pitch);
				tTemp.normalize_safe();
				tTemp.mul(m_fUnderFireDistance);
				m_tSpawnPosition.add(Position(),tTemp);
			}
		}
		if (m_fMorale >= m_fMoraleNormalValue - EPS_L) {
			GO_TO_PREV_STATE;
		}
	}

	vfUpdateTime(m_fTimeUpdateDelta);

	if (m_bStateChanged)//(Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir = m_tSpawnPosition;

	m_fSpeed = m_fAttackSpeed;

	vfComputeNewPosition(true,true);
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	//	Msg			("%6d : Rat %s, %f -> %f [%f]",Level().timeServer(),*cName(),movement().m_body.current.pitch,movement().m_body.target.pitch,get_custom_pitch_speed(0.f));

	//ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,aiRatRetreat,this,30.f));
	//if (eState != m_eCurrentState)
	//	GO_TO_NEW_STATE_THIS_UPDATE(eState);

	CHECK_IF_GO_TO_PREV_STATE(!memory().enemy().selected());

	CHECK_IF_GO_TO_NEW_STATE((memory().enemy().selected()->Position().distance_to(Position()) > m_fAttackDistance),aiRatAttackRun)

		Fvector tTemp;
	tTemp.sub(memory().enemy().selected()->Position(),Position());
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE(angle_difference(movement().m_body.current.yaw,sTemp.yaw) > m_fAttackAngle,aiRatAttackRun)

		Fvector			tDistance;
	tDistance.sub	(Position(),memory().enemy().selected()->Position());

	m_fSpeed		= 0.f;

	vfSetFire		(true);

	vfSetMovementType(0);
}

void CAI_Rat::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	//	Msg			("%6d : Rat %s, %f -> %f [%f]",Level().timeServer(),*cName(),movement().m_body.current.pitch,movement().m_body.target.pitch,get_custom_pitch_speed(0.f));
	vfSetFire(false);

	ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,m_eCurrentState,aiRatRetreat,aiRatRetreat,this,30.f));
	if (eState != m_eCurrentState) {
		eState = ERatStates(dwfChooseAction(0*m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,m_eCurrentState,aiRatRetreat,aiRatRetreat,this,30.f));
		GO_TO_NEW_STATE_THIS_UPDATE(eState);
	}

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(memory().enemy().selected() && (m_tSafeSpawnPosition.distance_to(memory().enemy().selected()->Position()) > m_fMaxPursuitRadius),aiRatReturnHome);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!memory().enemy().selected());

	Fvector tDistance;
	tDistance.sub(Position(),memory().enemy().selected()->Position());

	Fvector tTemp;
	tTemp.sub(memory().enemy().selected()->Position(),Position());
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((memory().enemy().selected()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(movement().m_body.target.yaw, sTemp.yaw) <= m_fAttackAngle),aiRatAttackFire)

		if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time) {
			m_tGoalDir.set(memory().enemy().selected()->Position());
		}

		vfUpdateTime(m_fTimeUpdateDelta);

		m_fSpeed = m_fAttackSpeed;

		vfComputeNewPosition(true,true);
}

void CAI_Rat::Retreat()
{
	WRITE_TO_LOG("Retreat");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false);

	if	(!memory().enemy().selected() ||
		(memory().enemy().selected() && 
		(
		!memory().enemy().selected()->g_Alive()
		|| 
		(
		(Level().timeServer() - memory().memory(memory().enemy().selected()).m_level_time > m_dwRetreatTime) && 
		(
		(m_tLastSound.dwTime < m_dwLastUpdateTime) || 
		!m_tLastSound.tpEntity || 
		(m_tLastSound.tpEntity->g_Team() == g_Team()) || 
		!bfCheckIfSoundFrightful()
		)
		)
		)
		)
		)
	{
		memory().enable	(memory().enemy().selected(),false);
		GO_TO_PREV_STATE;
	}

	if (memory().enemy().selected() && memory().enemy().selected()->g_Alive()) {
		ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatRetreat,aiRatRetreat,this,30.f));
		//		ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,this,30.f));
		if (eState != m_eCurrentState) {
			eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatRetreat,aiRatRetreat,this,30.f));
			GO_TO_NEW_STATE_THIS_UPDATE(eState);
		}
		Fvector tTemp;
		tTemp.sub(memory().enemy().selected()->Position(),Position());
		vfNormalizeSafe(tTemp);
		SRotation sTemp;
		mk_rotation(tTemp,sTemp);

		if ((memory().enemy().selected()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(movement().m_body.target.yaw, sTemp.yaw) > m_fAttackAngle)) {
			m_fSpeed = 0.f;
			return;
		}

		CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((memory().enemy().selected()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(movement().m_body.target.yaw, sTemp.yaw) <= m_fAttackAngle),aiRatAttackFire)

			tTemp.sub(Position(),memory().enemy().selected()->Position());
		tTemp.normalize_safe();
		tTemp.mul(m_fRetreatDistance);
		m_tSpawnPosition.add(Position(),tTemp);
	}

	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir = m_tSpawnPosition;

	vfComputeNewPosition(true,true);
}

void CAI_Rat::Pursuit()
{
	WRITE_TO_LOG("Pursuit something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false);

	if (memory().enemy().selected() && (Level().timeServer() - memory().memory(memory().enemy().selected()).m_level_time >= m_dwLostMemoryTime)) {
		memory().enable(memory().enemy().selected(),false);
		GO_TO_PREV_STATE_THIS_UPDATE;
	}

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(memory().enemy().selected(),aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_fMorale < m_fMoraleNormalValue),aiRatUnderFire);

	//	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_HIT) == SOUND_TYPE_WEAPON_BULLET_HIT)) {
	if	(
		(m_tLastSound.dwTime >= m_dwLastUpdateTime) && 
		(
		!m_tLastSound.tpEntity || 
		(
		(!memory().item().selected() || (memory().item().selected()->ID() != m_tLastSound.tpEntity->ID())) && 
		(m_tLastSound.tpEntity->g_Team() != g_Team())
		)
		) && 
		!memory().enemy().selected()
		)
	{
		GO_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set(memory().memory(memory().enemy().selected()).m_object_params.m_position);

	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	vfComputeNewPosition(true,true);
}

void CAI_Rat::FreeRecoil()
{
	WRITE_TO_LOG("Free hunting : Recoil from something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	if (m_bStateChanged) {
		m_dwLostRecoilTime	= Level().timeServer();
		m_tRecoilPosition	= m_tLastSound.tSavedPosition;
	}

	vfSetFire(false);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(memory().enemy().selected());

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_dwLastUpdateTime > m_dwLostRecoilTime + 2000);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(memory().enemy().selected() && (Level().timeServer() - memory().memory(memory().enemy().selected()).m_level_time >= m_dwLostRecoilTime),aiRatPursuit);

	if (m_bStateChanged) {
		Fvector tTemp;
		tTemp.setHP(-movement().m_body.current.yaw,-movement().m_body.current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(m_fUnderFireDistance);
		m_tSpawnPosition.add(Position(),tTemp);
	}
	//	else
	//		if (m_dwLastUpdateTime > m_dwLostRecoilTime + 2000)
	//			m_tSpawnPosition = m_tGoalDir = m_tRecoilPosition;
	//
	//	m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;
	//	
	//	vfUpdateTime(m_fTimeUpdateDelta);
	//
	//	if (m_dwLastUpdateTime > m_dwLostRecoilTime + 2000) {
	//		m_fASpeed				= m_fAngleSpeed;
	//		m_fSafeSpeed = m_fSpeed = m_fMinSpeed;
	//		bfCheckIfGoalChanged	();
	//		vfComputeNewPosition	(false);
	//	}
	//	else
	//		vfComputeNewPosition(true,true);
	//	if (m_dwLastUpdateTime > m_dwLostRecoilTime + 2000) {
	//		m_tSpawnPosition		= m_tRecoilPosition;
	//		m_fGoalChangeDelta		= 1000;//m_fSafeGoalChangeDelta;
	//		m_tVarGoal.set			(m_tGoalVariation);
	//		m_tVarGoal.mul			(.1f);
	//		m_fASpeed				= m_fAngleSpeed;
	//		m_fSpeed = m_fSafeSpeed = m_fMaxSpeed;
	//
	//		bfCheckIfGoalChanged	();
	//		vfUpdateTime			(m_fTimeUpdateDelta);
	//		vfComputeNewPosition	(false);
	//	}
	//	else 
	{
		m_fSpeed = m_fSafeSpeed = m_fMaxSpeed;
		vfComputeNewPosition	(true,true);
	}

	CSoundPlayer::play	(eRatSoundVoice,45*1000,15*1000);
}

void CAI_Rat::ReturnHome()
{
	WRITE_TO_LOG("Returning home");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false);

	if (memory().enemy().selected() && (m_tSafeSpawnPosition.distance_to(memory().enemy().selected()->Position()) < m_fMaxPursuitRadius)) {
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun)
	}

	if (memory().enemy().selected()) {
		Fvector tTemp;
		tTemp.sub(memory().enemy().selected()->Position(),Position());
		vfNormalizeSafe(tTemp);
		SRotation sTemp;
		mk_rotation(tTemp,sTemp);

		if ((memory().enemy().selected()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(movement().m_body.target.yaw, sTemp.yaw) > m_fAttackAngle)) {
			m_fSpeed = 0.f;
			return;
		}

		CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((memory().enemy().selected()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(movement().m_body.target.yaw, sTemp.yaw) <= m_fAttackAngle),aiRatAttackFire)
	}

	CHECK_IF_GO_TO_PREV_STATE(!memory().enemy().selected() || !memory().enemy().selected()->g_Alive() || Position().distance_to(m_tSafeSpawnPosition) < m_fMaxHomeRadius);

	m_tSpawnPosition.set	(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	m_fSpeed = m_fSafeSpeed = m_fAttackSpeed;

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set			(m_tSafeSpawnPosition);

	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	vfComputeNewPosition();
}

void CAI_Rat::EatCorpse()
{
	WRITE_TO_LOG("Eating a corpse");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE((memory().enemy().selected() && ((memory().enemy().selected()->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (Position().distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))));

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!memory().item().selected() || (m_fMorale < m_fMoraleNormalValue));

	m_fGoalChangeTime					= 10.f;

	if	(
		(m_tLastSound.dwTime >= m_dwLastUpdateTime) && 
		(
		!m_tLastSound.tpEntity || 
		(
		(!memory().item().selected() || (memory().item().selected()->ID() != m_tLastSound.tpEntity->ID())) && 
		(m_tLastSound.tpEntity->g_Team() != g_Team())
		)
		) && 
		!memory().enemy().selected()
		)
	{
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}

	//	CSkeletonAnimated					*V= smart_cast<CSkeletonAnimated*>(const_cast<CGameObject*>(memory().item().selected())->Visual());
	//	R_ASSERT							(V);
	//	u16									head_bone = V->LL_BoneID("bip01_head");
	//	Fmatrix								l_tMatrix;
	//	l_tMatrix.mul_43					(const_cast<CGameObject*>(memory().item().selected())->XFORM(),smart_cast<CKinematics*>(const_cast<CGameObject*>(memory().item().selected())->Visual())->LL_GetBoneInstance(head_bone).mTransform);
	//	Fvector								temp_position = l_tMatrix.c;
	Fvector								temp_position;
	memory().item().selected()->Center						(temp_position);

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set					(temp_position);

	vfUpdateTime						(m_fTimeUpdateDelta);

	bool								a = temp_position.distance_to(Position()) <= m_fAttackDistance;
	Fvector								direction;
	direction.sub						(temp_position,Position());
	float								y,p;
	direction.getHP						(y,p);
	if (a && angle_difference(y,-movement().m_body.current.yaw) < PI_DIV_6) {
		m_fSpeed						= 0;
		if (Level().timeServer() - m_previous_query_time > m_dwHitInterval) {
			m_previous_query_time		= Level().timeServer();
			const CEntityAlive			*const_corpse = smart_cast<const CEntityAlive*>(memory().item().selected());
			VERIFY						(const_corpse);
			CEntityAlive				*corpse = const_cast<CEntityAlive*>(const_corpse);
			VERIFY						(corpse);
			corpse->m_fFood				-= m_fHitPower/10.f;
		}
		m_bFiring = true;
		vfComputeNewPosition			(false);
		CSoundPlayer::play				(eRatSoundEat);
	}
	else {
		CSoundPlayer::remove_active_sounds(u32(-1));
		if (!a)
			m_fSpeed					= m_fMaxSpeed;
		else
			m_fSpeed					= 0.f;
		vfComputeNewPosition			(true,true);
	}
}

void CAI_Rat::vfUpdateSpawnPosition()
{
	if (!g_Alive())
		return;

	CEntity			*leader = Level().seniority_holder().team(g_Team()).squad(g_Squad()).leader();
	VERIFY			(leader);

	if (ID() != leader->ID())	{
		CAI_Rat		*rat_leader = smart_cast<CAI_Rat*>(leader);
		if (rat_leader) {
			if (m_tSafeSpawnPosition.distance_to(rat_leader->m_tSafeSpawnPosition) > EPS_L) {
				vfAddActiveMember(true);
				m_eCurrentState = aiRatFreeHuntingActive;
			}
			m_tSafeSpawnPosition = rat_leader->m_tSafeSpawnPosition;
		}
	}
	else {
		if ((Level().timeServer() >= m_dwTimeToChange) && (ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id() == m_tNextGP)) {
			m_tNextGP					= ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id();
			vfChooseNextGraphPoint		();
			m_tSafeSpawnPosition.set	(ai().game_graph().vertex(m_tNextGP)->level_point());
		}
	}
}

void CAI_Rat::test_movement	()
{
	m_bStopThinking			= true;
	m_fSpeed				= m_fMaxSpeed;
	vfComputeNewPosition	(true,true);
}