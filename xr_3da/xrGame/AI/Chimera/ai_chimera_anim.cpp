#include "stdafx.h"
#include "ai_chimera.h"

void CAI_Chimera::vfAssignBones(CInifile *ini, const char *section)
{
	int spin_bone		= PKinematics(Visual())->LL_BoneID(ini->r_string(section,"bone_spin"));
	PKinematics(Visual())->LL_GetInstance(spin_bone).set_callback(SpinCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(spin_bone),AXIS_Z); 
}

void __stdcall CAI_Chimera::SpinCallback(CBoneInstance *B)
{
	CAI_Chimera* A = dynamic_cast<CAI_Chimera*> (static_cast<CObject*>(B->Callback_Param));

	A->SpinBoneInMotion(B);
	A->SpinBoneInAttack(B);
	
	A->Bones.Update(B, Level().timeServer());
}


void CAI_Chimera::SpinBoneInMotion(CBoneInstance *B)
{
//	// ��������� �������� ������ �� ����� ��������
//	if (!IsInMotion()) return;
//	
//	// ���������� �� ������� SY = A * sin((alfa-a)* Pi / (b-a))
//	// ��� A		- ���������, 
//	//     alfa		- ������� ����
//	//     a, b		- �������� ��������, �.�.  ��������� ���� � �������� ����
//
//	float Mcy	= r_torso_current.yaw;
//	float Mty	= r_torso_target.yaw;
//	float Amp	= PI_DIV_3;				
//
//	// ��������� ������ �������� (���� target.yaw ��������� ������ ��� �� 30 ����)
//	if (_abs(angle_normalize_signed(fPrevMty - Mty)) > PI_DIV_6) {
//		fStartYaw	= Mcy;
//		fFinishYaw	= Mty;
//	}
//
//	float SY;	// ������� ���� �������� ��� ����
//	if (getAI().bfTooSmallAngle(fFinishYaw,fStartYaw,EPS_L)) SY = 0.f;	// ���� ������� == 0, ���������� ������� �� 0 :)
//	else if (getAI().bfTooSmallAngle(Mty,Mcy,EPS_L)) SY = 0.f;				// ���� ������� �� ���������
//	else SY = Amp * _sin((_abs(angle_normalize_signed(Mcy-fStartYaw))) * PI / (_abs(angle_normalize_signed(fFinishYaw - fStartYaw)))) ;
//
//	// ������� ������?
//	if (angle_normalize_signed(Mty - Mcy) > 0) SY = -SY;
//
//	// SpinYaw ��������� � SY
//	u32 cur_time  = Level().timeServer();
//	u32 new_time  = timeLastSpin + 20; 
//	
//	float MinDelta = 3 * PI / 180;	
//	float delta	  = PI / 180;            
//
//	// ���� ������� ����� SY � SpinYaw ������ ���� MinDelta- ��������� SpinYaw �� delta
//	if (_abs(fSpinYaw - SY) > MinDelta) {
//		if (new_time < cur_time) {
//			if (fSpinYaw < SY) fSpinYaw += delta;
//			else fSpinYaw -= delta;
//			timeLastSpin = cur_time;
//		}
//	}
//
//	// ������� ������� �������� � �������� �� mTransform ����
//	Fmatrix M;
//	M.rotateZ(fSpinYaw);
//	B->mTransform.mulB(M);
//
//	// ��������� ������� �������� target.yaw
//	fPrevMty = Mty;
}

void CAI_Chimera::SpinBoneInAttack(CBoneInstance *B)
{

}


void CAI_Chimera::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	index1 = index2 = 0;		// bug protection ;) todo: find out the reason
	index3 = -1;
	
	switch(motion) {
		case eAnimStandIdle:		index1 = 0; index2 = 0;	 break;
		case eAnimLieIdle:			index1 = 2; index2 = 0;	 break;
		case eAnimStandTurnLeft:	index1 = 0; index2 = 3;	 break;
		case eAnimWalkFwd:			index1 = 0; index2 = 3;	 break;
		case eAnimWalkTurnLeft:		index1 = 0; index2 = 5;  break;
		case eAnimWalkTurnRight:	index1 = 0; index2 = 6;  break;
		case eAnimRun:				index1 = 0; index2 = 7;  break;
		case eAnimRunTurnLeft:		index1 = 0; index2 = 8;  break;
		case eAnimRunTurnRight:		index1 = 0; index2 = 9;  break;
		case eAnimAttack:			index1 = 0; index2 = 10; break;
		case eAnimFastTurn:			index1 = 0; index2 = 7;  break;
		case eAnimEat:				index1 = 2; index2 = 14; break;
		case eAnimStandDamaged:		index1 = 0; index2 = 15; break;
		case eAnimScared:			index1 = 0; index2 = 16; break;
		case eAnimDie:				index1 = 0; index2 = 0;	 break;
		case eAnimStandLieDown:		index1 = 0; index2 = 20; break;
		case eAnimLieStandUp:		index1 = 2; index2 = 21; break;
		case eAnimCheckCorpse:		index1 = 2; index2 = 0;	 index3 = 0;	break;
		case eAnimStandLieDownEat:	index1 = 0; index2 = 22; break;
		///default:					NODEFAULT;
	} 

	if (index3 == -1) index3 = ::Random.randI((int)m_tAnimations.A[index1].A[index2].A.size());
}


void CAI_Chimera::LoadAttackAnim()
{
	Fvector center, left_side, right_side;

	center.set		(0.f,0.f,0.f);
	left_side.set	(-0.3f,0.f,0.f);
	right_side.set	(0.3f,0.f,0.f);

	// 1 //
	m_tAttackAnim.PushAttackAnim(0, 10, 0, 700,	800,	center,		2.f, m_fHitPower, 0.f, 0.f);
	
	// 2 //
	m_tAttackAnim.PushAttackAnim(0, 10, 1, 500,	600,	right_side, 2.5f, m_fHitPower, 0.f, 0.f);
	
	// 3 // 
	m_tAttackAnim.PushAttackAnim(0, 10, 2, 600,	700,	center,		2.5f, m_fHitPower, 0.f, 0.f);

	// 4 // 
	m_tAttackAnim.PushAttackAnim(0, 10, 3, 800,	900,	left_side,	1.0f, m_fHitPower, 0.f, 0.f);

	// 5 // 
	m_tAttackAnim.PushAttackAnim(0, 10, 5, 1500, 1600,	right_side, 2.0f, m_fHitPower, 0.f, 0.f);
}

