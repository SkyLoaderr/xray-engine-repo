////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

void CAI_Stalker::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	if (Weapons->ActiveWeapon()) {
		fire_pos.set(Weapons->ActiveWeapon()->Position());
		/**
		if ((tSavedEnemy) && (tSavedEnemy->g_Health() > 0)) {
			Fvector fTemp;
			fTemp.sub(tSavedEnemy->Position(),fire_pos);
			fTemp.normalize_safe();
			fire_dir.set(fTemp);
		}
		else
			fire_dir.set(eye_matrix.k);
		/**/
			//fire_dir.set(eye_matrix.k);
			//fire_dir.set(tWatchDirection);
		/**
		if (r_torso_current.yaw > PI)
			fire_dir.setHP(-r_torso_current.yaw + m_fAddWeaponAngle,-r_torso_current.pitch);
		else
			fire_dir.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
		/**/
		fire_dir.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
	}
}

void CAI_Stalker::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
//	m_dwHitTime = Level().timeServer();
//	m_tHitDir.set(D);
//	m_tHitDir.normalize();
//	m_tHitPosition = who->Position();
	
	// Play hit-sound
//	sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
//	if (g_Health() > 0) {
//		if (S.feedback)
//			return;
//		if (Random.randI(2))
//			return;
//		::Sound->PlayAtPos		(S,this,vPosition);
//	}
//	if (g_Health() - amount <= 0) {
//		if ((m_tpCurrentGlobalAnimation) && (!m_tpCurrentGlobalBlend->playing))
//			if (m_tpCurrentGlobalAnimation != m_tRatAnimations.tNormal.tGlobal.tpaDeath[0])
//				m_tpCurrentGlobalBlend = PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation = m_tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
//	}
}

void CAI_Stalker::g_WeaponBones	(int& L, int& R)
{
	VERIFY	(Weapons);
	L		=	Weapons->m_iACTboneL;
	R		=	Weapons->m_iACTboneR;
}