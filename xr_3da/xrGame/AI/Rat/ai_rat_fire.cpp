////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 06.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\actor.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\ai_funcs.h"

using namespace NAI_Rat_Constants;

void CAI_Rat::Exec_Action(float dt)
{
	AI::C_Command* C	= &q_action;
	AI::AIC_Action* L	= (AI::AIC_Action*)C;
	switch (L->Command) {
		case AI::AIC_Action::AttackBegin: {
			
			if (m_tSavedEnemy->g_Health() > 0) {
				DWORD dwTime = Level().timeServer();
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

float CAI_Rat::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team
	
	if (E->g_Health() <= 0)					
		return flt_max;		// don't attack dead enemiyes
	
	float	g_strength = E->g_Armor()+E->g_Health();
	
	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strength);
	//float   f3  = 1;
	//if (E==Level().CurrentEntity())  f3 = .5f;
	return  f1*f2;//*f3;
}

void CAI_Rat::HitSignal(float amount, Fvector& vLocalDir, CObject* who)
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
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->PlayAtPos		(S,this,vPosition);
	if (g_Health() + amount < 0) {
		Fvector	dir;
		dir.setHP(r_torso_current.yaw,r_torso_current.pitch);
		SelectAnimation(clTransform.k,dir,m_fCurSpeed);
	}
}

void CAI_Rat::SelectEnemy(SEnemySelected& S)
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
	
	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	
	for (DWORD i=0; i<Known.size(); i++) {
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			bool bVisible = false;
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)
				if (m_tpaVisibleObjects[i] == E) {
					bVisible = true;
					Msg("%5.2f",vPosition.distance_to(E->Position()));
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

bool CAI_Rat::bfGetActionSuccessProbability(EntityVec &Members, objVisible &VisibleEnemies, CBaseFunction &fSuccessProbabilityFunction)
{
	int i = 0, j = 0, I = (int)Members.size(), J = (int)VisibleEnemies.size();
	while ((i < I) && (j < J)) {
		Level().m_tpAI_DDD->m_tpCurrentMember = dynamic_cast<CEntityAlive *>(Members[i]);
		if (!(Level().m_tpAI_DDD->m_tpCurrentMember) || !(Level().m_tpAI_DDD->m_tpCurrentMember->g_Alive())) {
			i++;
			continue;
		}
		Level().m_tpAI_DDD->m_tpCurrentEnemy = dynamic_cast<CEntityAlive *>(VisibleEnemies[j].key);
		if (!(Level().m_tpAI_DDD->m_tpCurrentEnemy) || !(Level().m_tpAI_DDD->m_tpCurrentEnemy->g_Alive())) {
			j++;
			continue;
		}
		float fProbability = fSuccessProbabilityFunction.ffGetValue()/100.f, fCurrentProbability;
		if (fProbability > MIN_PROBABILITY) {
			fCurrentProbability = fProbability;
			for (j++; (i < I) && (j < J); j++) {
				Level().m_tpAI_DDD->m_tpCurrentEnemy = dynamic_cast<CEntityAlive *>(VisibleEnemies[j].key);
				if (!(Level().m_tpAI_DDD->m_tpCurrentEnemy) || !(Level().m_tpAI_DDD->m_tpCurrentEnemy->g_Alive())) {
					j++;
					continue;
				}
				fProbability = fSuccessProbabilityFunction.ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < MIN_PROBABILITY) {
					i++;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
		else {
			fCurrentProbability = 1.0f - fProbability;
			for (i++; (i < I) && (j < J); i++) {
				Level().m_tpAI_DDD->m_tpCurrentMember = dynamic_cast<CEntityAlive *>(Members[i]);
				if (!(Level().m_tpAI_DDD->m_tpCurrentMember) || !(Level().m_tpAI_DDD->m_tpCurrentMember->g_Alive())) {
					i++;
					continue;
				}
				fProbability = 1.0f - fSuccessProbabilityFunction.ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < MIN_PROBABILITY) {
					j++;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
	}
	return(j >= J);
}

DWORD CAI_Rat::dwfChooseAction(DWORD a1, DWORD a2, DWORD a3)
{
	//return(a3);
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	
	if (Level().timeServer() - Group.m_dwLastActionTime < ACTION_REFRESH_RATE) {
		switch (Group.m_dwLastAction) {
			case 0: return(a1);
			case 1: return(a2);
			default: return(a3);
		}
	}

	objVisible &VisibleEnemies = Level().Teams[g_Team()].KnownEnemys;
	
	if (!VisibleEnemies.size())
		switch (Group.m_dwLastAction) {
			case 0: return(a1);
			case 1: return(a2);
			default: return(a3);
		}

	EntityVec	Members;
	if (this != Level().Teams[g_Team()].Squads[g_Squad()].Leader)
		Members.push_back(Level().Teams[g_Team()].Squads[g_Squad()].Leader);
	Members.push_back(this);
	for (int k=0; k<(int)Group.Members.size(); k++) {
		if (Group.Members[k]->g_Alive())
			Members.push_back(Group.Members[k]);
	}

	if (bfGetActionSuccessProbability(Members,VisibleEnemies,Level().m_tpAI_DDD->pfAttackSuccessProbability)) {
		Group.m_dwLastActionTime = Level().timeServer();
		Group.m_dwLastAction = 0;
		return(a1);
	}
	else
		if (bfGetActionSuccessProbability(Members,VisibleEnemies,Level().m_tpAI_DDD->pfDefendSuccessProbability)) {
			Group.m_dwLastActionTime = Level().timeServer();
			Group.m_dwLastAction = 1;
			return(Group.m_dwLastAction = a2);
		}
		else {
			Group.m_dwLastActionTime = Level().timeServer();
			Group.m_dwLastAction = 2;
			return(Group.m_dwLastAction = a3);
		}
}