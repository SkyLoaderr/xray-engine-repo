#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../rat/ai_rat.h"

#include "../../PhysicsShell.h"
#include "../../phcapture.h"

#define		REST_AFTER_LUNCH_TIME			5000
#define		DIST_SLOW_APPROACH_TO_CORPSE	5.0f

//#define		EAT_WITHOUT_DRAG

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
	LOG_EX("eat init");
	IState::Init();

	// �������� ���� � �����
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = const_cast<CEntity*>(ve.obj);

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
	flag_once_1			= false;

	rebuild_path		= 0;

	Captured			= false;
	m_dwLastImpulse		= 0;
}

void CBitingEat::Run()
{
	// ���� ����� ����, ����� ���������������� ��������� 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve) || (!pCorpse->m_pPhysicsShell)) {Done(); return;}
	if (pCorpse != ve.obj) Init();

	// ���������� ������� ��������� ���� � �����
	Fvector nearest_bone_pos = pMonster->m_PhysicMovementControl.PHCaptureGetNearestElemPos(const_cast<CEntity*>(pCorpse));
	float cur_dist = nearest_bone_pos.distance_to(pMonster->Position());

	if (bHideAfterLunch) m_tAction = ACTION_GET_HIDE;
	else if (bRestAfterLunch) m_tAction = ACTION_LITTLE_REST;
	else if (bEating && (cur_dist > m_fDistToCorpse)) m_tAction = ACTION_WALK;

	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // ����������, �� ������� ��� ������� ����

	Fvector		approach_pos;
	u32			approach_vertex_id;
	
	bool bNeedRebuild = false; 

	switch (m_tAction) {
	case ACTION_CORPSE_APPROACH_RUN:	// ������ � �����
		pMonster->MotionMan.m_tAction = ACT_RUN;

		approach_vertex_id = ai().level_graph().check_position_in_direction(pCorpse->level_vertex_id(), pCorpse->Position(), nearest_bone_pos);
		if (approach_vertex_id == -1) {
			approach_vertex_id	= pCorpse->level_vertex_id();
			approach_pos		= ai().level_graph().vertex_position(approach_vertex_id);
		} else {
			approach_pos = nearest_bone_pos;
		}
		R_ASSERT(ai().level_graph().inside(approach_vertex_id,approach_pos));
	
		if (IS_NEED_REBUILD()) bNeedRebuild = true;
		if (pMonster->CMovementManager::level_dest_vertex_id() != approach_vertex_id) bNeedRebuild = true;
		
		if (bNeedRebuild) pMonster->MoveToTarget(approach_pos,approach_vertex_id);

		pMonster->HDebug->L_Add(approach_pos,D3DCOLOR_XRGB(255,0,128));

		if (cur_dist < DIST_SLOW_APPROACH_TO_CORPSE) m_tAction = ACTION_CORPSE_APPROACH_WALK;
		break;
	case ACTION_CORPSE_APPROACH_WALK:

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		pMonster->MoveToTarget(nearest_bone_pos,pCorpse->level_vertex_id());
		
		if (cur_dist < m_fDistToCorpse) {
			
			// ���� ������ �������� � �����, ���������� �������� �������� �����
			pMonster->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
			
#ifndef		EAT_WITHOUT_DRAG
			m_tAction = ( (bCanDrag) ?  ACTION_PREPARE_DRAG : ACTION_EAT );
#else	
			m_tAction = ACTION_EAT;
#endif
			m_dwPrepareDrag	= m_dwCurrentTime;

		}
		break;

	case ACTION_EAT:

		pMonster->MotionMan.m_tAction	= ACT_EAT;
		pMonster->enable_movement		(false);

		bEating = true;
		if (pMonster->GetSatiety() >= 0.9f) bHideAfterLunch = true;

		// ������ �����
		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastTimeEat, m_dwEatInterval);
			pMonster->ChangeSatiety(0.02f);
			pCorpse->m_fFood -= pMonster->_sd->m_fHitPower/5.f;
		DO_IN_TIME_INTERVAL_END();
	

//		DO_IN_TIME_INTERVAL_BEGIN(m_dwLastImpulse, 100);
//			Fmatrix global_transform;
//			Fvector target_pos;
//			
//			global_transform.set(pMonster->XFORM());
//			global_transform.mulB(pMonster->GetEatBone()->mTransform);
//			target_pos = global_transform.c;
//
//			if (target_pos.distance_to(nearest_bone_pos) < 0.1f) {
//				Fvector dir;
//				dir.sub(nearest_bone_pos, target_pos);
//
//				CPhysicsElement *ph_elem =  pCorpse->m_pPhysicsShell->NearestToPoint(pMonster->Position());
//				ph_elem->applyImpulse(dir,1000.f);
//			}
//		
//		DO_IN_TIME_INTERVAL_END();

		break;
	
	case ACTION_GET_HIDE:
		
		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;		


		if (IS_NEED_REBUILD()) {
			pMonster->SetPathParams(pMonster->level_vertex_id(),pMonster->Position()); 
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 30);
		}	

		if (cur_dist > 10.f || (IS_NEED_REBUILD() && (cur_dist > 3.f))) {
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

		pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
		pMonster->MoveToTarget(nearest_bone_pos,pCorpse->level_vertex_id());

		if (cur_dist < m_fDistToCorpse) {
			m_tAction = ACTION_EAT;
		}
		break;
	case ACTION_PREPARE_DRAG:

		if (m_dwPrepareDrag + 1000 < m_dwCurrentTime) {
			// ���� ���� ����� || ���� �� ���������� �����
			bDragging = false;
			m_tAction = ACTION_EAT;

			if (!bEatRat && bCanDrag) {	// ���� �� ���� ����� � ����� ������
				// �������� ����� ����
				pMonster->m_PhysicMovementControl.PHCaptureObject(pCorpse);

				if (!pMonster->m_PhysicMovementControl.PHCapture()->Failed()) {
					// ������ ����
					bDragging		= true;
					m_tAction		= ACTION_DRAG;
				} else {
					pMonster->HDebug->M_Add(0,"CAPTURE FAILED",D3DCOLOR_XRGB(255,0,128));
					LOG_EX("CAPTURE FAILED");
				}
			} 
		}

		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
		break;

	case ACTION_DRAG:

		// ���������� ��������� ��������
		pMonster->MotionMan.m_tAction = ACT_DRAG; 
		pMonster->MotionMan.SetSpecParams(ASP_MOVE_BKWD);
		
		if (IS_NEED_REBUILD()) {
			pMonster->SetPathParams(pMonster->level_vertex_id(),pMonster->Position()); 
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 20);
		}	

		// ���� �� ����� ������
		if (0 == pMonster->m_PhysicMovementControl.PHCapture()) m_tAction = ACTION_EAT; 

		if ((saved_dist > m_fDistToDrag)) {
			// ������� ����
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

	// ���� ����� ���� - �������
	if (bDragging) {
		pMonster->m_PhysicMovementControl.PHReleaseObject();
	}
}


