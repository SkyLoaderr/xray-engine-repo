#include "stdafx.h"
#include "ai_flesh.h"

#include "..\\..\\ai_space.h"
using namespace AI;

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

	// TEMP
	PState							= 0;
	PressedLastTime					= 0;	
	// ----
}


void CAI_Flesh::StateSelector()
{
	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && H && !I)		SetState(stateAttack);		//тихо подобраться и начать аттаку
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(stateHide);		// отход перебежками через укрытия
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);		//тихо подобраться и начать аттаку
	else if (E && !H && I) 		SetState(stateDetour); 
	else if (E && !H && !I)		SetState(stateDetour); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateDetour); 
	else if (F && !H && !I) 	SetState(stateHide);
	else if (A && !K && !H)		SetState(stateExploreNDE);  // SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
	else if (A && !K && H)		SetState(stateExploreNDE);  // SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K && !H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
	else if (B && !K && H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 
}

void CAI_Flesh::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	index1 = index2 = 0;		// bug protection ;) todo: find out the reason
	index3 = -1;

	switch(motion) {
		case eAnimStandIdle:		index1 = 0; index2 = 0;	 break;
		case eAnimLieIdle:			index1 = 2; index2 = 0;	 break;
		case eAnimStandTurnLeft:	index1 = 0; index2 = 2;	 break;
		case eAnimWalkFwd:			index1 = 0; index2 = 3;	 break;
		case eAnimWalkBkwd:			index1 = 0; index2 = 4;  break;
		case eAnimWalkTurnLeft:		index1 = 0; index2 = 5;  break;
		case eAnimWalkTurnRight:	index1 = 0; index2 = 6;  break;
		case eAnimRun:				index1 = 0; index2 = 7;  break;
		case eAnimRunTurnLeft:		index1 = 0; index2 = 8;  break;
		case eAnimRunTurnRight:		index1 = 0; index2 = 9;  break;
		case eAnimAttack:			index1 = 0; index2 = 10;  break;
		case eAnimAttackRat:		index1 = 0; index2 = 11; break;
		case eAnimFastTurn:			index1 = 0; index2 = 13; break;
		case eAnimEat:				index1 = 2; index2 = 14; break;
		case eAnimStandDamaged:		index1 = 0; index2 = 15; break;
		case eAnimScared:			index1 = 0; index2 = 16; break;
		case eAnimDie:				index1 = 0; index2 = 17; break;
		case eAnimStandLieDown:		index1 = 0; index2 = 20; break;
		case eAnimLieStandUp:		index1 = 2; index2 = 21; break;
		case eAnimCheckCorpse:		index1 = 0; index2 = 0;	 index3 = 2;	break;
		case eAnimStandLieDownEat:	index1 = 0; index2 = 22; break;
		case eAnimAttackJump:		index1 = 0; index2 = 12; break;
		//default:					NODEFAULT;
	}

	if (index3 == -1) index3 = ::Random.randI((int)m_tAnimations.A[index1].A[index2].A.size());
}

void CAI_Flesh::LoadAttackAnim()
{
	Fvector center, left_side, right_side, special_side;

	center.set		(0.f,0.f,0.f);
	left_side.set	(-0.1f,0.f,0.f);
	right_side.set	(0.1f,0.f,0.f);
	special_side.set(-0.5f,0.f,0.5f);

	// 1 //
	m_tAttackAnim.PushAttackAnim(0, 10, 0, 700,	800,	center,		2.f, m_fHitPower, 0.f, 0.f);

	// 2 //
	m_tAttackAnim.PushAttackAnim(0, 10, 1, 600,	800,	center,		2.5f, m_fHitPower, 0.f, 0.f);

	// 3 // 
	m_tAttackAnim.PushAttackAnim(0, 10, 2, 1100,	1250,	right_side,	1.5f, m_fHitPower, 0.f, 0.f);

	// 4 // 
	m_tAttackAnim.PushAttackAnim(0, 10, 3, 1300,	1400,	left_side,	0.6f, m_fHitPower, 0.f, 0.f);

	// 5 // 
	m_tAttackAnim.PushAttackAnim(0, 11, 0, 600, 800,	special_side,	2.6f, m_fHitPower, 0.f, 0.f, AA_FLAG_ATTACK_RAT);

	// 6 //
	m_tAttackAnim.PushAttackAnim(0, 12, 0, 700, 850,	center,		2.6f, m_fHitPower, 0.f, 0.f, AA_FLAG_FIRE_ANYWAY);

}

void CAI_Flesh::CheckAttackHit()
{
	// Проверка состояния анимации (атака)
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

		// трассировка нужна?
		if ((apt_anim.flags & AA_FLAG_FIRE_ANYWAY) == AA_FLAG_FIRE_ANYWAY) {
			DoDamage(ve.obj, apt_anim.damage,apt_anim.dir_yaw, apt_anim.dir_pitch);	// не нужна
			m_tAttackAnim.UpdateLastAttack(cur_time);
		} else if ((apt_anim.flags & AA_FLAG_ATTACK_RAT) == AA_FLAG_ATTACK_RAT) {

			// TestIntersection конуса(копыта) и сферы(крысы)
			Fvector dir;	dir.set(0.f,-1.f,0.f);		// направление конуса
			Fvector vC;		ve.obj->Center(vC);			// центр сферы

			if (ConeSphereIntersection(trace_from, PI_DIV_6, dir, vC, ve.obj->Radius())) {
				DoDamage(ve.obj,apt_anim.damage,apt_anim.dir_yaw, apt_anim.dir_pitch);
				m_tAttackAnim.UpdateLastAttack(cur_time);
			}

		} else 	{ // нужна
			this->setEnabled(false);
			Collide::ray_query	l_rq;
			
			if (Level().ObjectSpace.RayPick(trace_from, Direction(), apt_anim.dist, l_rq)) {
				if ((l_rq.O == obj) && (l_rq.range < apt_anim.dist)) {
					DoDamage(ve.obj, apt_anim.damage,apt_anim.dir_yaw, apt_anim.dir_pitch);
					m_tAttackAnim.UpdateLastAttack(cur_time);
				}
			}
			this->setEnabled(true);			

			if (!ve.obj->g_Alive()) AddCorpse(ve);	
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Функция ConeSphereIntersection
// Пересечение конуса (не ограниченного) со сферой
// Необходима для определения пересечения копыта плоти с баунд-сферой крысы
// Параметры: ConeVertex - вершина конуса, ConeAngle - угол конуса (между поверхностью и высотой)
// ConeDir - направление конуса, SphereCenter - центр сферы, SphereRadius - радиус сферы
bool CAI_Flesh::ConeSphereIntersection(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, Fvector SphereCenter, float SphereRadius)
{
	float fInvSin = 1.0f/_sin(ConeAngle);
	float fCosSqr = _cos(ConeAngle)*_cos(ConeAngle);

	
	Fvector kCmV;	kCmV.sub(SphereCenter,ConeVertex);
	Fvector kD		= kCmV;
	Fvector tempV	= ConeDir;
	tempV.mul		(SphereRadius* fInvSin);
	kD.add			(tempV);

	float fDSqrLen = kD.square_magnitude();
	float fE = kD.dotproduct(ConeDir);
	if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr ) {
		
		float fSinSqr = _sin(ConeAngle)*_sin(ConeAngle);

		fDSqrLen = kCmV.square_magnitude();
		fE = -kCmV.dotproduct(ConeDir);
		if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr ) {
			float fRSqr = SphereRadius*SphereRadius;
			return fDSqrLen <= fRSqr;
		} else return true;
	} 
	
	return false;
}

#ifdef DEBUG
void CAI_Flesh::OnRender()
{
	inherited::OnRender();

	RCache.OnFrameEnd				();

	if (PTurn.empty()) return;

	Fvector pos;
	
	// Draw start point	
	pos = PTurn[0];
	pos.y += 0.1f;
	RCache.dbg_DrawAABB	(pos,.1f,.1f,.1f,D3DCOLOR_XRGB(0x33,0x00,0xff));

	Fvector P1,P2;
	for (u32 i=1; i<PTurn.size();i++) {
		// draw line between previous node and this node
		P1 = PTurn[i-1];	P1.y += 0.1f;
		P2 = PTurn[i];		P2.y += 0.1f;
		RCache.dbg_DrawLINE			(Fidentity,P1,P2,D3DCOLOR_XRGB(0xFF,0x00,0x33));
		// draw this node

		if (i >= PTurn.size() - 2) {
			RCache.dbg_DrawAABB	(P2,.1f,.1f,.1f,D3DCOLOR_XRGB(0xff,0x00,0x00));
		} else RCache.dbg_DrawAABB	(P2,.1f,.1f,.1f,D3DCOLOR_XRGB(0xff,0xff,0x00));


	}

}
#endif

///////////////////////////////////////////////////////////////////////////////////
// TEMP
void CAI_Flesh::SetPoints()
{
	if (PressedLastTime + 500 > Level().timeServer()) return;

	if (Level().IR_GetKeyState(DIK_RCONTROL)) {
		PressedLastTime = Level().timeServer();
		
		CObject	*pE = Level().CurrentEntity();
			

		switch (PState) {
			case 0:
				PTurn.clear();
				PStart		= pE->Position();
				MStart		= pE->XFORM();
				break;
			case 1:
				PFinish		= pE->Position();
				break;
			case 2:
				MakeTurn();
				break;
		}
		PState++;

		if (PState > 2) PState = 0;
	}

}

void CAI_Flesh::MakeTurn()
{
	Fvector		S;					// start point
	Fvector		F;					// finish point
	Fmatrix		xform;
	float		R = 2.f;

	S		= PStart;
	F		= PFinish;
	xform	= MStart;

	PTurn.push_back(S);
	
	bool bLeftSide;
	float yaw1, yaw2, tt;
	S.getHP(yaw1,tt);
	F.getHP(yaw2,tt);

 	if (angle_normalize_signed(yaw2 - yaw1) > 0) bLeftSide = true;
	else bLeftSide = false;
	Msg("Left?: [%i]", bLeftSide);

	Fvector		P = S;
	(bLeftSide) ? MoveInAxis(P, xform.i, -R) : MoveInAxis(P, xform.i, R);
	
	float d  =	F.distance_to(P);
	float angle = acosf(R / d);

	Fvector Ddir;
	Ddir.sub(F,P);
	Ddir.normalize();

	float yaw,pitch;
	Ddir.getHP(yaw,pitch);
	yaw += ((bLeftSide) ? -angle : angle);
	yaw = angle_normalize(yaw);
	Ddir.setHP(yaw,pitch);
	Ddir.mul(R);

	
	Fvector Q;
	Q.add(P,Ddir);
	
	// построение окружности
	float min_angle = PI_DIV_6;
	float curYaw;
	float destYaw;
	float noneV;

	Fvector tV;
	tV.sub(Q,P);
	tV.getHP(destYaw,noneV);
	tV.sub(S,P);
	tV.getHP(curYaw,noneV);
	
	while (!getAI().bfTooSmallAngle(destYaw,curYaw,min_angle)) {
		Fvector newP = P;
		curYaw += ((bLeftSide) ? min_angle : -min_angle);
		curYaw = angle_normalize(curYaw);
		tV.setHP(curYaw,noneV);
		tV.normalize();
		tV.mul(R);
		newP.add(tV);
		PTurn.push_back(newP);
	}

	//PTurn.push_back(P);
	PTurn.push_back(Q);
	PTurn.push_back(F);
}


// Перемещенеи вдоль вектора направления 'dir' на растояние dx
void CAI_Flesh::MoveInAxis(Fvector &Pos, const Fvector &dir,  float dx)
{
	// dir - should be already normalized
	Fvector shift;
	shift.mul(dir, dx);
	Pos.add(shift);
}




