#include "stdafx.h"
#include "ai_chimera.h"

void CAI_Chimera::vfAssignBones(CInifile *ini, const char *section)
{
	int spin_bone		= PKinematics(Visual())->LL_BoneID(ini->r_string(section,"bone_spin"));
	PKinematics(Visual())->LL_GetInstance(u16(spin_bone)).set_callback(SpinCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(&PKinematics(Visual())->LL_GetInstance(u16(spin_bone)),AXIS_Z); 
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
