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
			case aiRatTurn : {
				Turn();
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
		m_bStateChanged = m_ePreviousState != m_eCurrentState;
	}
	while (!m_bStopThinking);
}

void CAI_Rat::Death()
{
	//WRITE_TO_LOG	("Dying...");
	vfSetFire		(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	m_bStopThinking = true;

	CGroup			&Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire		(false,Group);
	
	SelectAnimation	(XFORM().k,direction(),0);

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

void CAI_Rat::Turn()
{
	WRITE_TO_LOG("Turning...");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	m_tHPB.x			= -m_body.current.yaw;

	CHECK_IF_GO_TO_PREV_STATE(angle_difference(m_body.target.yaw, m_body.current.yaw) <= PI_DIV_6)
	
	INIT_SQUAD_AND_LEADER
	
	m_body.speed		= PI_MUL_2;

	m_fSpeed			= 0;

	CSoundPlayer::play	(eRatSoundVoice,45*1000,15*1000);
}

void CAI_Rat::FreeHuntingActive()
{
	WRITE_TO_LOG("Free hunting active");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((enemy() && ((enemy()->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (Position().distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))),aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue,aiRatUnderFire);
	
	if	(
			(m_tLastSound.dwTime >= m_dwLastUpdateTime) && 
			(
				!m_tLastSound.tpEntity || 
				(
					(!item() || (item()->ID() != m_tLastSound.tpEntity->ID())) && 
					(m_tLastSound.tpEntity->g_Team() != g_Team())
				)
			) && 
			!enemy()
		)
	{
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}
    
	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(item(),aiRatEatCorpse);
	
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
			vfRemoveActiveMember();
		}
	}

	if (m_bStateChanged)
		vfChooseNewSpeed();

	vfUpdateTime(m_fTimeUpdateDelta);

	if (bfComputeNewPosition(false))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
	
	CSoundPlayer::play	(eRatSoundVoice,45*1000,15*1000);
}

void CAI_Rat::FreeHuntingPassive()
{
	WRITE_TO_LOG("Free hunting passive");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (enemy()) {
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

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (enemy()) {
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
				tTemp.setHP(-m_body.current.yaw,-m_body.current.pitch);
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

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
}

void CAI_Rat::AttackFire()
{
	WRITE_TO_LOG("Attacking enemy...");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	//ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,aiRatRetreat,this,30.f));
	//if (eState != m_eCurrentState)
	//	GO_TO_NEW_STATE_THIS_UPDATE(eState);

	CHECK_IF_GO_TO_PREV_STATE(!enemy());
		
	CHECK_IF_GO_TO_NEW_STATE((enemy()->Position().distance_to(Position()) > m_fAttackDistance),aiRatAttackRun)

	Fvector tTemp;
	tTemp.sub(enemy()->Position(),Position());
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);
	
	CHECK_IF_GO_TO_NEW_STATE(angle_difference(m_body.current.yaw,sTemp.yaw) > m_fAttackAngle,aiRatAttackRun)
		
	Fvector			tDistance;
	tDistance.sub	(Position(),enemy()->Position());
	
	vfAimAtEnemy	();

	m_fSpeed		= 0.f;

	vfSetFire		(true,Level().get_group(g_Team(),g_Squad(),g_Group()));
	
	vfSetMovementType(0);
}

void CAI_Rat::AttackRun()
{
	WRITE_TO_LOG("Attack enemy");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,m_eCurrentState,aiRatRetreat,aiRatRetreat,this,30.f));
	if (eState != m_eCurrentState) {
		eState = ERatStates(dwfChooseAction(0*m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),m_eCurrentState,m_eCurrentState,m_eCurrentState,aiRatRetreat,aiRatRetreat,this,30.f));
		GO_TO_NEW_STATE_THIS_UPDATE(eState);
	}

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(enemy() && (m_tSafeSpawnPosition.distance_to(enemy()->Position()) > m_fMaxPursuitRadius),aiRatReturnHome);

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!enemy());

	Fvector tDistance;
	tDistance.sub(Position(),enemy()->Position());
	
	Fvector tTemp;
	tTemp.sub(enemy()->Position(),Position());
	vfNormalizeSafe(tTemp);
	SRotation sTemp;
	mk_rotation(tTemp,sTemp);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((enemy()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(m_body.target.yaw, sTemp.yaw) <= m_fAttackAngle),aiRatAttackFire)

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time) {
		m_tGoalDir.set(enemy()->Position());
	}
	
	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
}

void CAI_Rat::Retreat()
{
	WRITE_TO_LOG("Retreat");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (enemy() && enemy()->g_Alive()) {
		ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatRetreat,aiRatRetreat,this,30.f));
//		ERatStates eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,this,30.f));
		if (eState != m_eCurrentState) {
			eState = ERatStates(dwfChooseAction(m_dwActionRefreshRate,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,m_fAttackSuccessProbability,g_Team(),g_Squad(),g_Group(),aiRatAttackRun,aiRatAttackRun,aiRatAttackRun,aiRatRetreat,aiRatRetreat,this,30.f));
			GO_TO_NEW_STATE_THIS_UPDATE(eState);
		}
		Fvector tTemp;
		tTemp.sub(enemy()->Position(),Position());
		vfNormalizeSafe(tTemp);
		SRotation sTemp;
		mk_rotation(tTemp,sTemp);

		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((enemy()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(m_body.target.yaw, sTemp.yaw) > m_fAttackAngle),aiRatTurn);

		CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((enemy()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(m_body.target.yaw, sTemp.yaw) <= m_fAttackAngle),aiRatAttackFire)

		tTemp.sub(Position(),enemy()->Position());
		tTemp.normalize_safe();
		tTemp.mul(m_fRetreatDistance);
		m_tSpawnPosition.add(Position(),tTemp);
	}
	else
		if	(
				enemy() && 
				(
					!enemy()->g_Alive()
					|| 
					(
						(Level().timeServer() - memory(enemy()).m_level_time > m_dwRetreatTime) && 
						(
							(m_tLastSound.dwTime < m_dwLastUpdateTime) || 
							!m_tLastSound.tpEntity || 
							(m_tLastSound.tpEntity->g_Team() == g_Team()) || 
							!bfCheckIfSoundFrightful()
						)
					)
				)
			)
		{
			GO_TO_PREV_STATE;
		}

	vfUpdateTime(m_fTimeUpdateDelta);
	
	m_fSpeed = m_fAttackSpeed;

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir = m_tSpawnPosition;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
}

void CAI_Rat::Pursuit()
{
	WRITE_TO_LOG("Pursuit something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (enemy() && (Level().timeServer() - memory(enemy()).m_level_time >= m_dwLostMemoryTime)) {
		enable(enemy(),false);
		GO_TO_PREV_STATE_THIS_UPDATE;
	}

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE(enemy(),aiRatAttackRun);

	CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((m_fMorale < m_fMoraleNormalValue),aiRatUnderFire);

//	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((m_tLastSound.eSoundType & SOUND_TYPE_WEAPON_BULLET_HIT) == SOUND_TYPE_WEAPON_BULLET_HIT)) {
	if	(
			(m_tLastSound.dwTime >= m_dwLastUpdateTime) && 
			(
				!m_tLastSound.tpEntity || 
				(
					(!item() || (item()->ID() != m_tLastSound.tpEntity->ID())) && 
					(m_tLastSound.tpEntity->g_Team() != g_Team())
				)
			) && 
			!enemy()
		)
	{
		GO_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set(memory(enemy()).m_object_params.m_position);
	
	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
}

void CAI_Rat::FreeRecoil()
{
	WRITE_TO_LOG("Free hunting : Recoil from something");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(enemy());

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(m_fMorale < m_fMoraleNormalValue);

	CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE(enemy() && (Level().timeServer() - memory(enemy()).m_level_time >= m_dwLostRecoilTime),aiRatPursuit);

	if ((m_tLastSound.dwTime >= m_dwLastUpdateTime) && ((!m_tLastSound.tpEntity) || (m_tLastSound.tpEntity->g_Team() != g_Team()))) {
		Fvector tTemp;
		tTemp.setHP(m_body.current.yaw,m_body.current.pitch);
		tTemp.normalize_safe();
		tTemp.mul(m_fUnderFireDistance);
		m_tSpawnPosition.add(Position(),tTemp);
	}
	
	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set(m_tSpawnPosition);
	
	m_fSafeSpeed = m_fSpeed = m_fMaxSpeed;
	
	vfUpdateTime(m_fTimeUpdateDelta);

	if (bfComputeNewPosition(true,true))
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);

	CSoundPlayer::play	(eRatSoundVoice,45*1000,15*1000);
}

void CAI_Rat::ReturnHome()
{
	WRITE_TO_LOG("Returning home");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	if (enemy() && (m_tSafeSpawnPosition.distance_to(enemy()->Position()) < m_fMaxPursuitRadius)) {
		m_fGoalChangeTime = 0;
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatAttackRun)
	}
 
	if (enemy()) {
		Fvector tTemp;
		tTemp.sub(enemy()->Position(),Position());
		vfNormalizeSafe(tTemp);
		SRotation sTemp;
		mk_rotation(tTemp,sTemp);

		CHECK_IF_SWITCH_TO_NEW_STATE_THIS_UPDATE((enemy()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(m_body.target.yaw, sTemp.yaw) > m_fAttackAngle),aiRatTurn);

		CHECK_IF_GO_TO_NEW_STATE_THIS_UPDATE((enemy()->Position().distance_to(Position()) <= m_fAttackDistance) && (angle_difference(m_body.target.yaw, sTemp.yaw) <= m_fAttackAngle),aiRatAttackFire)
	}

	CHECK_IF_GO_TO_PREV_STATE(!enemy() || !enemy()->g_Alive() || Position().distance_to(m_tSafeSpawnPosition) < m_fMaxHomeRadius);

	m_tSpawnPosition.set	(m_tSafeSpawnPosition);
	m_fGoalChangeDelta		= m_fSafeGoalChangeDelta;
	m_tVarGoal.set			(m_tGoalVariation);
	m_fASpeed				= m_fAngleSpeed;
	m_fSpeed = m_fSafeSpeed = m_fAttackSpeed;

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set			(m_tSafeSpawnPosition);

	vfUpdateTime(m_fTimeUpdateDelta);

	m_fSpeed = m_fAttackSpeed;

	if (bfComputeNewPosition())
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
}

void CAI_Rat::EatCorpse()
{
	WRITE_TO_LOG("Eating a corpse");

	if (!g_Alive()) {
		m_fSpeed = m_fSafeSpeed = 0;
		return;
	}

	vfSetFire(false,Level().get_group(g_Team(),g_Squad(),g_Group()));

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE((enemy() && ((enemy()->Position().distance_to(m_tSafeSpawnPosition) < m_fMaxPursuitRadius) || (Position().distance_to(m_tSafeSpawnPosition) > m_fMaxHomeRadius))));

//	if (item()) {
//		vfChangeGoal();
//		m_fGoalChangeTime = 10.f;
//	}

	CHECK_IF_GO_TO_PREV_STATE_THIS_UPDATE(!item() || (m_fMorale < m_fMoraleNormalValue));

	m_fGoalChangeTime					= 10.f;

	if	(
			(m_tLastSound.dwTime >= m_dwLastUpdateTime) && 
			(
				!m_tLastSound.tpEntity || 
				(
					(!item() || (item()->ID() != m_tLastSound.tpEntity->ID())) && 
					(m_tLastSound.tpEntity->g_Team() != g_Team())
				)
			) && 
			!enemy()
		)
	{
		SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatFreeRecoil);
	}

//	CSkeletonAnimated					*V= PSkeletonAnimated(const_cast<CGameObject*>(item())->Visual());
//	R_ASSERT							(V);
//	u16									head_bone = V->LL_BoneID("bip01_head");
//	Fmatrix								l_tMatrix;
//	l_tMatrix.mul_43					(const_cast<CGameObject*>(item())->XFORM(),PKinematics(const_cast<CGameObject*>(item())->Visual())->LL_GetBoneInstance(head_bone).mTransform);
//	Fvector								temp_position = l_tMatrix.c;
	Fvector								temp_position;
	item()->Center						(temp_position);

	if ((Level().timeServer() - m_previous_query_time > TIME_TO_GO) || !m_previous_query_time)
		m_tGoalDir.set					(temp_position);

	vfUpdateTime						(m_fTimeUpdateDelta);

	bool								a = temp_position.distance_to(Position()) <= m_fAttackDistance;
	Fvector								direction;
	direction.sub						(temp_position,Position());
	float								y,p;
	direction.getHP						(y,p);
	if (a && angle_difference(y,-m_body.current.yaw) < PI_DIV_6) {
		m_fSpeed						= 0;
		if (Level().timeServer() - m_previous_query_time > m_dwHitInterval) {
			m_previous_query_time		= Level().timeServer();
			const CEntityAlive			*const_corpse = dynamic_cast<const CEntityAlive*>(item());
			VERIFY						(const_corpse);
			CEntityAlive				*corpse = const_cast<CEntityAlive*>(const_corpse);
			VERIFY						(corpse);
			corpse->m_fFood				-= m_fHitPower/10.f;
		}
		m_bFiring = true;
		CSoundPlayer::play				(eRatSoundEat);
	}
	else {
		CSoundPlayer::set_sound_mask	(u32(-1));
		CSoundPlayer::set_sound_mask	(0);

		bool							a = false;
		if (true || (temp_position.distance_to(Position()) > 3.5f) || ((temp_position.distance_to(Position()) > 2.5f) && !fsimilar(m_fSpeed,m_fMinSpeed))) 
		{
			m_fSpeed					= m_fMaxSpeed;
			a							= bfComputeNewPosition(true,true);
		}
		else {
			m_fSpeed					= m_fMinSpeed;
			a							= bfComputeNewPosition(false);
		}
		if (a)
			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
//		m_fSpeed						= m_fMaxSpeed;
//		if (bfComputeNewPosition(true,true))
//			SWITCH_TO_NEW_STATE_THIS_UPDATE(aiRatTurn);
	}
}

void CAI_Rat::vfUpdateSpawnPosition()
{
	if (!g_Alive())
		return;
	INIT_SQUAD_AND_LEADER;
	if (this != Leader)	{
		CAI_Rat *tpLeader = dynamic_cast<CAI_Rat*>(Leader);
		if (tpLeader) {
			if (m_tSafeSpawnPosition.distance_to(tpLeader->m_tSafeSpawnPosition) > EPS_L) {
				vfAddActiveMember(true);
				m_eCurrentState = aiRatFreeHuntingActive;
			}
			m_tSafeSpawnPosition = tpLeader->m_tSafeSpawnPosition;
		}
	}
	else {
		if ((Level().timeServer() >= m_dwTimeToChange) && (ai().cross_table().vertex(level_vertex_id()).game_vertex_id() == m_tNextGP)) {
			m_tNextGP					= ai().cross_table().vertex(level_vertex_id()).game_vertex_id();
			vfChooseNextGraphPoint		();
			m_tSafeSpawnPosition.set	(ai().game_graph().vertex(m_tNextGP)->level_point());
		}
	}
}