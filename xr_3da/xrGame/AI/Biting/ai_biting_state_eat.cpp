#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../rat/ai_rat.h"

#include "../../PhysicsShell.h"
#include "../../phcapture.h"

#define		REST_AFTER_LUNCH_TIME			5000
#define		DIST_SLOW_APPROACH_TO_CORPSE	5.0f

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingEat::CBitingEat(CAI_Biting *p, bool pmt_can_drag)  
{
	pMonster	= p;
	bCanDrag	= pmt_can_drag;

	SetPriority(PRIORITY_LOW);
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
	m_fDistToCorpse = ((bEatRat)? 1.0f : pMonster->_sd->m_fDistToCorpse); 

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
	LOG_EX("_ Eat Init _");
}


void CBitingEat::Run()
{
	// Если новый труп, снова инициализировать состояние 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) {Done(); return;}
	if (pCorpse != ve.obj) Init();

	// Определить позицию ближайшей боны у трупа
	Fvector nearest_bone_pos = pMonster->m_PhysicMovementControl.PHCaptureGetNearestElemPos(pCorpse);
	float cur_dist = nearest_bone_pos.distance_to(pMonster->Position());

	// temp show out
	pMonster->dbg_info.set(nearest_bone_pos);

	if (bHideAfterLunch) m_tAction = ACTION_GET_HIDE;
	else if (bRestAfterLunch) m_tAction = ACTION_LITTLE_REST;
	else if (bEating && (cur_dist > m_fDistToCorpse)) m_tAction = ACTION_WALK;

	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // расстояние, на которое уже оттащен труп

	u32 node_id;

	switch (m_tAction) {
	case ACTION_CORPSE_APPROACH_RUN:	// бежать к трупу
		
		node_id = ai().level_graph().check_position_in_direction(pCorpse->level_vertex_id(), pCorpse->Position(), nearest_bone_pos);
		if (node_id == -1)pMonster->set_level_dest_vertex (pCorpse->level_vertex_id());
		else pMonster->set_level_dest_vertex (node_id);
		
		pMonster->set_dest_position(nearest_bone_pos);
		pMonster->set_path_type (CMovementManager::ePathTypeLevelPath);
		
		pMonster->MotionMan.m_tAction = ACT_RUN;

		if (cur_dist < DIST_SLOW_APPROACH_TO_CORPSE) m_tAction = ACTION_CORPSE_APPROACH_WALK;
		break;
	case ACTION_CORPSE_APPROACH_WALK:

		pMonster->set_level_dest_vertex (pCorpse->level_vertex_id());
		pMonster->set_dest_position(nearest_bone_pos);
		pMonster->set_path_type (CMovementManager::ePathTypeLevelPath);

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		
		if (cur_dist < m_fDistToCorpse) {
			
			// если монстр подбежал к трупу, необходимо отыграть проверку трупа
			pMonster->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
			
			m_tAction = ( (bCanDrag) ?   ACTION_PREPARE_DRAG : ACTION_EAT );
			//m_tAction =  ACTION_EAT;
			m_dwPrepareDrag	= m_dwCurrentTime;
		}
		break;
	case ACTION_EAT:
		pMonster->MotionMan.m_tAction = ACT_EAT;
		pMonster->enable_movement	(false);

		bEating = true;
		if (pMonster->GetSatiety() >= 1.0f) bHideAfterLunch = true;

		// съесть часть
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
			pMonster->ChangeSatiety(0.02f);
			pCorpse->m_fFood -= pMonster->_sd->m_fHitPower/5.f;

			//Fmatrix M = pMonster->m_PhysicMovementControl.PHCaptureGetNearestElemTransform(pCorpse);
//			Fvector fv;
//			fv.set(0.f,0.f,0.f);
//			Fvector dir;
//
//			dir = pCorpse->Position();
//			dir.sub(pMonster->Position());
//
//			pCorpse->m_pPhysicsShell->applyImpulseTrace(fv,dir,1000.f,0);
		DO_IN_TIME_INTERVAL_END();
		break;
	
	case ACTION_GET_HIDE:
		pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 10.f);
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

		if (cur_dist > 10.f) {
			m_tAction = ACTION_LITTLE_REST;
			bHideAfterLunch = false;
			bRestAfterLunch	= true;
			m_dwTimeStartRest = m_dwCurrentTime;
		}
		break;
	case ACTION_LITTLE_REST:
		pMonster->enable_movement	(false);

		pMonster->MotionMan.m_tAction = ACT_REST; 
		if (m_dwTimeStartRest + REST_AFTER_LUNCH_TIME < m_dwCurrentTime) {
			pMonster->flagEatNow	= false;
			bRestAfterLunch			= false; 
			pMonster->AddIgnoreObject(pCorpse);
		}
		break;

	case ACTION_WALK:

		pMonster->set_level_dest_vertex (pCorpse->level_vertex_id());
		pMonster->set_dest_position(nearest_bone_pos);
		pMonster->set_path_type (CMovementManager::ePathTypeLevelPath);

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

		if (cur_dist  + 0.2f  < m_fDistToCorpse) m_tAction = ACTION_EAT;
		break;
	case ACTION_PREPARE_DRAG:
		if (m_dwPrepareDrag + 1000 < m_dwCurrentTime) {
			// Если труп крысы || если не получилось взять
			bDragging = false;
			m_tAction = ACTION_EAT;

			if (!bEatRat && bCanDrag) {	// Если не труп крысы и может тащить
				// пытаться взять труп
				pMonster->m_PhysicMovementControl.PHCaptureObject(pCorpse);

				if (!pMonster->m_PhysicMovementControl.PHCapture()->Failed()) {
					// тащить труп
					bDragging = true;
					m_tAction = ACTION_DRAG;
				}
			} 
		}

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		break;

	case ACTION_DRAG:
		
		pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), saved_dist);

		// Установить параметры движения
		pMonster->MotionMan.m_tAction = ACT_DRAG; 
		pMonster->MotionMan.SetSpecParams(ASP_DRAG_CORPSE | ASP_MOVE_BKWD);

		// если не может тащить
		if (0 == pMonster->m_PhysicMovementControl.PHCapture()) m_tAction = ACTION_EAT; 

		if (saved_dist > m_fDistToDrag) {
			// бросить труп
			pMonster->m_PhysicMovementControl.PHReleaseObject();

			bDragging = false; 
			m_tAction = ACTION_EAT;
		}
		break;
	}
	
	pMonster->SetSound(SND_TYPE_EAT, pMonster->_sd->m_dwEatSndDelay);
}

void CBitingEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow	= false;

	// если тащит труп - бросить
	if (bDragging) {
		pMonster->m_PhysicMovementControl.PHReleaseObject();
	}
}
