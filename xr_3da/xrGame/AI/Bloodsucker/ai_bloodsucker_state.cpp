#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_state.h"
#include "..\\rat\\ai_rat.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bloodsucker Rest implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBloodsuckerRest::CBloodsuckerRest(CAI_Bloodsucker *p)  
{
	pMonster = p;
	Reset();

	SetLowPriority();
}


void CBloodsuckerRest::Reset()
{
	IState::Reset();

	m_dwReplanTime		= 0;
	m_dwLastPlanTime	= 0;

	m_tAction			= ACTION_WALK;

	pMonster->SetMemoryTimeDef();

}

void CBloodsuckerRest::Init()
{
	IState::Init();

	WRITE_TO_LOG("_ Blood State Init _");
}

void CBloodsuckerRest::Replanning()
{
	m_dwLastPlanTime = m_dwCurrentTime;	

	u32		dwMinRand, dwMaxRand;
	u8		day_time = Level().GetDayTime();

	if ((day_time >= pMonster->m_dwDayTimeBegin) && (day_time <= pMonster->m_dwDayTimeEnd)) {  // ����?

		bool bNormalSatiety = (pMonster->GetSatiety() > pMonster->m_fMinSatiety) && (pMonster->GetSatiety() < pMonster->m_fMaxSatiety); 
		if (bNormalSatiety) {		// ����� �� ����
			WRITE_TO_LOG("ACTION_SIT");
			m_tAction = ACTION_SIT;

			// ������ �������� �����
//			if ((pMonster->m_tAnim != eAnimLieIdle) && (pMonster->m_tAnim != eAnimSitIdle)) {
//				pMonster->Motion.m_tSeq.Add(eAnimStandSitDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
//				pMonster->Motion.m_tSeq.Switch();
//			}
			
			dwMinRand = pMonster->m_timeLieIdleMin;	
			dwMaxRand = pMonster->m_timeLieIdleMax;

		} else {					// ������, ���� ���
			WRITE_TO_LOG("ACTION_WALK");
			m_tAction = ACTION_WALK;

			// ��������� ���� ������ ����� �����, ����� ����
			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = pMonster->m_timeFreeWalkMin;	
			dwMaxRand = pMonster->m_timeFreeWalkMax;
		}
	} else { 
		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // �������
			// �����
			WRITE_TO_LOG("ACTION_SLEEP");
			m_tAction = ACTION_SLEEP;

			// ������ �������� �����
//			if ((pMonster->m_tAnim != eAnimLieIdle) && (pMonster->m_tAnim != eAnimSitIdle) && (pMonster->m_tAnim != eAnimSleep)) {
//				pMonster->Motion.m_tSeq.Add(eAnimStandSitDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
//				pMonster->Motion.m_tSeq.Add(eAnimSitToSleep,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
//				pMonster->Motion.m_tSeq.Switch();
//			}

			dwMinRand = pMonster->m_timeSleepMin; 
			dwMaxRand = pMonster->m_timeSleepMax;

		} else {
			// ������� (������������), ����� ������������� �� ��������
			WRITE_TO_LOG("ACTION_WALK_CIRCUMSPECTION");

			m_tAction = ACTION_WALK_CIRCUMSPECTION;

			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = pMonster->m_timeFreeWalkMin; 
			dwMaxRand = pMonster->m_timeFreeWalkMax;
		}
	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
}


void CBloodsuckerRest::Run()
{
	// ��������� ����� �� �������� ��������������
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();
	
	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_SIT: 
//			pMonster->Motion.m_tParams.SetParams(eAnimSitIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			break;
		case ACTION_WALK: // ����� ����� �����
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
//			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimWalkFwd, eAnimWalkFwd,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
		case ACTION_SLEEP:
//			pMonster->Motion.m_tParams.SetParams(eAnimSleep,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			break;
		case ACTION_WALK_CIRCUMSPECTION:
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
//			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimWalkFwd, eAnimWalkFwd,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
	}
}

TTime CBloodsuckerRest::UnlockState(TTime cur_time)
{
	return cur_time;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bloodsucker Eat implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBloodsuckerEat::CBloodsuckerEat(CAI_Bloodsucker *p)  
{
	pMonster = p;
	Reset();
	SetLowPriority();
}
void CBloodsuckerEat::Reset()
{
	IState::Reset();

	pCorpse				= 0;
	m_dwLastTimeEat		= 0;
	m_dwEatInterval		= 1000;

}
void CBloodsuckerEat::Init()
{
	IState::Init();

	// �������� ���� � �����
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	bTastyCorpse = ((tpRat) ? false : true); 

	m_fDistToCorpse		= 0.8f;
	m_fWalkDistToCorpse = 5.f;

	// ���� ����� ������� - ������ � �����
	if (pMonster->GetSatiety() < 0.5f) m_tAction = ACTION_CORPSE_APPROACH_RUN;
	else m_tAction = ACTION_CORPSE_APPROACH_WALK;

	// Test
	WRITE_TO_LOG("_ Eat Init _");
}

void CBloodsuckerEat::Run()
{
	// ���� ����� ����, ����� ���������������� ��������� 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	if (pCorpse != ve.obj) Init();

	float dist = pMonster->Position().distance_to(pCorpse->Position());
	if ((m_tAction != ACTION_EAT)  && (dist < m_fWalkDistToCorpse) && (dist > m_fDistToCorpse)) {
		m_tAction = ACTION_CORPSE_APPROACH_WALK;
	} else if ((m_tAction != ACTION_EAT)  &&  (dist < m_fDistToCorpse)) {
		// ������� �������� � �����
		if (!bTastyCorpse) {
			pMonster->AddIgnoreObject(pCorpse);
			// ������ �������� (������ ����)
//			pMonster->Motion.m_tSeq.Add(eAnimWalkBkwd,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
//			pMonster->Motion.m_tSeq.Switch();
			
		} else {
			// ������ �������� �����
//			pMonster->Motion.m_tSeq.Add(eAnimAttack,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
//			pMonster->Motion.m_tSeq.Add(eAnimStandSitDown,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
//			pMonster->Motion.m_tSeq.Switch();
			m_tAction = ACTION_EAT;
		}
	}

	switch (m_tAction) {
		case ACTION_CORPSE_APPROACH_RUN:

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

//			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimRun,eAnimRun, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			break;
		case ACTION_CORPSE_APPROACH_WALK:
			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

//			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimWalkFwd, eAnimWalkFwd,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);

			break;
		case ACTION_EAT:
//			pMonster->Motion.m_tParams.SetParams(eAnimEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Clear();

			if (pMonster->GetSatiety() >= 1.0f) pMonster->flagEatNow = false;
			else pMonster->flagEatNow = true;
		
			// ������ �����
			DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
				pMonster->ChangeSatiety(0.05f);
				pCorpse->m_fFood -= pMonster->m_fHitPower/5.f;
			DO_IN_TIME_INTERVAL_END();
			
			break;
	}
}

void CBloodsuckerEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bloodsucker attack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBloodsuckerAttack::CBloodsuckerAttack(CAI_Bloodsucker *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBloodsuckerAttack::Reset()
{
	IState::Reset();

	m_tAction			= ACTION_RUN;

	m_tEnemy.obj		= 0;

	m_fDistMin			= 0.f;	
	m_fDistMax			= 0.f;

	m_dwFaceEnemyLastTime	= 0;
	m_dwFaceEnemyLastTimeInterval = 1200;

}
void CBloodsuckerAttack::Init()
{
	IState::Init();

	// �������� �����
	m_tEnemy = pMonster->m_tEnemy;

	// ��������� ��������� ������
	m_fDistMin = 1.4f;
	m_fDistMax = 2.8f;

	pMonster->SetMemoryTimeDef();

	// Test
	WRITE_TO_LOG("_ Attack Init _");
}

void CBloodsuckerAttack::Run()
{
	// ���� ���� ���������, ���������������� ���������
	if (!pMonster->m_tEnemy.obj) R_ASSERT("Enemy undefined!!!");
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) {
		Reset();
		Init();
	} else m_tEnemy = pMonster->m_tEnemy;

	// ����� ���������
	bool bAttackMelee = (m_tAction == ACTION_ATTACK_MELEE);

	float dist = m_tEnemy.obj->Position().distance_to(pMonster->Position());

	if (bAttackMelee && (dist < m_fDistMax)) 
		m_tAction = ACTION_ATTACK_MELEE;
	else 
		m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_ATTACK_MELEE);

	// ���� ���� �� ����� - ������ � ����
	if (m_tAction == ACTION_ATTACK_MELEE && (m_tEnemy.time != m_dwCurrentTime)) {
		m_tAction = ACTION_RUN;
	}
	
	// �����������
	if ((dist < pMonster->m_fInvisibilityDist) && (pMonster->GetPower() > pMonster->m_fPowerThreshold)) {
		if (pMonster->m_tVisibility.Switch(false)) {
			pMonster->ChangePower(pMonster->m_ftrPowerDown);
			pMonster->ActivateEffector(pMonster->m_tVisibility.GetInvisibleInterval() / 1000.f);
		}
	}
	
	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		// ������ �� �����

			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, 300);

//			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimRun,eAnimRun, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			break;
		case ACTION_ATTACK_MELEE:		// ��������� ��������
			// �������� �� ����� 
			float yaw, pitch;
			if (m_dwFaceEnemyLastTime + m_dwFaceEnemyLastTimeInterval < m_dwCurrentTime) {

				m_dwFaceEnemyLastTime = m_dwCurrentTime;
				pMonster->AI_Path.TravelPath.clear();

				Fvector EnemyCenter;
				Fvector MyCenter;

				m_tEnemy.obj->Center(EnemyCenter);
				pMonster->Center(MyCenter);

				EnemyCenter.sub(MyCenter);
				EnemyCenter.getHP(yaw,pitch);
				yaw *= -1;
				yaw = angle_normalize(yaw);
			} else yaw = pMonster->r_torso_target.yaw;

			// set motion params
//			pMonster->Motion.m_tParams.SetParams(eAnimAttack,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
//			pMonster->Motion.m_tTurn.Set(eAnimRun, eAnimRun, 0, pMonster->m_ftrAttackFastRSpeed,pMonster->m_ftrRunAttackMinAngle);
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// State Hear NDE Sound __DONE__
//////////////////////////////////////////////////////////////////////////
CBloodsuckerHearNDE::CBloodsuckerHearNDE(CAI_Bloodsucker *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBloodsuckerHearNDE::Init()
{	
	IState::Init();	
	bool bTemp;
	pMonster->GetSound(m_tSound, bTemp);

	flag_once_1 = false;
	flag_once_2 = false;

	m_tAction = ACTION_LOOK_DESTINATION;

}
void CBloodsuckerHearNDE::Run()
{
	// ���� ����� ����, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:			// �����������  ������� �����
		DO_ONCE_BEGIN(flag_once_1);
		pMonster->AI_Path.TravelPath.clear();
		pMonster->LookPosition(m_tSound.position, PI_DIV_3);
		DO_ONCE_END();

//		pMonster->Motion.m_tParams.SetParams(eAnimStandIdle, 0, 0, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//		pMonster->Motion.m_tTurn.Set(eAnimStandTurnLeft, eAnimStandTurnRight, 0, PI_DIV_3, PI_DIV_6/20);

		if (m_dwStateStartedTime + 3000 < m_dwCurrentTime) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		break;
	case ACTION_GOTO_SOUND_SOURCE:			// ���� � ���������

		DO_ONCE_BEGIN(flag_once_2);
			pMonster->vfInitSelector(pMonster->m_tSelectorHearSnd, true);
			pMonster->m_tSelectorHearSnd.m_tEnemyPosition		= m_tSound.position;
		DO_ONCE_END();

		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorHearSnd,0, true, 0, 300);
//		pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);		
//		pMonster->Motion.m_tTurn.Set(eAnimWalkFwd, eAnimWalkFwd,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// State Hear DNE Sound
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBloodsuckerHearDNE::CBloodsuckerHearDNE(CAI_Bloodsucker *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBloodsuckerHearDNE::Init()
{	
	IState::Init();	
	bool bTemp;
	pMonster->GetSound(m_tSound, bTemp);

	flag_once_1				= false;
	SavedPosition			= pMonster->Position();
	StartPosition			= SavedPosition;			// ��������� ��������� �������
	LastPosition.set		(0.f,0.f,0.f);			
	m_fRunAwayDist			= 50.f;
	m_dwLastPosSavedTime	= 0;
	m_dwStayLastTimeCheck	= m_dwCurrentTime;

	m_tAction				= ACTION_RUN_AWAY;

	SetInertia				(20000);
}

void CBloodsuckerHearDNE::Run()
{
	// ���� ����� ����, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();

	// �����������
	if (pMonster->GetPower() > pMonster->m_fPowerThreshold) {
		if (pMonster->m_tVisibility.Switch(false)) pMonster->ChangePower(pMonster->m_ftrPowerDown);
	}
	
	switch (m_tAction) {
	case ACTION_RUN_AWAY: // ������� �� N ������ �� �����
		Msg(" DNE : [RUN AWAY]");

		pMonster->Path_GetAwayFromPoint (0, m_tSound.position, m_fRunAwayDist, 2000);

//		pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//		pMonster->Motion.m_tTurn.Set(eAnimRun,eAnimRun, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

		// ������ ������ ��������� ������� �������, � ����� ������������ � ���������� � ��������� �������
		// �.�. ������������ �����
		if (m_dwLastPosSavedTime + 1000 < m_dwCurrentTime) {
			m_dwLastPosSavedTime = m_dwCurrentTime;
			SavedPosition = pMonster->Position();
		}
		
		// ��������� ������� �������� � ����. ��������� (����������� ���������� || ����� �� ����� ����� 2 ���)
		if (pMonster->Position().distance_to(StartPosition) > m_fRunAwayDist) m_tAction = ACTION_LOOK_BACK_POSITION;
		if (m_dwStayLastTimeCheck + 2000 < m_dwCurrentTime)	{
			m_dwStayLastTimeCheck = m_dwCurrentTime;
			if (pMonster->Position().similar(LastPosition)) m_tAction = ACTION_LOOK_BACK_POSITION;
			LastPosition = pMonster->Position();
		}
		break;
	case ACTION_LOOK_BACK_POSITION:			// ����������� � ������� �����
		Msg("DNE : [LOOK_BACK_POSITION]");
		DO_ONCE_BEGIN(flag_once_1);
			pMonster->AI_Path.TravelPath.clear();
			pMonster->LookPosition(SavedPosition, PI_DIV_3);
		DO_ONCE_END();

//		pMonster->Motion.m_tParams.SetParams(eAnimStandIdle, 0, 0, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//		pMonster->Motion.m_tTurn.Set(eAnimStandTurnLeft, eAnimStandTurnRight, 0, PI_DIV_3, PI_DIV_6/20);

		// ���� ��� ����������, ������� � ����. ���������
		if (getAI().bfTooSmallAngle(pMonster->r_torso_current.yaw, pMonster->r_torso_target.yaw, PI_DIV_6/6)) m_tAction = ACTION_LOOK_AROUND;
		break;

	case ACTION_LOOK_AROUND:
		Msg("DNE : [LOOK_AROUND]");
//		pMonster->Motion.m_tParams.SetParams(eAnimStandIdle, 0, 0, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//		pMonster->Motion.m_tTurn.Clear();
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// State Panic
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBloodsuckerPanic::CBloodsuckerPanic(CAI_Bloodsucker *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBloodsuckerPanic::Reset()
{
	inherited::Reset();

	m_tEnemy.obj	= 0;

	cur_pos.set		(0.f,0.f,0.f);
	prev_pos		= cur_pos;
	bFacedOpenArea	= false;
	m_dwStayTime	= 0;

}

void CBloodsuckerPanic::Init()
{	
	IState::Init();	

	// �������� �����
	m_tEnemy = pMonster->m_tEnemy;

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	// Test
	WRITE_TO_LOG("_ Panic Init _");
}


void CBloodsuckerPanic::Run()
{
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (m_dwStayTime != 0) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->AI_Path.TravelPath.clear();

		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI);

//		pMonster->Motion.m_tSeq.Add(eAnimStandTurnLeft,0,pMonster->m_ftrScaredRSpeed * 2.0f,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);		
//		pMonster->Motion.m_tSeq.Switch();
	} 

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (m_dwStayTime == 0) m_dwStayTime = m_dwCurrentTime;

	
	// �����������
	if (pMonster->GetPower() > pMonster->m_fPowerThreshold) {
		if (pMonster->m_tVisibility.Switch(false)) pMonster->ChangePower(pMonster->m_ftrPowerDown);
	}

	// ������� ����
	pMonster->vfInitSelector(pMonster->m_tSelectorFreeHunting, false);
	pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + pMonster->m_tSelectorFreeHunting.m_fSearchRange;
	pMonster->m_tSelectorFreeHunting.m_fOptEnemyDistance = pMonster->m_tSelectorFreeHunting.m_fMaxEnemyDistance;
	pMonster->m_tSelectorFreeHunting.m_fMinEnemyDistance = m_tEnemy.position.distance_to(pMonster->Position()) + 3.f;
	pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorFreeHunting, 0, true, 0,2000);

	if (!bFacedOpenArea) {
//		pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//		pMonster->Motion.m_tTurn.Set(eAnimRun,eAnimRun, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);
	} else {
		// try to rebuild path
		if (pMonster->AI_Path.TravelPath.size() > 5) {
//			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Set(eAnimRun,eAnimRun, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);
		} else {
//			pMonster->Motion.m_tParams.SetParams(eAnimStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
//			pMonster->Motion.m_tTurn.Clear();
		}
	}

	prev_pos = cur_pos;
}

