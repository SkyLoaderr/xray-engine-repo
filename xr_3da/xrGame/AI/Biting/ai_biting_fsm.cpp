////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\rat\\ai_rat.h"
#include "..\\..\\actor.h"

using namespace AI_Biting;
#define SILENCE


#undef	WRITE_TO_LOG
#ifdef SILENCE
#define WRITE_TO_LOG(s) ;

#else
#define WRITE_TO_LOG(s) {\
	Msg("Monster %s : \n* State : %s",cName(),s);\
}
#endif

void CAI_Biting::Think()
{
	if (!g_Alive()) return;


	m_dwLastUpdateTime		= m_dwCurrentUpdate;
	m_dwCurrentUpdate		= Level().timeServer();

	vfUpdateParameters		();

#ifndef SILENCE
	if (g_Alive())
		Msg("%s : [A=%d][B=%d][C=%d][D=%d][E=%d][F=%d][H=%d][I=%d][J=%d][K=%d]",cName(),A,B,C,D,E,F,H,I,J,K);
#endif

	// A - � ����� ������� ����
	// B - � ����� ��������� ����
	// � - � ���� ����� �������� �����
	// D - � ���� �������� �����
	// E - � ���� ������� �����
	// F - � ���� ������� �����
	// H - ���� ��������
	// I - ���� ����� ����
	// J - A | B
	// K - C | D | E | F 

	VisionElem ve;

	if (Motion.m_tSeq.Active())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		//- FSM 1-level 

		if (C && H && I)			SetState(stateHide);
		else if (C && H && !I)		SetState(stateHide);
		else if (C && !H && I)		SetState(stateHide);
		else if (C && !H && !I) 	SetState(stateHide);
		else if (D && H && I)		SetState(stateHide);
		else if (D && H && !I)		SetState(stateAttack);  //���� ����������� � ������ ������
		else if (D && !H && I)		SetState(stateHide);
		else if (D && !H && !I) 	SetState(stateHide);	// ����� ����������� ����� �������
		else if (E && H && I)		SetState(stateAttack); 
		else if (E && H && !I)  	SetState(stateAttack);  //���� ����������� � ������ ������
		else if (E && !H && I) 		SetState(statePanic); 
		else if (E && !H && !I)		SetState(statePanic); 
		else if (F && H && I) 		SetState(stateAttack); 		
		else if (F && H && !I)  	SetState(stateAttack); 
		else if (F && !H && I)  	SetState(statePanic); 
		else if (F && !H && !I) 	SetState(statePanic);	
//		else if (A && !K && !H)		SetState(stateDetour); // ����� ������� ����, �� �� ����, ���� �� ��������		(ExploreDNE)
//		else if (A && !K && H)		SetState(stateDetour); // ����� ������� ����, �� �� ����, ���� ��������			(ExploreDE)
//		else if (B && !K && !H)		SetState(stateDetour); // ����� �� ������� ����, �� �� ����, ���� �� ��������	(ExploreNDNE)
//		else if (B && !K && H)		SetState(stateDetour); // ����� �� ������� ����, �� �� ����, ���� ��������		(ExploreNDE)
		else if (GetCorpse(ve) && ve.obj->m_fFood > 1)	
									SetState(stateEat);
		else						SetState(stateRest); 
		//---
		
		CurrentState->Execute(m_dwCurrentUpdate);
		
		// ��������� �� �������������
		//if (CurrentState->CheckCompletion()) {SetState(stateRest, true);
	}
	
	Motion.SetFrameParams(this);


	ControlAnimation();
}