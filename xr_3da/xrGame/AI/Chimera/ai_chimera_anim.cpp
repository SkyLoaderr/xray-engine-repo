#include "stdafx.h"
#include "ai_chimera.h"

void CAI_Chimera::vfAssignBones(CInifile *ini, const char *section)
{
	int spin_bone		= PKinematics(Visual())->LL_BoneID(ini->r_string(section,"bone_spin"));
	PKinematics(Visual())->LL_GetInstance(spin_bone).set_callback(SpinCallback,this);
}

void __stdcall CAI_Chimera::SpinCallback(CBoneInstance *B)
{
	CAI_Chimera* A = dynamic_cast<CAI_Chimera*> (static_cast<CObject*>(B->Callback_Param));

	A->SpinBoneInMotion(B);
	A->SpinBoneInAttack(B);
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
		case eMotionDie:			index1 = 0; index2 = 15; index3 = -1;	break;
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


void CAI_Chimera::LoadAttackAnim()
{
	Fvector center, left_side, right_side;

	center.set		(0.f,0.f,0.f);
	left_side.set	(-0.3f,0.f,0.f);
	right_side.set	(0.3f,0.f,0.f);

	// 1 //
	m_tAttackAnim.PushAttackAnim(0, 9, 0, 700,	800,	center,		3.f, 20.f);
	
	// 2 //
	m_tAttackAnim.PushAttackAnim(0, 9, 1, 500,	600,	right_side, 2.5f, 20.f);
	
	// 3 // 
	m_tAttackAnim.PushAttackAnim(0, 9, 2, 600,	700,	center,		3.5f, 20.f);

	// 4 // 
	m_tAttackAnim.PushAttackAnim(0, 9, 3, 800,	900,	left_side,	1.0f, 20.f);

	// 5 // 
	m_tAttackAnim.PushAttackAnim(0, 9, 5, 1500, 1600,	right_side, 3.0f, 20.f);
}


void CAI_Chimera::CheckAttackHit()
{
	// Проверка состояния анимации (атака)
	TTime cur_time = Level().timeServer();
	SAttackAnimation apt_anim;

	bool bGoodTime = m_tAttackAnim.CheckTime(cur_time,apt_anim);

	if (bGoodTime) {
		VisionElem ve;
		if (!GetEnemy(ve)) return;
		CObject *obj = dynamic_cast<CObject *>(ve.obj);

		// определить точку, откуда будем пускать луч 
		Fvector trace_from;
		Center(trace_from);
		trace_from.add(apt_anim.trace_offset);

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

