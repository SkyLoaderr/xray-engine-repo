////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fire.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Fire and enemy parameters for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "..\\..\\actor.h"

float CAI_Biting::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team

	if (!E->g_Alive())
		return flt_max;		// don't attack dead enemies

	float	g_strength = E->g_Armor()+E->g_Health();

	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strength);
	return  f1*f2;
}


void CAI_Biting::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	m_tHitDir.normalize();
	m_tHitPosition = who->Position();
	
	SHurt	tHurt;
	tHurt.dwTime	= Level().timeServer();
	if (0 != (tHurt.tpEntity = dynamic_cast<CEntity*>(who)))
		vfUpdateHurt(tHurt);

	feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);


	// Play hit-sound
	sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,Position());
	}
	
	if (g_Health() - amount <= 0) {
	}
}

int	 CAI_Biting::ifFindHurtIndex(CEntity *tpEntity)
{
	for (int i=0; i<(int)m_tpaHurts.size(); i++)
		if (m_tpaHurts[i].tpEntity == tpEntity)
			return(i);
	return(-1);
}

void CAI_Biting::vfAddHurt(const SHurt &tHurt)
{
	if (m_tpaHurts.size() >= MAX_HURT_COUNT)
		m_tpaHurts.erase(m_tpaHurts.begin());
	m_tpaHurts.push_back(tHurt);
}

void CAI_Biting::vfUpdateHurt(const SHurt &tHurt)
{
	int iIndex = ifFindHurtIndex(tHurt.tpEntity);
	if (iIndex != -1)
		m_tpaHurts[iIndex].dwTime = tHurt.dwTime;
	else
		vfAddHurt(tHurt);
}

float CAI_Biting::CorpHeuristics(CEntity* E)
{
	if (!E->g_Alive()) {
		CEntityAlive *tpEntityAlive = dynamic_cast<CEntityAlive *>(E);
		if ( tpEntityAlive && (m_dwCurrentUpdate - tpEntityAlive->m_dwDeathTime < m_dwEatCorpseInterval) && 
			(tpEntityAlive->m_fFood > 0) && (m_bEatMemberCorpses || (E->g_Team() != g_Team())) && 
			(m_bCannibalism || (E->SUB_CLS_ID != SUB_CLS_ID)))

			return (tpEntityAlive->m_fFood*tpEntityAlive->m_fFood)*Position().distance_to(E->Position());
		else
			return flt_max;
	}
	else
		return flt_max;
}

//void CAI_Biting::SelectCorp(SEnemySelected& S)
//{
//	// Initiate process
//	objVisible&	Known	= Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
//	S.Enemy				= 0;
//	S.bVisible			= FALSE;
//	S.fCost				= flt_max-1;
//	
//	if (Known.size()==0)
//		return;
//	// Get visible list
//	feel_vision_get	(m_tpaVisibleObjects);
//	std::sort		(m_tpaVisibleObjects.begin(),m_tpaVisibleObjects.end());
//	
//	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
//	
//	for (u32 i=0; i<Known.size(); i++) {
//		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
//		float		H = CorpHeuristics(E);
//		if (H < flt_max) {
//			bool bVisible = false;
//			for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)
//				if (m_tpaVisibleObjects[i] == E) {
//					bVisible = true;
//					break;
//				}
//				float	cost	 = bVisible? H*.95f : H;
//			if (cost<S.fCost)	{
//				S.Enemy		= E;
//				S.bVisible	= bVisible;
//				S.fCost		= cost;
//				Group.m_bEnemyNoticed = true;
//			}
//		}
//	}
//}


// Возвращает true - если необходимо нанести Hit
bool CAI_Biting::AttackMelee(CObject *obj, bool bAttackRat) 
{
	// Если actor
	CActor *pA = dynamic_cast<CActor *>(obj);
	if (pA) {
		// получить координаты fire bones
		Fmatrix MBoneLeft;
		Fmatrix MBoneRight;

#pragma todo("TRANSFORM REFACTORING...")
//		MBoneLeft.mul(svTransform, PKinematics(Visual())->LL_GetInstance(m_iLeftFireBone).mTransform);
//		MBoneRight.mul(svTransform, PKinematics(Visual())->LL_GetInstance(m_iRightFireBone).mTransform);

		// получить BBox врага
		Fvector vEnemyBoxCenter;
		float   fEnemyBoxRadius;

		pA->ph_Movement.GetBoundingSphere(vEnemyBoxCenter,fEnemyBoxRadius);

		// положение костей внутри bbox врага?
		bool LeftBoneIn = false, RightBoneIn = false;

		if (MBoneLeft.c.distance_to(vEnemyBoxCenter) < fEnemyBoxRadius) LeftBoneIn = true;
		if (MBoneRight.c.distance_to(vEnemyBoxCenter) < fEnemyBoxRadius) RightBoneIn = true;

		if (LeftBoneIn || RightBoneIn) {
			
			if (m_dwAttackActorMeleeTime + 2000 < m_dwCurrentUpdate){
				m_dwAttackActorMeleeTime = m_dwCurrentUpdate;	
				return true;
			}
			
		}	
		return false;
	}
	
	// если плоть в прыжке || бьёт 
	// если "нужное" время аттаки 
	if (((m_dwAttackMeleeTime + 1000>m_dwCurrentUpdate) && (m_dwAttackMeleeTime + 700 < m_dwCurrentUpdate)) ||
		(m_dwAttackMeleeTime + 2000 < m_dwCurrentUpdate)){
		
		if (obj->Position().distance_to(Position()) < .8f) {
			m_dwAttackMeleeTime = m_dwCurrentUpdate;
			return true;
		}


		this->setEnabled(false);
		Collide::ray_query	l_rq;
		Fvector vCenter;
		Center(vCenter);
		Fvector dir = Direction();
		if (bAttackRat) {
			Fvector tempV;

			tempV.set(0,-Radius(),0);
			dir.add(tempV);
		} 


		if(g_pGameLevel->ObjectSpace.RayPick(vCenter, dir, 1.5f, l_rq)) {
			if ((l_rq.O == obj) && (l_rq.range < 1.0)) {
				m_dwAttackMeleeTime = m_dwCurrentUpdate;
				return true;
			}
		}
		this->setEnabled(true);
	}
	
	// если RayPick успешный
	return false;
}	


