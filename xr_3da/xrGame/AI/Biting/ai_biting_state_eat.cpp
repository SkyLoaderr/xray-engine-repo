#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "..\\rat\\ai_rat.h"

#include "..\\..\\PhysicsShell.h"
#include "..\\..\\phcapture.h"

#define  REST_AFTER_LUNCH_TIME 5000

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingEat::CBitingEat(CAI_Biting *p, bool pmt_can_drag)  
{
	pMonster	= p;
	bCanDrag	= pmt_can_drag;

	Reset		();
	SetLowPriority();
}

void CBitingEat::Reset()
{
	IState::Reset();

	pCorpse				= 0;

	m_dwLastTimeEat		= 0;
	m_dwEatInterval		= 1000;
}
void CBitingEat::Init()
{
	IState::Init();

	// Получить инфо о трупе
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	bEatRat = (dynamic_cast<CAI_Rat *>(pCorpse) ? true : false);
	m_fDistToCorpse = ((bEatRat)? 1.0f : pMonster->m_fDistToCorpse); 

	SavedPos			= pCorpse->Position();		// сохранить позицию трупа
	m_fDistToDrag		= 20.f;
	bDragging			= false;

	m_tAction			= ACTION_CORPSE_APPROACH_RUN;

	flag_once_1			= false;
	m_dwStandStart		= 0;

	m_dwPrepareDrag		= 0;
	bEating				= false;
	
	bRestAfterLunch		= false;
	bHideAfterLunch		= false;
	
	m_dwTimeStartRest	= 0;

	pMonster->flagEatNow = true;

	// Test
	WRITE_TO_LOG("_ Eat Init _");
}

void CBitingEat::Run()
{
	// Если новый труп, снова инициализировать состояние 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) {Done(); return;}
	if (pCorpse != ve.obj) Init();

	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // расстояние, на которое уже оттащен труп
	float cur_dist		= pCorpse->Position().distance_to(pMonster->Position());

	if (bEating && (cur_dist > m_fDistToCorpse)) m_tAction = ACTION_WALK;

	if (bHideAfterLunch) m_tAction = ACTION_GET_HIDE;
	else if (bRestAfterLunch) m_tAction = ACTION_LITTLE_REST;

	// Выполнение состояния
	switch (m_tAction) {
		case ACTION_CORPSE_APPROACH_RUN:	// бежать к трупу
			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

			pMonster->MotionMan.m_tAction = ACT_RUN;

			if (cur_dist < 6.f) m_tAction = ACTION_CORPSE_APPROACH_WALK;

			break;
		case ACTION_CORPSE_APPROACH_WALK:

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

			pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
			
			if (cur_dist < m_fDistToCorpse) {
				
				// если монстр подбежал к трупу, необходимо отыграть проверку трупа
				pMonster->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
				
				//if (!tasty_corpse) { pMonster->AddIgnoreObject(pCorpse); m_tAction = ACTION_LOOK_AROUND; } 
				
				m_tAction = ACTION_PREPARE_DRAG;
				m_dwPrepareDrag	= m_dwCurrentTime;
			}
			break;

		case ACTION_PREPARE_DRAG:
			if (m_dwPrepareDrag + 1000 < m_dwCurrentTime) {
				// Если труп крысы || если не получилось взять
				bDragging = false;
				m_tAction = ACTION_EAT;

				if (!bEatRat && bCanDrag) {	// Если не труп крысы и может тащить
					// пытаться взять труп
					pMonster->Movement.PHCaptureObject(pCorpse);

					if (!pMonster->Movement.PHCapture()->Failed()) {
						// тащить труп
						bDragging = true;
						m_tAction = ACTION_DRAG;
					}
				} 
			}
	
			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
			break;

		case ACTION_DRAG:
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), saved_dist, 500);

			// Установить параметры движения
			pMonster->MotionMan.m_tAction = ACT_DRAG; 
			pMonster->MotionMan.SetSpecParams(ASP_DRAG_CORPSE | ASP_MOVE_BKWD);

			// если не может тащить
			if (pMonster->Movement.PHCapture() == 0) m_tAction = ACTION_WALK_LITTLE_AWAY; 

			if (saved_dist > m_fDistToDrag) {
				// бросить труп
				pMonster->Movement.PHReleaseObject();

				bDragging = false; 
				m_tAction = ((::Random.randI(3)) ? ACTION_LOOK_AROUND : ACTION_EAT);
			}

			break;
		case ACTION_WALK_LITTLE_AWAY:
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 4.f, 500);
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

			if (cur_dist > 3.f) m_tAction = ACTION_LOOK_AROUND;
			break;

		case ACTION_LOOK_AROUND:  // постоять 2 сек
			DO_ONCE_BEGIN(flag_once_1);
				m_dwStandStart = m_dwCurrentTime;
			DO_ONCE_END();
			
			if (m_dwStandStart + 2000 < m_dwCurrentTime) {
				pMonster->AI_Path.TravelPath.clear();
				m_tAction = ACTION_WALK;
			}

			// Look around
			pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND; 

			break;
		case ACTION_WALK:

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0,500);
	
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

			if (cur_dist  + 0.2f  < m_fDistToCorpse) {
				m_tAction = ACTION_EAT;
			}

			break;
		case ACTION_EAT:
			pMonster->MotionMan.m_tAction = ACT_EAT; 
			bEating = true;
			if (pMonster->GetSatiety() >= 1.0f) bHideAfterLunch = true;

			// съесть часть
			DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
				pMonster->ChangeSatiety(0.05f);
				pCorpse->m_fFood -= pMonster->m_fHitPower/5.f;
			DO_IN_TIME_INTERVAL_END();

			break;
		case ACTION_GET_HIDE:
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 10.f, 300);
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

			if (cur_dist > 5.f) {
				m_tAction = ACTION_LITTLE_REST;
				bHideAfterLunch = false;
				bRestAfterLunch	= true;
				m_dwTimeStartRest = m_dwCurrentTime;
			}
			break;
		case ACTION_LITTLE_REST:
			pMonster->MotionMan.m_tAction = ACT_REST; 
			if (m_dwTimeStartRest + REST_AFTER_LUNCH_TIME < m_dwCurrentTime) {
				pMonster->flagEatNow	= false;
				bRestAfterLunch			= false; 
			}
			break;
	}
}

void CBitingEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow	= false;

	// если тащит труп - бросить
	if (bDragging) {
		pMonster->Movement.PHReleaseObject();
	}
}
