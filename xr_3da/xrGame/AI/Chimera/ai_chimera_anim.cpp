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
//	// выполнять вращение только во время движения
//	if (!IsInMotion()) return;
//	
//	// реализация по формуле SY = A * sin((alfa-a)* Pi / (b-a))
//	// где A		- амплитуда, 
//	//     alfa		- текущий угол
//	//     a, b		- диапазон вращения, т.е.  стартовый угол и конечный угол
//
//	float Mcy	= r_torso_current.yaw;
//	float Mty	= r_torso_target.yaw;
//	float Amp	= PI_DIV_3;				
//
//	// проверить начало поворота (если target.yaw изменился больше чем на 30 град)
//	if (_abs(angle_normalize_signed(fPrevMty - Mty)) > PI_DIV_6) {
//		fStartYaw	= Mcy;
//		fFinishYaw	= Mty;
//	}
//
//	float SY;	// текущий угол поворота для боны
//	if (getAI().bfTooSmallAngle(fFinishYaw,fStartYaw,EPS_L)) SY = 0.f;	// если разница == 0, пропустить деление на 0 :)
//	else if (getAI().bfTooSmallAngle(Mty,Mcy,EPS_L)) SY = 0.f;				// если поворот не требуется
//	else SY = Amp * _sin((_abs(angle_normalize_signed(Mcy-fStartYaw))) * PI / (_abs(angle_normalize_signed(fFinishYaw - fStartYaw)))) ;
//
//	// поворот вправо?
//	if (angle_normalize_signed(Mty - Mcy) > 0) SY = -SY;
//
//	// SpinYaw стремится к SY
//	u32 cur_time  = Level().timeServer();
//	u32 new_time  = timeLastSpin + 20; 
//	
//	float MinDelta = 3 * PI / 180;	
//	float delta	  = PI / 180;            
//
//	// если разница между SY и SpinYaw больше угла MinDelta- нарастить SpinYaw на delta
//	if (_abs(fSpinYaw - SY) > MinDelta) {
//		if (new_time < cur_time) {
//			if (fSpinYaw < SY) fSpinYaw += delta;
//			else fSpinYaw -= delta;
//			timeLastSpin = cur_time;
//		}
//	}
//
//	// создать матрицу вращения и умножить на mTransform боны
//	Fmatrix M;
//	M.rotateZ(fSpinYaw);
//	B->mTransform.mulB(M);
//
//	// Сохранить текущее значение target.yaw
//	fPrevMty = Mty;
}

void CAI_Chimera::SpinBoneInAttack(CBoneInstance *B)
{

}
