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

	u8	day_time = Level().GetDayTime();
	// temp: force daytime
	day_time = 12;

	if ((day_time >= 10) && (day_time <= 18)) {  // ����?

		bool bNormalSatiety = (pMonster->GetSatiety() > 0.6f) && (pMonster->GetSatiety() < 0.9f); 
		if (bNormalSatiety) {		// ����� �� ����
			WRITE_TO_LOG("ACTION_SIT");
			m_tAction = ACTION_SIT;

			dwMinRand = 3000;	dwMaxRand = 5000;
		} else {					// ������, ���� ���
			WRITE_TO_LOG("ACTION_WALK");
			m_tAction = ACTION_WALK;

			// ��������� ���� ������ ����� �����
			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = 3000;	dwMaxRand = 5000;
		}
	} else { 
		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // �������
			// �����
			WRITE_TO_LOG("ACTION_SLEEP");

			m_tAction = ACTION_SLEEP;
			dwMinRand = 3000; dwMaxRand = 5000;

		} else {
			// ������� (������������), ����� ������������� �� ��������
			WRITE_TO_LOG("ACTION_WALK_CIRCUMSPECTION");

			m_tAction = ACTION_WALK_CIRCUMSPECTION;

			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = 3000; dwMaxRand = 5000;
		}
	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);
}


void CBloodsuckerRest::Run()
{
	// ��������� ����� �� �������� ��������������
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();
	
	pMonster->Motion.m_tTurn.Clear();

	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_SIT: 
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			break;
		case ACTION_WALK: // ����� ����� �����
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
		case ACTION_SLEEP:
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			break;
		case ACTION_WALK_CIRCUMSPECTION:
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionStandIdle, eMotionStandIdle,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			break;
	}
}

TTime CBloodsuckerRest::UnlockState(TTime cur_time)
{
	TTime dt = inherited::UnlockState(cur_time);

	m_dwReplanTime		+= dt;
	m_dwLastPlanTime	+= dt;

	return dt;
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
	if (!pMonster->GetEnemy(ve)) R_ASSERT(false);
	if (pCorpse != ve.obj) {
		Reset();
		Init();
	}	

	float dist = pMonster->Position().distance_to(pCorpse->Position());
	if ((m_tAction != ACTION_EAT)  && (dist < m_fWalkDistToCorpse) && (dist > m_fDistToCorpse)) {
		m_tAction = ACTION_CORPSE_APPROACH_WALK;
	} else if ((m_tAction != ACTION_EAT)  &&  (dist < m_fDistToCorpse)) {
		// ������� �������� � �����
		if (!bTastyCorpse) {
			pMonster->AddIgnoreObject(pCorpse);
			// ������ �������� (������ ����)
			pMonster->Motion.m_tSeq.Add(eMotionWalkBkwd,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			pMonster->Motion.m_tSeq.Switch();
			
		} else {
			// ������ �������� �����
			pMonster->Motion.m_tSeq.Add(eMotionCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			pMonster->Motion.m_tSeq.Add(eMotionLieDownEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			pMonster->Motion.m_tSeq.Switch();
			m_tAction = ACTION_EAT;
		}
	}

	switch (m_tAction) {
		case ACTION_CORPSE_APPROACH_RUN:

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			break;
		case ACTION_CORPSE_APPROACH_WALK:
			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

			pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);

			break;
		case ACTION_EAT:
			pMonster->Motion.m_tParams.SetParams(eMotionEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();

			if (pMonster->GetSatiety() >= 1.0f) pMonster->flagEatNow = false;
			else pMonster->flagEatNow = true;
		
			// ������ �����
			if (m_dwLastTimeEat + m_dwEatInterval < m_dwCurrentTime) {
				pMonster->ChangeSatiety(0.05f);
				pCorpse->m_fFood -= pMonster->m_fHitPower/5.f;
				m_dwLastTimeEat = m_dwCurrentTime;
			}
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
		if (pMonster->m_tVisibility.Switch(false)) pMonster->ChangePower(pMonster->m_ftrPowerDown);
	}
	
	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:		// ������ �� �����

			pMonster->AI_Path.DestNode = m_tEnemy.obj->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&m_tEnemy.obj->Position(), true, 0, 300);

			pMonster->Motion.m_tParams.SetParams(eMotionRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionRunTurnLeft,eMotionRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

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
			pMonster->Motion.m_tParams.SetParams(eMotionAttack,0,pMonster->m_ftrRunRSpeed,yaw,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			pMonster->Motion.m_tTurn.Set(eMotionFastTurnLeft, eMotionFastTurnLeft, 0, pMonster->m_ftrAttackFastRSpeed,pMonster->m_ftrRunAttackMinAngle);
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

void CBloodsuckerHearDNE::Reset()
{
	IState::Reset();

	Msg("[ RESET ]");
}

void CBloodsuckerHearDNE::Init()
{	
	IState::Init();	
	
	bool bTemp;
	pMonster->GetSound(m_tSound, bTemp);

	flag_once_1 = false;
	flag_once_2 = false;

	m_tAction = ACTION_LOOK_DESTINATION;

	Msg("[ INIT ]");
}

void CBloodsuckerHearDNE::Run()
{
	// ���� ����� ����, restart
	SoundElem	se;
	bool		bTemp;
	pMonster->GetSound(se, bTemp);
	if (m_tSound.time + 2000 < se.time) Init();
	
	switch (m_tAction) {
	case ACTION_LOOK_DESTINATION:
		DO_ONCE_BEGIN(flag_once_1);
			pMonster->AI_Path.TravelPath.clear();
			pMonster->LookPosition(m_tSound.position, PI_DIV_3);
		DO_ONCE_END();
		
		pMonster->Motion.m_tParams.SetParams(eMotionStandIdle, 0, 0, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
		pMonster->Motion.m_tTurn.Set(eMotionStandTurnLeft, eMotionStandTurnRight, 0, PI_DIV_3, PI_DIV_6/20);

		if (m_dwStateStartedTime + 3000 < m_dwCurrentTime) m_tAction = ACTION_GOTO_SOUND_SOURCE;
		Msg("[ ACTION_LOOK_DESTINATION ]  :: CurrentTime = [%i] CurYaw = [%f] TargetYaw = [%f]  SoundPos = [%f,%f,%f]", m_dwCurrentTime, pMonster->r_torso_current.yaw, pMonster->r_torso_target.yaw,VPUSH(m_tSound.position));

		break;
	case ACTION_GOTO_SOUND_SOURCE:
		DO_ONCE_BEGIN(flag_once_2);
			pMonster->vfInitSelector(pMonster->m_tSelectorHearSnd, true);
			pMonster->m_tSelectorHearSnd.m_tEnemyPosition		= m_tSound.position;
		DO_ONCE_END();
		
		pMonster->vfChoosePointAndBuildPath(&pMonster->m_tSelectorHearSnd,0, true, 0, 300);

		pMonster->Motion.m_tParams.SetParams(eMotionWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);		
		pMonster->Motion.m_tTurn.Set(eMotionWalkTurnLeft, eMotionWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
		Msg("[ ACTION_GOTO_SOUND_SOURCE ]");		
		break;
	}
}

void CBloodsuckerHearDNE::Restart()
{

}

//////////////////////////////////////////////////////////////////////////
// State Hear NDE Sound
//////////////////////////////////////////////////////////////////////////

CBloodsuckerHearNDE::CBloodsuckerHearNDE(CAI_Bloodsucker *p)
{
	pMonster = p;
	Reset();
	SetHighPriority();
}

void CBloodsuckerHearNDE::Reset()
{
	IState::Reset();
	m_tAction = ACTION_DO_NOTHING;
}

void CBloodsuckerHearNDE::Init()
{	
	IState::Init();	
	Msg("[ DO NOTHING ]");		
}

void CBloodsuckerHearNDE::Run()
{
	pMonster->Motion.m_tParams.SetParams(eMotionStandIdle, 0, 0, 0, 0, MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
	pMonster->Motion.m_tTurn.Clear();
}

