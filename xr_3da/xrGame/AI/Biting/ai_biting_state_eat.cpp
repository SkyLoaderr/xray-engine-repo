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

	// �������� ���� � �����
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	pCorpse = ve.obj;

	CAI_Rat	*tpRat = dynamic_cast<CAI_Rat *>(pCorpse);
	m_fDistToCorpse = ((tpRat)? 1.0f : 2.5f);

	SavedPos			= pCorpse->Position();		// ��������� ������� �����
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
	// ���� ����� ����, ����� ���������������� ��������� 
	VisionElem ve;
	if (!pMonster->GetCorpse(ve)) R_ASSERT(false);
	if (pCorpse != ve.obj) Init();

	float saved_dist	= SavedPos.distance_to(pMonster->Position()); // ����������, �� ������� ��� ������� ����
	float cur_dist		= pCorpse->Position().distance_to(pMonster->Position());

	// ���������� ���������
	switch (m_tAction) {
		case ACTION_RUN:	// ������ � �����

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0,2000);

			pMonster->Motion.m_tParams.SetParams(eAnimRun,pMonster->m_ftrRunAttackSpeed,pMonster->m_ftrRunRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimRunTurnLeft,eAnimRunTurnRight, pMonster->m_ftrRunAttackTurnSpeed,pMonster->m_ftrRunAttackTurnRSpeed,pMonster->m_ftrRunAttackMinAngle);

			if (cur_dist < m_fDistToCorpse) {
				// �������� ����� ����
				pMonster->Movement.PHCaptureObject(pCorpse);

				if (pMonster->Movement.PHCapture()->Failed()) {
					// ���� �� ���������� �����
					bDragging = false;
					m_tAction = ACTION_EAT;
				}else {
					// ������ ����
					bDragging = true;
					m_tAction = ACTION_DRAG;
				}

				// ���� ������ �������� � �����, ���������� �������� �������� �����
				pMonster->Motion.m_tSeq.Add(eAnimCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
				pMonster->Motion.m_tSeq.Switch();
			}
			break;

		case ACTION_DRAG:
			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), saved_dist, 5000);

			// ���������� ��������� ��������
			pMonster->Motion.m_tParams.SetParams(eAnimWalkBkwd,pMonster->m_ftrWalkSpeed / 2, pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set		(eAnimWalkBkwd, eAnimWalkBkwd,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);			
			pMonster->Motion.m_tTurn.SetMoveBkwd(true);

			// ���� �� ����� ������
			if (pMonster->Movement.PHCapture() == 0) m_tAction = ACTION_WALK_LITTLE_AWAY;

			if (saved_dist > m_fDistToDrag) {
				// ������� ����
				pMonster->Movement.PHReleaseObject();

				bDragging = false; 
				m_tAction = ((::Random.randI(2)) ? ACTION_LOOK_AROUND : ACTION_EAT);
			}

			break;
		case ACTION_WALK_LITTLE_AWAY:

			pMonster->Path_GetAwayFromPoint(pCorpse, pCorpse->Position(), 4.f, 5000);

			
			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed / 2,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);

			if (cur_dist > 3.f) m_tAction = ACTION_LOOK_AROUND;
			break;

		case ACTION_LOOK_AROUND:  // �������� 2 ���
			DO_ONCE_BEGIN(flag_once_1);
				m_dwStandStart = m_dwCurrentTime;
			DO_ONCE_END();
			
			if (m_dwStandStart + 2000 < m_dwCurrentTime) m_tAction = ACTION_WALK;

			pMonster->Motion.m_tParams.SetParams(eAnimCheckCorpse,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();

			break;
		case ACTION_WALK:

			pMonster->AI_Path.DestNode = pCorpse->AI_NodeID;
			pMonster->vfChoosePointAndBuildPath(0,&pCorpse->Position(), true, 0,2000);

			pMonster->Motion.m_tParams.SetParams(eAnimWalkFwd,pMonster->m_ftrWalkSpeed,pMonster->m_ftrWalkRSpeed,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Set(eAnimWalkTurnLeft, eAnimWalkTurnRight,pMonster->m_ftrWalkTurningSpeed,pMonster->m_ftrWalkTurnRSpeed,pMonster->m_ftrWalkMinAngle);
			pMonster->Motion.m_tTurn.SetMoveBkwd(false);

			if (cur_dist  + 0.5f  < m_fDistToCorpse) {
				m_tAction = ACTION_EAT;

				// ���� � ����
				pMonster->Motion.m_tSeq.Add(eAnimStandLieDownEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED, STOP_ANIM_END);
				pMonster->Motion.m_tSeq.Switch();
			}

			break;
		case ACTION_EAT:
			pMonster->Motion.m_tParams.SetParams(eAnimEat,0,0,0,0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED);
			pMonster->Motion.m_tTurn.Clear();

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

void CBitingEat::Done()
{
	inherited::Done();

	pMonster->flagEatNow = false;

	pMonster->Motion.m_tTurn.SetMoveBkwd(false);
	// ���� ����� ���� - �������
	if (bDragging) {
		pMonster->Movement.PHReleaseObject();
	}
}
