#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "..\\rat\\ai_rat.h"

#include "..\\..\\PhysicsShell.h"
#include "..\\..\\phcapture.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingEat::CBitingEat(CAI_Biting *p)  
{
	pMonster = p;
	Reset();
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

	// ѕолучить инфо о трупе
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	bEatRat = (dynamic_cast<CAI_Rat *>(pCorpse) ? true : false);
	m_fDistToCorpse = ((bEatRat)? 1.0f : 1.8f);
	
	if (bEatRat) Msg("Eat Rat");

	SavedPos			= pCorpse->Position();		// сохранить позицию трупа
	m_fDistToDrag		= 20.f;
	bDragging			= false;

	m_tAction			= ACTION_RUN;

	flag_once_1			= false;
	m_dwStandStart		= 0;

	// Test
	WRITE_TO_LOG("_ Eat Init _");
}

void CBitingEat::Run()
{
	// ≈сли новый труп, снова инициализировать состо€ние 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	if (pCorpse != ve.obj) Init();

	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // рассто€ние, на которое уже оттащен труп
	float cur_dist		= pCorpse->Position().distance_to(pMonster->Position());

	// ¬ыполнение состо€ни€
	switch (m_tAction) {
		case ACTION_RUN:	// бежать к трупу

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0);

			pMonster->MotionMan.m_tAction = ACT_RUN;
			
			if (cur_dist < m_fDistToCorpse) {
				
				// ≈сли труп крысы
				if (!bEatRat) {
					// пытатьс€ вз€ть труп
					pMonster->Movement.PHCaptureObject(pCorpse);

					if (pMonster->Movement.PHCapture()->Failed()) {
						// если не получилось вз€ть
						bDragging = false;
						m_tAction = ACTION_EAT;
					}else {
						// тащить труп
						bDragging = true;
						m_tAction = ACTION_DRAG;
					}

				} else {
					bDragging = false;
					m_tAction = ACTION_EAT;
				}

				// если монстр подбежал к трупу, необходимо отыграть проверку трупа
				pMonster->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
			}
			break;

		case ACTION_DRAG:
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), saved_dist, 500);

			// ”становить параметры движени€
			pMonster->MotionMan.m_tAction = ACT_WALK_BKWD; 
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
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 4.f, 5000);
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

			if (cur_dist > 3.f) m_tAction = ACTION_LOOK_AROUND;
			break;

		case ACTION_LOOK_AROUND:  // посто€ть 2 сек
			DO_ONCE_BEGIN(flag_once_1);
				m_dwStandStart = m_dwCurrentTime;
			DO_ONCE_END();
			
			if (m_dwStandStart + 2000 < m_dwCurrentTime) m_tAction = ACTION_WALK;

			// Look around
			pMonster->MotionMan.m_tAction = ACT_LOOK_AROUND; 

			break;
		case ACTION_WALK:

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0,2000);
	
			pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

			if (cur_dist  + 0.5f  < m_fDistToCorpse) {
				m_tAction = ACTION_EAT;
			}

			break;
		case ACTION_EAT:
			pMonster->MotionMan.m_tAction = ACT_EAT; 

			if (pMonster->GetSatiety() >= 1.0f) pMonster->flagEatNow = false;
			else pMonster->flagEatNow = true;

			// съесть часть
			DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
				pMonster->ChangeSatiety(0.05f);
				pCorpse->m_fFood -= pMonster->m_fHitPower/5.f;
			DO_IN_TIME_INTERVAL_END();

			break;
	}
}

void CBitingEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow = false;

	// если тащит труп - бросить
	if (bDragging) {
		pMonster->Movement.PHReleaseObject();
	}
}
