////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_fire.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "..\\..\\ai_funcs.h"

void CAI_Zombie::Exec_Action(float dt)
{
	AI::C_Command* C	= &q_action;
	AI::AIC_Action* L	= (AI::AIC_Action*)C;
	switch (L->Command) {
		case AI::AIC_Action::AttackBegin: {
			pSounds->PlayAtPos(m_tpaSoundAttack[Random.randI(SND_ATTACK_COUNT)],this,vPosition);
			u32 dwTime = Level().timeServer();
			if ((m_tSavedEnemy->g_Health() > 0) && (dwTime - m_dwStartAttackTime > m_dwHitInterval)) {
				m_bActionStarted = true;
				m_dwStartAttackTime = dwTime;
				Fvector tDirection;
				tDirection.sub(m_tSavedEnemy->Position(),this->Position());
				tDirection.normalize();
				
				if ((this->Local()) && (m_tSavedEnemy) && (m_tSavedEnemy->CLS_ID == CLSID_ENTITY))
					m_tSavedEnemy->Hit(m_fHitPower,tDirection,this);
			}
			else
				m_bActionStarted = false;

			break;
		}
		case AI::AIC_Action::AttackEnd: {
			m_bActionStarted = false;
			break;
		}
		default:
			break;
	}
	if (Device.dwTimeGlobal>=L->o_timeout)	
		L->setTimeout();
}

void CAI_Zombie::HitSignal(float amount, Fvector& vLocalDir, CObject* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	m_tHitDir.normalize();
	m_tHitPosition = who->Position();
	
	// Play hit-sound
	sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		pSounds->PlayAtPos		(S,this,vPosition);
	}
	if (g_Health() - amount <= 0) {
		if ((m_tpCurrentGlobalAnimation) && (!m_tpCurrentGlobalBlend->playing))
			if (m_tpCurrentGlobalAnimation != m_tZombieAnimations.tNormal.tGlobal.tpaDeath[0])
				m_tpCurrentGlobalBlend = PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
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
	objVisible&	Known	= Level().Teams[g_Team()].KnownEnemys;
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	
	if (Known.size()==0)
		return;
	// Get visible list
	feel_vision_get	(m_tpaVisibleObjects);
	std::sort		(m_tpaVisibleObjects.begin(),m_tpaVisibleObjects.end());
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	
	for (u32 i=0; i<Known.size(); i++) {
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			bool bVisible = false;
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)
				if (m_tpaVisibleObjects[i] == E) {
					bVisible = true;
					break;
				}
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.fCost)	{
				S.Enemy		= E;
				S.bVisible	= bVisible;
				S.fCost		= cost;
				Group.m_bEnemyNoticed = true;
			}
		}
	}
}

void CAI_Zombie::vfUpdateMorale()
{
	u32 dwCurTime = Level().timeServer();
	if (m_fMorale < m_fMoraleMinValue)
		m_fMorale = m_fMoraleMinValue;
	if (m_fMorale > m_fMoraleMaxValue)
		m_fMorale = m_fMoraleMaxValue;
	if (dwCurTime - m_dwMoraleLastUpdateTime > m_dwMoraleRestoreTimeInterval) {
		m_dwMoraleLastUpdateTime = dwCurTime;
		float fDistance = vPosition.distance_to(m_tSafeSpawnPosition);
		fDistance = fDistance < 1.f ? 1.f : fDistance;
		switch (eCurrentState) {
			case aiZombieFreeHuntingActive :
			case aiZombieFreeHuntingPassive : {
				if (m_fMorale < m_fMoraleNormalValue) {
					m_fMorale += m_fMoraleRestoreQuant;//*(1.f - fDistance/m_fMoraleNullRadius);
					if (m_fMorale > m_fMoraleNormalValue)
						m_fMorale = m_fMoraleNormalValue;
				}
				else 
					if (m_fMorale > m_fMoraleNormalValue) {
						m_fMorale -= m_fMoraleRestoreQuant;//*(fDistance/m_fMoraleNullRadius);
						if (m_fMorale < m_fMoraleNormalValue)
							m_fMorale = m_fMoraleNormalValue;
					}
				break;
			}
			case aiZombieUnderFire :
			case aiZombieRetreat : {
				//m_fMorale += fDistance <= m_fMoraleNullRadius ? m_fMoraleRestoreQuant : 0;
				//m_fMorale += m_fMoraleRestoreQuant*(m_fMoraleNullRadius/fDistance);
				m_fMorale += m_fMoraleRestoreQuant;
				break;
			}
			case aiZombieAttackRun :
			case aiZombieAttackFire :
			case aiZombieReturnHome : {
				//m_fMorale += m_fMoraleRestoreQuant*(1.f - fDistance/m_fMoraleNullRadius);
				//m_fMorale += m_fMoraleRestoreQuant*(m_fMoraleNullRadius/fDistance);
				m_fMorale += m_fMoraleRestoreQuant;
				break;
			}
		}
		if (m_fMorale < m_fMoraleMinValue)
			m_fMorale = m_fMoraleMinValue;
		if (m_fMorale > m_fMoraleMaxValue)
			m_fMorale = m_fMoraleMaxValue;
	}
}

void CAI_Zombie::vfUpdateMoraleBroadcast(float fValue, float fRadius)
{
	CEntity *tpLeader = Level().Teams[g_Team()].Squads[g_Squad()].Leader;
	if (tpLeader->g_Alive())
		tpLeader->m_fMorale += fValue;
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	for (int i=0; i<(int)Group.Members.size(); i++)
		if (Group.Members[i]->g_Alive())
			Group.Members[i]->m_fMorale += fValue;
}