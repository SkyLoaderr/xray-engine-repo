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

	Bones.Reset();
}


void CAI_Chimera::Think()
{
	inherited::Think();

	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}

	
	if (!Bones.IsActive()) {

		if (!getAI().bfTooSmallAngle(r_torso_current.yaw,r_torso_target.yaw, PI_DIV_6)) {
			float k;	
			if (angle_normalize_signed(r_torso_target.yaw - r_torso_current.yaw) > 0) k = -1.0;
			else k = 1.0f;

			int spin_bone	= PKinematics(Visual())->LL_BoneID("bip01_spine1");
			Bones.SetMotion(&PKinematics(Visual())->LL_GetInstance(spin_bone), AXIS_Z, k * PI_DIV_6 , PI, 1);	
		}
			
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
	if (IsAnimLocked(m_dwCurrentUpdate)) { 
		m_fCurSpeed		= 0.f;
		r_torso_speed	= 0.f;
	}

	ControlAnimation();
}

void CAI_Chimera::UpdateCL()
{
	inherited::UpdateCL();

}


BOOL CAI_Chimera::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones(pSettings,cNameSect());

	return TRUE;
}

void CAI_Chimera::Load (LPCSTR section)
{
	inherited::Load (section);

	LockAnim		(eAnimAttack, 0, 1, 1300);
	LockAnim		(eAnimAttack, 1, 1, 800);
	LockAnim		(eAnimAttack, 2, 1, 1000);
}