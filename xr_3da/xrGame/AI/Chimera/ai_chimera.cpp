#include "stdafx.h"
#include "ai_chimera.h"

CAI_Chimera::CAI_Chimera()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CChimeraAttack>	(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this);
	stateExploreDE		= xr_new<CBitingExploreDE>	(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	CurrentState		= stateRest;

	Init();
}

CAI_Chimera::~CAI_Chimera()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreDE);
	xr_delete(stateExploreNDE);
}


void CAI_Chimera::Init()
{
	inherited::Init();

	CurrentState			= stateRest;
	CurrentState->Reset		();

	fSpinYaw				= 0.f;
	timeLastSpin			= 0;
	fStartYaw				= 0.f;
	fFinishYaw				= 1.f;
	fPrevMty				= 0.f;

}


void CAI_Chimera::Think()
{
	inherited::Think();

	if (flagEnemyGoOffline) {
		CurrentState->Reset();
		SetState(stateRest);
	}
	
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

	if (Motion.m_tSeq.isActive())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		//- FSM 1-level 

		//if (flagEnemyLostSight && H && (E || F) && !A) SetState(stateFindEnemy);	// ����� �����
		if (C && H && I)		SetState(statePanic);
		else if (C && H && !I)		SetState(statePanic);
		else if (C && !H && I)		SetState(statePanic);
		else if (C && !H && !I) 	SetState(statePanic);
		else if (D && H && I)		SetState(stateAttack);
		else if (D && H && !I)		SetState(stateAttack);  //���� ����������� � ������ ������
		else if (D && !H && I)		SetState(statePanic);
		else if (D && !H && !I) 	SetState(stateHide);	// ����� ����������� ����� �������
		else if (E && H && I)		SetState(stateAttack); 
		else if (E && H && !I)  	SetState(stateAttack);  //���� ����������� � ������ ������
		else if (E && !H && I) 		SetState(stateDetour); 
		else if (E && !H && !I)		SetState(stateDetour); 
		else if (F && H && I) 		SetState(stateAttack); 		
		else if (F && H && !I)  	SetState(stateAttack); 
		else if (F && !H && I)  	SetState(stateDetour); 
		else if (F && !H && !I) 	SetState(stateHide);
		else if (A && !K && !H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);  // ����� ������� ����, �� �� ����, ���� �� ��������		(ExploreDNE)
		else if (A && !K && H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// ����� ������� ����, �� �� ����, ���� ��������			(ExploreDE)		
		else if (B && !K && !H)		SetState(stateExploreNDE);	// ����� �� ������� ����, �� �� ����, ���� �� ��������	(ExploreNDNE)
		else if (B && !K && H)		SetState(stateExploreNDE);	// ����� �� ������� ����, �� �� ����, ���� ��������		(ExploreNDE)
		else if (GetCorpse(ve) && ve.obj->m_fFood > 1)	
			SetState(stateEat);
		else						SetState(stateRest); 

		//---

		CurrentState->Execute(m_dwCurrentUpdate);

		// ��������� �� �������������
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	Motion.SetFrameParams(this);

	ControlAnimation();
}

void CAI_Chimera::UpdateCL()
{
	inherited::UpdateCL();

	// �������� ��������� �������� (�����)
	CheckAttackHit();
}


BOOL CAI_Chimera::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones(pSettings,cNameSect());

	return TRUE;
}
