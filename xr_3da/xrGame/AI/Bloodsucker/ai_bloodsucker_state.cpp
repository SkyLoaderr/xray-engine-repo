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

	if ((day_time >= 10) && (day_time <= 18)) {  // день?

		bool bSatiety = pMonster->GetSatiety() > 0.6f;
		Msg("Satiety = [%f]",pMonster->GetSatiety());

		if (bSatiety) {		// сидит на полу


			WRITE_TO_LOG("ACTION_SIT");


			m_tAction = ACTION_SIT;
			dwMinRand = 3000;
			dwMaxRand = 5000;
		} else {			// бродит, ищет еду
			WRITE_TO_LOG("ACTION_WALK");

			m_tAction = ACTION_WALK;

			// ѕостроить путь обхода точек графа
			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = 3000;
			dwMaxRand = 5000;
		}
	} else { 
		//bool bSerenity = pMonster->GetSerenity() > 0.8f; 
		bool bSerenity = true;
		if (bSerenity) { // спокоен
			// спать
			WRITE_TO_LOG("ACTION_SLEEP");

			m_tAction = ACTION_SLEEP;
			dwMinRand = 3000;
			dwMaxRand = 5000;

		} else {
			// бродить (настороженно), часто осматриватьс€ по сторонам
			WRITE_TO_LOG("ACTION_WALK_CIRCUMSPECTION");

			m_tAction = ACTION_WALK_CIRCUMSPECTION;

			pMonster->AI_Path.TravelPath.clear();
			pMonster->vfUpdateDetourPoint();	
			pMonster->AI_Path.DestNode	= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;

			dwMinRand = 3000;
			dwMaxRand = 5000;
		}
	}

	m_dwReplanTime = ::Random.randI(dwMinRand,dwMaxRand);

}


void CBloodsuckerRest::Run()
{
	// проверить нужно ли провести перепланировку
	if (m_dwCurrentTime > m_dwLastPlanTime + m_dwReplanTime) Replanning();
	
	pMonster->Motion.m_tTurn.Clear();

	// ¬ыполнение состо€ни€
	switch (m_tAction) {	
		case ACTION_SIT: 
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			break;
		case ACTION_WALK: // обход точек графа
			pMonster->vfChoosePointAndBuildPath(0,0, false, 0,2000);
			pMonster->Motion.m_tParams.SetParams(eMotionStandIdle,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eMotionStandIdle, eMotionStandIdle,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
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


//			if (!pMonster->Bones.isActive()) {
//				//Look leftside/rightside
//				
//				bonesMotion		cur_motion;
//
//				// смотреть влево
//				// будут вращатьс€ 2 боны
//				//cur_motion.AddBone(0,AXIS_X, -PI_DIV_4, PI_DIV_2, 0, 0, 0, 0, 2000);
//				cur_motion.AddBone(0,AXIS_X, -PI_DIV_2, PI_DIV_2, 0, 0, 0, 0, 1000);
//
//				pMonster->Bones.AddMotion(cur_motion);
//				cur_motion.Clear();
//
//				// смотреть вправо
//				// будут вращатьс€ 2 боны
//				//cur_motion.AddBone(0,AXIS_X, PI_DIV_4, PI_DIV_2, 0, 0, 0, 0, 2000);
//				cur_motion.AddBone(0,AXIS_X, PI, PI, 0, 0, 0, 0, 1000);
//
//				pMonster->Bones.AddMotion(cur_motion);
//			}
//
////			float newYaw;
////			newYaw = pMonster->r_torso_current.yaw;
////
////			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw + PI_DIV_4),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_ANIM_END);
////			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw - PI_DIV_2),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_AT_TURNED);
////			pMonster->Motion.m_tSeq.Switch();
////			
////			WRITE_TO_LOG("_ Switch _");


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

	// ѕолучить инфо о трупе
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	bTastyCorpse = ((tpRat) ? false : true); 

	m_fDistToCorpse		= 0.8f;
	m_fWalkDistToCorpse = 5.f;

	// если очень голоден - бежать у трупу
	if (pMonster->GetSatiety() < 0.5f) m_tAction = ACTION_CORPSE_APPROACH_RUN;
	else m_tAction = ACTION_CORPSE_APPROACH_WALK;

	// Test
	WRITE_TO_LOG("_ Eat Init _");
}

void CBloodsuckerEat::Run()
{
	// ≈сли новый труп, снова инициализировать состо€ние 
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
		// подошли вплотную к трупу
		if (!bTastyCorpse) {
			pMonster->AddIgnoreObject(pCorpse);
			// играть анимацию (плохой труп)
			// ...
			pMonster->Motion.m_tSeq.Add(eMotionWalkBkwd,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
			pMonster->Motion.m_tSeq.Switch();

			
		} else {
			// играть анимацию сесть
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
		
			// съесть часть
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
