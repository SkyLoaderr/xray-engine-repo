////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_fire.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"

void CAI_Zombie::Exec_Action(float /**dt/**/)
{
	switch (m_tAction) {
		case eZombieActionAttackBegin : {
			bool bOk = false;
			if (m_tpSoundBeingPlayed) {
				for (int i=0; i<SND_ATTACK_COUNT; ++i)
					if (&(m_tpaSoundAttack[i]) == m_tpSoundBeingPlayed) {
						bOk = true;
						break;
					}
			}
			if (!bOk)
				::Sound->play_at_pos(m_tpaSoundAttack[Random.randI(SND_ATTACK_COUNT)],this,eye_matrix.c);
			else
				m_tpSoundBeingPlayed->set_position(eye_matrix.c);
			u32 dwTime = Level().timeServer();
			if ((m_tSavedEnemy->g_Health() > 0) && (dwTime - m_dwStartAttackTime > m_dwHitInterval)) {
				m_bActionStarted = true;
				m_dwStartAttackTime = dwTime;
				Fvector tDirection;
				Fvector position_in_bone_space;
				position_in_bone_space.set(0.f,0.f,0.f);
				tDirection.sub(m_tSavedEnemy->Position(),this->Position());
				tDirection.normalize();
				
				if ((this->Local()) && (m_tSavedEnemy) && (m_tSavedEnemy->CLS_ID == CLSID_ENTITY))
					m_tSavedEnemy->Hit(m_fHitPower,tDirection,this,-1,position_in_bone_space,0);
			}
			else
				m_bActionStarted = false;

			break;
		}
		case eZombieActionAttackEnd : {
			m_bActionStarted = false;
			break;
		}
		default:
			break;
	}
}

void CAI_Zombie::HitSignal(float /**amount/**/, Fvector& vLocalDir, CObject* who, s16 /**element/**/)
{
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	m_hit_time = Level().timeServer();
	m_hit_direction.set(D);
	m_hit_direction.normalize();
	m_tHitPosition = who->Position();
	
	// Play hit-ref_sound
	ref_sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,Position());
	}
}

float CAI_Zombie::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team
	
	if (!E->g_Alive())					
		return flt_max - 0;		// don't attack dead enemiyes
	
	float	g_strength = E->g_Armor()+E->g_Health();
	
	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strength);
	//float   f3  = 1;
	//if (E==Level().CurrentEntity())  f3 = .5f;
	return  f1*f2;//*f3;
}

void CAI_Zombie::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	S.m_enemy			= 0;
	S.m_visible			= FALSE;
	S.m_cost			= flt_max-1;
	
	if (Known.size()==0)
		return;
	// Get visible list
	feel_vision_get	(m_tpaVisibleObjects);
	std::sort		(m_tpaVisibleObjects.begin(),m_tpaVisibleObjects.end());
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	
	for (u32 i=0; i<Known.size(); ++i) {
		CEntityAlive*	E = dynamic_cast<CEntityAlive*>(Known[i].key);
		if (!E)
			continue;
		float		H = EnemyHeuristics(E);
		if (H<S.m_cost) {
			bool bVisible = false;
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); ++i)
				if (m_tpaVisibleObjects[i] == E) {
					bVisible = true;
					break;
				}
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.m_cost)	{
				S.m_enemy		= E;
				S.m_visible		= bVisible;
				S.m_cost		= cost;
				Group.m_bEnemyNoticed = true;
			}
		}
	}
}
