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

	// �������� ���� � �����
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	bEatRat = (dynamic_cast<CAI_Rat *>(pCorpse) ? true : false);
	m_fDistToCorpse = ((bEatRat)? 1.0f : pMonster->_sd->m_fDistToCorpse); 

	SavedPos			= pCorpse->Position();		// ��������� ������� �����
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
	// ���� ����� ����, ����� ���������������� ��������� 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) {Done(); return;}
	if (pCorpse != ve.obj) Init();

	// ���������� ������� ��������� ���� � �����
	Fvector nearest_bone_pos = pMonster->Movement.PHCaptureGetNearestElemPos(pCorpse);
	float cur_dist = nearest_bone_pos.distance_to(pMonster->Position());

	// temp show out
	pMonster->dbg_info.set(nearest_bone_pos);

	if (bHideAfterLunch) m_tAction = ACTION_GET_HIDE;
	else if (bRestAfterLunch) m_tAction = ACTION_LITTLE_REST;
	else if (bEating && (cur_dist > m_fDistToCorpse)) m_tAction = ACTION_WALK;

	LOG_EX2("My pos[%f,%f,%f],c.pos[%f,%f,%f], dist[%f], r[%f]", *"*/ VPUSH(pMonster->Position()), VPUSH(nearest_bone_pos), cur_dist, m_fDistToCorpse  /*"*);
	
	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // ����������, �� ������� ��� ������� ����

	switch (m_tAction) {
	case ACTION_CORPSE_APPROACH_RUN:	// ������ � �����
		pMonster->set_level_dest_vertex (pCorpse->level_vertex_id());
		pMonster->vfChoosePointAndBuildPath(0,&nearest_bone_pos, true, 0);

		pMonster->MotionMan.m_tAction = ACT_RUN;

		if (cur_dist < DIST_SLOW_APPROACH_TO_CORPSE) m_tAction = ACTION_CORPSE_APPROACH_WALK;
		break;
	case ACTION_CORPSE_APPROACH_WALK:

		pMonster->set_level_dest_vertex	(pCorpse->level_vertex_id());
		pMonster->vfChoosePointAndBuildPath(0,&nearest_bone_pos, true, 0);

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		
		if (cur_dist < m_fDistToCorpse) {
			
			// ���� ������ �������� � �����, ���������� �������� �������� �����
			pMonster->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
			
			m_tAction = ( (bCanDrag) ?   ACTION_PREPARE_DRAG : ACTION_EAT );
			//m_tAction =  ACTION_EAT;
			m_dwPrepareDrag	= m_dwCurrentTime;
		}
		break;
	case ACTION_EAT:
		pMonster->MotionMan.m_tAction = ACT_EAT;
		
		bEating = true;
		if (pMonster->GetSatiety() >= 1.0f) bHideAfterLunch = true;

		// ������ �����
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
			pMonster->ChangeSatiety(0.02f);
			pCorpse->m_fFood -= pMonster->_sd->m_fHitPower/5.f;

			//Fmatrix M = pMonster->Movement.PHCaptureGetNearestElemTransform(pCorpse);
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
		pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 10.f, 300);
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

		if (cur_dist > 10.f) {
			m_tAction = ACTION_LITTLE_REST;
			bHideAfterLunch = false;
			bRestAfterLunch	= true;
			m_dwTimeStartRest = m_dwCurrentTime;
		}

		LOG_EX("EAT:: action_get_hide activated");

		break;
	case ACTION_LITTLE_REST:
		pMonster->MotionMan.m_tAction = ACT_REST; 
		if (m_dwTimeStartRest + REST_AFTER_LUNCH_TIME < m_dwCurrentTime) {
			pMonster->flagEatNow	= false;
			bRestAfterLunch			= false; 
		}
		
		LOG_EX("EAT:: action_little_rest activated");

		break;

	case ACTION_WALK:

		pMonster->set_level_dest_vertex	(pCorpse->level_vertex_id());
		pMonster->vfChoosePointAndBuildPath(0,&nearest_bone_pos, true, 0,500);

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD; 

		if (cur_dist  + 0.2f  < m_fDistToCorpse) m_tAction = ACTION_EAT;
		break;
	case ACTION_PREPARE_DRAG:
		if (m_dwPrepareDrag + 1000 < m_dwCurrentTime) {
			// ���� ���� ����� || ���� �� ���������� �����
			bDragging = false;
			m_tAction = ACTION_EAT;

			if (!bEatRat && bCanDrag) {	// ���� �� ���� ����� � ����� ������
				// �������� ����� ����
				pMonster->Movement.PHCaptureObject(pCorpse);

				if (!pMonster->Movement.PHCapture()->Failed()) {
					// ������ ����
					bDragging = true;
					m_tAction = ACTION_DRAG;
				}
			} 
		}

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		break;

	case ACTION_DRAG:
		pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), saved_dist, 500);

		// ���������� ��������� ��������
		pMonster->MotionMan.m_tAction = ACT_DRAG; 
		pMonster->MotionMan.SetSpecParams(ASP_DRAG_CORPSE | ASP_MOVE_BKWD);

		// ���� �� ����� ������
		if (0 == pMonster->Movement.PHCapture()) m_tAction = ACTION_EAT; 

		if (saved_dist > m_fDistToDrag) {
			// ������� ����
			pMonster->Movement.PHReleaseObject();

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

	// ���� ����� ���� - �������
	if (bDragging) {
		pMonster->m_PhysicMovementControl.PHReleaseObject();
	}
}
