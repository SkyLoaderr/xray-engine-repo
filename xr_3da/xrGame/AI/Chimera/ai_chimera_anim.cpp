#include "stdafx.h"
#include "ai_chimera.h"

//
//void CAI_Chimera::SelectAnimation(const Fvector &_view, const Fvector &_move, float speed )
//{
//	if (bShowDeath)	{
//		m_tpCurrentGlobalAnimation = m_tAnimations.A[0].A[0].A[0];
//		PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//		bShowDeath  = false;
//		return;
//	}
//	
//	if (g_Alive())
//		if (!m_tpCurrentGlobalAnimation) {
//
//			int i1, i2, i3;
//			i1 = i2 = i3 = 0;
//			MotionToAnim(m_tAnim,i1,i2,i3);
//			if (i3 == -1) {
//				i3 = ::Random.randI((int)m_tAnimations.A[i1].A[i2].A.size());
//			}
//		
//			m_tpCurrentGlobalAnimation = m_tAnimations.A[i1].A[i2].A[i3];
//			PKinematics(Visual())->PlayCycle(m_tpCurrentGlobalAnimation,TRUE,vfPlayCallBack,this);
//
//			if (i2 == 9) FillAttackStructure(i3, m_dwCurrentUpdate);
////			else if (i2 == 10) FillAttackStructure(4, m_dwCurrentUpdate); // атака крыс
////			else if (i2 == 19) FillAttackStructure(5, m_dwCurrentUpdate); // атака крыс|прыжок
//			else m_tAttack.time_started = 0;
//		}
//}
//

void CAI_Chimera::vfAssignBones(CInifile *ini, const char *section)
{
	int spin_bone		= PKinematics(Visual())->LL_BoneID(ini->r_string(section,"bone_spin"));
	PKinematics(Visual())->LL_GetInstance(spin_bone).set_callback(SpinCallback,this);
}

void __stdcall CAI_Chimera::SpinCallback(CBoneInstance *B)
{
	CAI_Chimera* A = dynamic_cast<CAI_Chimera*> (static_cast<CObject*>(B->Callback_Param));

	A->SpinBone(B);
}


void CAI_Chimera::SpinBone(CBoneInstance *B)
{
	// реализация по формуле SY = A * sin((alfa-a)* Pi / (b-a))
	// где A		- амплитуда, 
	//     alfa		- текущий угол
	//     a, b		- диапазон вращения, т.е.  стартовый угол и конечный угол

	float Mcy	= r_torso_current.yaw;
	float Mty	= r_torso_target.yaw;
	float Amp	= PI_DIV_3;				

	// проверить начало поворота (если target.yaw изменился больше чем на 30 град)
	if (_abs(angle_normalize_signed(fPrevMty - Mty)) > PI_DIV_6) {
		fStartYaw	= Mcy;
		fFinishYaw	= Mty;
	}

	float SY;	// текущий угол поворота для боны
	if (getAI().bfTooSmallAngle(fFinishYaw,fStartYaw,EPS_L)) SY = 0.f;	// если разница == 0, пропустить деление на 0 :)
	else SY = Amp * _sin((_abs(angle_normalize_signed(Mcy-fStartYaw))) * PI / (_abs(angle_normalize_signed(fFinishYaw - fStartYaw)))) ;

	// поворот вправо?
	if (angle_normalize_signed(Mty - Mcy) > 0) SY = -SY;

	// SpinYaw стремится к SY
	u32 cur_time  = Level().timeServer();
	u32 new_time  = timeLastSpin + 20; 
	
	float MinDelta = 3 * PI / 180;	
	float delta	  = PI / 180;            

	// если разница между SY и SpinYaw больше угла MinDelta- нарастить SpinYaw на delta
	if (_abs(fSpinYaw - SY) > MinDelta) {
		if (new_time < cur_time) {
			if (fSpinYaw < SY) fSpinYaw += delta;
			else fSpinYaw -= delta;
			timeLastSpin = cur_time;
		}
	}

	// создать матрицу вращения и умножить на mTransform боны
	Fmatrix M;
	M.rotateZ(fSpinYaw);
	B->mTransform.mulB(M);

	// Сохранить текущее значение target.yaw
	fPrevMty = Mty;
}


void CAI_Chimera::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;	 index3 = -1;	break;
		case eMotionLieIdle:		index1 = 2; index2 = 0;	 index3 = -1;	break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 4;	 index3 = -1;	break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;	 index3 = -1;	break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 2;  index3 = -1;	break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 4;  index3 = -1;	break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 5;  index3 = -1;	break;
		case eMotionRun:			index1 = 0; index2 = 6;  index3 = -1;	break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 7;  index3 = -1;	break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 8;  index3 = -1;	break;
		case eMotionAttack:			index1 = 0; index2 = 9;  index3 = -1;	break;
		case eMotionAttackRat:		index1 = 0; index2 = 9;	 index3 = -1;	break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 8;  index3 = -1;	break;
		case eMotionEat:			index1 = 2; index2 = 12; index3 = -1;	break;
		case eMotionStandDamaged:	index1 = 0; index2 = 0;  index3 = -1;	break;
		case eMotionScared:			index1 = 0; index2 = 0;  index3 = -1;	break;
		case eMotionDie:			index1 = 0; index2 = 0;  index3 = -1;	break;
		case eMotionLieDown:		index1 = 0; index2 = 16; index3 = -1;	break;
		case eMotionStandUp:		index1 = 2; index2 = 17; index3 = -1;	break;
		case eMotionCheckCorpse:	index1 = 2; index2 = 0;	 index3 = 0;	break;
		case eMotionLieDownEat:		index1 = 0; index2 = 18; index3 = -1;	break;
		case eMotionAttackJump:		index1 = 0; index2 = 0;  index3 = -1;	break;
		///default:					NODEFAULT;
	} 
	
//	else if (this->SUB_CLS_ID == CLSID_AI_DOG_RED) {
//		switch(motion) {
//			case eMotionStandIdle:		index1 = 0; index2 = 0;	 index3 = -1;	break;
//			case eMotionLieIdle:		index1 = 2; index2 = 0;	 index3 = -1;	break;
//			case eMotionStandTurnLeft:	index1 = 0; index2 = 0;	 index3 = -1;	break;
//			case eMotionWalkFwd:		index1 = 0; index2 = 2;	 index3 = -1;	break;
//			case eMotionWalkBkwd:		index1 = 0; index2 = 2;  index3 = -1;	break;
//			case eMotionWalkTurnLeft:	index1 = 0; index2 = 2;  index3 = -1;	break;
//			case eMotionWalkTurnRight:	index1 = 0; index2 = 2;  index3 = -1;	break;
//			case eMotionRun:			index1 = 0; index2 = 6;  index3 = -1;	break;
//			case eMotionRunTurnLeft:	index1 = 0; index2 = 6;  index3 = -1;	break;
//			case eMotionRunTurnRight:	index1 = 0; index2 = 6;  index3 = -1;	break;
//			case eMotionAttack:			index1 = 0; index2 = 9;  index3 = -1;	break;
//			case eMotionAttackRat:		index1 = 0; index2 = 9;	 index3 = -1;	break;
//			case eMotionFastTurnLeft:	index1 = 0; index2 = 6;  index3 = -1;	break;
//			case eMotionEat:			index1 = 2; index2 = 12; index3 = -1;	break;
//			case eMotionStandDamaged:	index1 = 0; index2 = 0;  index3 = -1;	break;
//			case eMotionScared:			index1 = 0; index2 = 0;  index3 = -1;	break;
//			case eMotionDie:			index1 = 0; index2 = 0;  index3 = -1;	break;
//			case eMotionLieDown:		index1 = 0; index2 = 16; index3 = -1;	break;
//			case eMotionStandUp:		index1 = 2; index2 = 17; index3 = -1;	break;
//			case eMotionCheckCorpse:	index1 = 0; index2 = 0;	 index3 = 0;	break;
//			case eMotionLieDownEat:		index1 = 0; index2 = 16; index3 = -1;	break;
//			case eMotionAttackJump:		index1 = 0; index2 = 0;  index3 = -1;	break;
//			//default:					NODEFAULT;
//		} 
//	}
}


void CAI_Chimera::FillAttackStructure(u32 i, TTime t)
{
	m_tAttack.i_anim		= i;
	m_tAttack.time_started	= t;
	m_tAttack.b_fire_anyway = false;
	m_tAttack.b_attack_rat	= false;

	Fvector tempV;

	switch (m_tAttack.i_anim) {
		case 0:
			m_tAttack.time_from = 700;
			m_tAttack.time_to	= 800;
			m_tAttack.dist		= 3.f;
			
			Center(m_tAttack.TraceFrom);
			break;
		case 1:
			m_tAttack.time_from = 500;
			m_tAttack.time_to	= 600;
			m_tAttack.dist		= 2.5f;
			Center(m_tAttack.TraceFrom);
			tempV.set(0.3f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 2:
			m_tAttack.time_from = 600;
			m_tAttack.time_to	= 700;
			m_tAttack.dist		= 3.5f;
			Center(m_tAttack.TraceFrom);
			break;
		case 3:
			m_tAttack.time_from = 800;
			m_tAttack.time_to	= 900;
			m_tAttack.dist		= 1.0f;
			
			Center(m_tAttack.TraceFrom);
			tempV.set(-0.3f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 4:
			m_tAttack.time_started = 0;
			break;
		case 5:
			m_tAttack.time_from = 1500;
			m_tAttack.time_to	= 1600;
			m_tAttack.dist		= 3.0f;
			Center(m_tAttack.TraceFrom);
			tempV.set(0.3f,0.f,0.f);
			m_tAttack.TraceFrom.add(tempV);
			break;
		case 6:
			m_tAttack.time_started = 0;
			break;
	}
}
