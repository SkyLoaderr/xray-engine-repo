////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_misc.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Miscellanious functions for all the chimera monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"
#include "..\\..\\actor.h"
#include "..\\..\\hudmanager.h"

using namespace AI_Chimera;

const float tempCrouchFactor = 0.5f;
const float tempWalkFactor = 1.7f;
const float tempWalkFreeFactor = 1.7f;
const float tempRunFactor = 5.0f;
const float tempRunFreeFactor = 5.0f;
const float tempPanicFactor = 5.0f;

const float min_angle = PI_DIV_4;
const float min_turning_angle = PI_DIV_6;

// исправление несоответстви€ позиции узлу 
void CAI_Chimera::vfValidatePosition(Fvector &tPosition, u32 dwNodeID)
{
	//if ((dwNodeID <= 0) || (dwNodeID >= getAI().Header().count) || (getAI().dwfCheckPositionInDirection(dwNodeID,getAI().tfGetNodeCenter(dwNodeID),tPosition) == u32(-1)))
	//	m_tSavedEnemyPosition = getAI().tfGetNodeCenter(dwNodeID);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬ходные воздействи€ 
// «рение, слух, веро€тность победы, выгодность противника
void CAI_Chimera::vfUpdateParameters()
{
	UpdateMemory();

	//------------------------------------
	// слух
	A = B = false;
	
	SoundElem se;
	
	if (IsRememberSound()) {
		GetMostDangerousSound(se,A);
		B = !A;
	}


	J = A | B;

	//------------------------------------
	// «рение
	objVisible			&VisibleEnemies = Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;

	VisionElem ve;
	if (GetEnemy(ve)) {
		VisibleEnemies.insert(ve.obj);

		// определить, видит ли мен€ враг
		I = false;
		float			yaw1 = 0.f, pitch1 =0.f, yaw2, pitch2, fYawFov = 0.f, fPitchFov = 0.f, fRange = 0.f;
		

		CCustomMonster	*tpCustomMonster = dynamic_cast<CCustomMonster *>(ve.obj);
		if (tpCustomMonster) {
			yaw1		= -tpCustomMonster->r_current.yaw;
			pitch1		= -tpCustomMonster->r_current.pitch;
			fYawFov		= angle_normalize_signed(tpCustomMonster->ffGetFov()*PI/180.f);
			fRange		= tpCustomMonster->ffGetRange();
		}
		else {
			CActor		*tpActor = dynamic_cast<CActor *>(ve.obj);
			if (tpActor) {
				yaw1	= tpActor->Orientation().yaw;
				pitch1	= tpActor->Orientation().pitch;
				fYawFov	= angle_normalize_signed(tpActor->ffGetFov()*PI/180.f);
				fRange	= tpActor->ffGetRange();
			}
		}

		if (ve.position.distance_to(Position()) <= fRange) {
			
			fYawFov			= angle_normalize_signed((_abs(fYawFov) + _abs(atanf(1.f/ve.position.distance_to(Position()))))/2.f);
			fPitchFov		= angle_normalize_signed(fYawFov*1.f);
			ve.position.sub	(Position());
			ve.position.mul	(-1);
			ve.position.getHP	(yaw2,pitch2);
			yaw1			= angle_normalize_signed(yaw1);
			pitch1			= angle_normalize_signed(pitch1);
			yaw2			= angle_normalize_signed(yaw2);
			pitch2			= angle_normalize_signed(pitch2);
			
			I = getAI().bfTooSmallAngle(yaw1,yaw2,fYawFov) && (getAI().bfTooSmallAngle(pitch1,pitch2,fPitchFov));
		}
	}


	//------------------------------------
	// веро€тность победы
	C = D = E = F = G	= false;
	
	if (bfIsAnyAlive(VisibleEnemies)) {
		switch (dwfChooseAction(0,m_fAttackSuccessProbability0,m_fAttackSuccessProbability1,m_fAttackSuccessProbability2,m_fAttackSuccessProbability3,g_Team(),g_Squad(),g_Group(),0,1,2,3,4,this,30.f)) {
			case 4 : 
				C = true;
				break;
			case 3 : 
				D = true;
				break;
			case 2 : 
				E = true;
				break;
			case 1 : 
			case 0 : 
				F = true;
				break;
		}
	}
	K					= C | D | E | F;

	//------------------------------------
	// враг выгоден ?
	H = false;
	getAI().m_tpCurrentMember = this;
	for (int i=0, n=VisibleEnemies.size(); i<n; i++) {
		if (0 == (getAI().m_tpCurrentEnemy  = dynamic_cast<CEntityAlive*>(VisibleEnemies[i].key)))
			continue;
		if ((E || F || G) && (0 != (H = !!getAI().m_pfExpediency->dwfGetDiscreteValue(2))))
			break;
		else
			if (ifFindHurtIndex(getAI().m_tpCurrentEnemy) != -1)
				H = true;
	}

	// temp - выгоден
//	if (ve.obj) {
//		D = C = false;
//		E = true;
//	}
	H = false;
	H= true;
}




//bool CAI_Chimera::IsLeftSide(const Fvector &Position)
//{
//	Fvector iV; // i-состовл€юща€ матрицы mRotate
//	Fvector temp;
//
//	iV		= XFORM().i;
//	temp	= XFORM().c;
//	temp.sub(Position);
//	float f = temp.dotproduct(iV);
//
//	if (f >= 0) return true;
//	else return false;
//}
