#include "stdafx.h"
#include "ai_flesh.h"

CAI_Flesh::CAI_Flesh()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
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

CAI_Flesh::~CAI_Flesh()
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


void CAI_Flesh::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
	m_fEyeShiftYaw					= PI_DIV_6;
}


void CAI_Flesh::Think()
{
	inherited::Think();

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
		//-
		
		CurrentState->Execute(m_dwCurrentUpdate);

		// ��������� �� �������������
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	Motion.SetFrameParams(this);
	ControlAnimation();		
}

void CAI_Flesh::UpdateCL()
{
	inherited::UpdateCL();


	// �������� ��������� �������� (�����)
	CheckAttackHit();
}


void CAI_Flesh::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;	 index3 = -1;	break;
		case eMotionLieIdle:		index1 = 2; index2 = 0;	 index3 = -1;	break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 1;	 index3 = -1;	break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;	 index3 = -1;	break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 3;  index3 = -1;	break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 4;  index3 = -1;	break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 5;  index3 = -1;	break;
		case eMotionRun:			index1 = 0; index2 = 6;  index3 = -1;	break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 7;  index3 = -1;	break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 8;  index3 = -1;	break;
		case eMotionAttack:			index1 = 0; index2 = 9;  index3 = -1;	break;
		case eMotionAttackRat:		index1 = 0; index2 = 10; index3 = -1;	break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 11; index3 = -1;	break;
		case eMotionEat:			index1 = 2; index2 = 12; index3 = -1;	break;
		case eMotionStandDamaged:	index1 = 0; index2 = 13; index3 = -1;	break;
		case eMotionScared:			index1 = 0; index2 = 14; index3 = -1;	break;
		case eMotionDie:			index1 = 0; index2 = 15; index3 = -1;	break;
		case eMotionLieDown:		index1 = 0; index2 = 16; index3 = -1;	break;
		case eMotionStandUp:		index1 = 2; index2 = 17; index3 = -1;	break;
		case eMotionCheckCorpse:	index1 = 0; index2 = 0;	 index3 = 2;	break;
		case eMotionLieDownEat:		index1 = 0; index2 = 18; index3 = -1;	break;
		case eMotionAttackJump:		index1 = 0; index2 = 19; index3 = -1;	break;
		//default:					NODEFAULT;
	}
}

void CAI_Flesh::LoadAttackAnim()
{
	Fvector center, left_side, right_side, special_side;

	center.set		(0.f,0.f,0.f);
	left_side.set	(-0.1f,0.f,0.f);
	right_side.set	(0.1f,0.f,0.f);
	special_side.set(-0.5f,0.f,0.5f);


	// 1 //
	m_tAttackAnim.PushAttackAnim(0, 9, 0, 700,	800,	center,		2.f, m_fHitPower);

	// 2 //
	m_tAttackAnim.PushAttackAnim(0, 9, 1, 600,	800,	center,		2.5f, m_fHitPower);

	// 3 // 
	m_tAttackAnim.PushAttackAnim(0, 9, 2, 1100,	1250,	right_side,	1.5f, m_fHitPower);

	// 4 // 
	m_tAttackAnim.PushAttackAnim(0, 9, 3, 1300,	1400,	left_side,	0.6f, m_fHitPower);

	// 5 // 
	m_tAttackAnim.PushAttackAnim(0, 9, 4, 600, 800,	special_side,	2.6f, m_fHitPower, AA_FLAG_ATTACK_RAT);

	// 6 //
	m_tAttackAnim.PushAttackAnim(0, 9, 5, 700, 850,		center,		2.6f, m_fHitPower, AA_FLAG_FIRE_ANYWAY);

}

void CAI_Flesh::CheckAttackHit()
{
	// �������� ��������� �������� (�����)
	TTime cur_time = Level().timeServer();
	SAttackAnimation apt_anim;

	bool bGoodTime = m_tAttackAnim.CheckTime(cur_time,apt_anim);

	if (bGoodTime) {
		VisionElem ve;
		if (!GetEnemy(ve)) return;
		CObject *obj = dynamic_cast<CObject *>(ve.obj);

		Fvector trace_from;
		Center(trace_from);
		trace_from.add(apt_anim.trace_offset);

		// ����������� �����?
		if ((apt_anim.flags & AA_FLAG_FIRE_ANYWAY) == AA_FLAG_FIRE_ANYWAY) {
			DoDamage(ve.obj, apt_anim.damage);	// �� �����
			m_tAttackAnim.UpdateLastAttack(cur_time);
		} else if ((apt_anim.flags & AA_FLAG_ATTACK_RAT) == AA_FLAG_ATTACK_RAT) {

			// TestIntersection ������(������) � �����(�����)
			bool Intersected = false;

			float angle = PI_DIV_6;					// ���� ������
			Fvector fromV = trace_from;				// ������� ������
			Fvector dir;							// ����������� ������
			dir.set(0.f,-1.f,0.f);

			float fInvSin = 1.0f/_sin(angle);
			float fCosSqr = _cos(angle)*_cos(angle);

			Fvector vC;		ve.obj->Center(vC);		// ����� �����
			Fvector kCmV;	kCmV.sub(vC,fromV);
			Fvector kD		= kCmV;
			Fvector tempV	= dir;
			tempV.mul(ve.obj->Radius()* fInvSin);
			kD.add(tempV);

			float fDSqrLen = kD.square_magnitude();
			float fE = kD.dotproduct(dir);
			if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr )
			{
				float fSinSqr = _sin(angle)*_sin(angle);

				fDSqrLen = kCmV.square_magnitude();
				fE = -kCmV.dotproduct(dir);
				if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr ) {
					float fRSqr = ve.obj->Radius()*ve.obj->Radius();
					Intersected =  fDSqrLen <= fRSqr;
				}else Intersected = true;
			} else Intersected = false;

			if (Intersected) {
				DoDamage(ve.obj,apt_anim.damage);
				m_tAttackAnim.UpdateLastAttack(cur_time);
			}

		} else 	{ // �����
			this->setEnabled(false);
			Collide::ray_query	l_rq;
			
			if (Level().ObjectSpace.RayPick(trace_from, Direction(), apt_anim.dist, l_rq)) {
				if ((l_rq.O == obj) && (l_rq.range < apt_anim.dist)) {
					DoDamage(ve.obj, apt_anim.damage);
					m_tAttackAnim.UpdateLastAttack(cur_time);
				}
			}
			this->setEnabled(true);			

			if (!ve.obj->g_Alive()) AddCorpse(ve);	
		}
	}
}


