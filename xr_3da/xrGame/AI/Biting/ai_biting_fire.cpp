////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fire.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Fire and enemy parameters for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

void CAI_Biting::vfSaveEnemy()
{
	m_tSavedEnemy			= m_tEnemy.Enemy;
	m_tSavedEnemyPosition	= m_tEnemy.Enemy->Position();
	m_tpSavedEnemyNode		= m_tEnemy.Enemy->AI_Node;
	m_dwSavedEnemyNodeID	= m_tEnemy.Enemy->AI_NodeID;
	m_dwSeenEnemyLastTime	= Level().timeServer();
	m_tMySavedPosition		= vPosition;
	m_dwMyNodeID			= AI_NodeID;
	vfValidatePosition		(m_tSavedEnemyPosition,m_dwSavedEnemyNodeID);
};

void CAI_Biting::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	m_tHitDir.normalize();
	m_tHitPosition = who->Position();
	
/*
	// Play hit-sound
	sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,vPosition);
	}
	
	if (g_Health() - amount <= 0) {
		if ((m_tpCurrentGlobalAnimation) && (!m_tpCurrentGlobalBlend->playing))
			if (m_tpCurrentGlobalAnimation != m_tRatAnimations.tNormal.tGlobal.tpaDeath[0])
				m_tpCurrentGlobalBlend = PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation = m_tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
	}
*/


}