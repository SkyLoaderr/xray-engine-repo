////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\actor.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\ai_funcs.h"

#define SPECIAL_SQUAD					6
#define LIGHT_FITTING					
#define MIN_PROBABILITY					0.5f

void CAI_Rat::Exec_Action(float dt)
{
	AI::C_Command* C	= &q_action;
	AI::AIC_Action* L	= (AI::AIC_Action*)C;
	switch (L->Command) {
		case AI::AIC_Action::AttackBegin: {
			
			if (tSavedEnemy->g_Health() > 0) {
				DWORD dwTime = Level().timeServer();
				m_bActionStarted = true;
				m_dwStartAttackTime = dwTime;
				Fvector tDirection;
				tDirection.sub(tSavedEnemy->Position(),this->Position());
				tDirection.normalize();
				
				if ((this->Local()) && (tSavedEnemy) && (tSavedEnemy->CLS_ID == CLSID_ENTITY))
					tSavedEnemy->Hit(m_fHitPower,tDirection,this);
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
	float   f3  = 1;
	if (E==Level().CurrentEntity())  f3 = .5f;
	return  f1*f2*f3;
}

// when someone hit rat
void CAI_Rat::HitSignal(float amount, Fvector& vLocalDir, CObject* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwHitTime = Level().timeServer();
	tHitDir.set(D);
	tHitDir.normalize();
	tHitPosition = who->Position();
	
	// Play hit-sound
	sound& S				= sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->PlayAtPos		(S,this,vPosition);
	if (g_Health() + amount < 0) {
		PKinematics(pVisual)->PlayCycle(tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
	}
}

// when someone hit rat
void CAI_Rat::SenseSignal(float amount, Fvector& vLocalDir, CObject* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

void CAI_Rat::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].KnownEnemys;
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	
#ifdef LIGHT_FITTING
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// this code is dummy
	// only for fitting light coefficients
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	bool bActorInCamera = false;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

	if (Known.size()==0) {
#ifdef LIGHT_FITTING
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// this code is dummy
		// only for fitting light coefficients
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (!bActorInCamera && (g_Squad() == SPECIAL_SQUAD))
			Level().HUD()->outMessage(0xffffffff,cName(),"I don't see you");
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
		return;
	}
	
	// Get visible list
	feel_vision_get	(tpaVisibleObjects);
	std::sort		(tpaVisibleObjects.begin(),tpaVisibleObjects.end());
	
	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	
	// Iterate on known
	for (DWORD i=0; i<Known.size(); i++)
	{
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			if (!Group.m_bEnemyNoticed)
#ifdef LIGHT_FITTING
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// this code is dummy
				// only for fitting light coefficients
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (g_Squad() == SPECIAL_SQUAD) {
					bool bB = true;//bfCheckForVisibility(E);
					CActor *tpActor = dynamic_cast<CActor *>(E);
					if (tpActor) {
						Level().HUD()->outMessage(0xffffffff,cName(),bB ? "I see you" : "I don't see you");
						bActorInCamera = true;
						//continue;
					}
					else
						if (!bB)
							continue;
				}
				else
					if (!bfCheckForVisibility(E))
						continue;
#else
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// this code is correct
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (!bfCheckForVisibility(E))
					continue;
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
				// Calculate local visibility
				CObject**	ins	 = lower_bound(tpaVisibleObjects.begin(),tpaVisibleObjects.end(),(CObject*)E);
				bool	bVisible = ((ins==tpaVisibleObjects.end())?FALSE:((E==*ins)?TRUE:FALSE)) && (bfCheckForVisibility(E));
				float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
				if (cost<S.fCost)	{
					S.Enemy		= E;
					S.bVisible	= bVisible;
					S.fCost		= cost;
					Group.m_bEnemyNoticed = true;
				}
		}
	}
#ifdef LIGHT_FITTING
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// this code is dummy
	// only for fitting light coefficients
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (!bActorInCamera && (g_Squad() == SPECIAL_SQUAD))
		Level().HUD()->outMessage(0xffffffff,cName(),"I don't see you");

	//if (g_Squad() == SPECIAL_SQUAD)
	//	Enemy.Enemy = 0;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
}

CAI_Rat::ERatStates CAI_Rat::sfChooseAction()
{
	objVisible &VisibleEnemies = Level().Teams[g_Team()].KnownEnemys;
	if (!VisibleEnemies.size())
		return(tStateStack.top());
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	EntityVec	Members;
	Members.push_back(Level().Teams[g_Team()].Squads[g_Squad()].Leader);
	Members.push_back(this);
	for (int k=0; k<(int)Group.Members.size(); k++)
		if (Group.Members[k]->g_Alive())
			Members.push_back(Group.Members[k]);

	int i = 0, j = 0;
	float	fCurrentMemberProbability = 1.0f;
	float	fCurrentEnemyProbability	= 1.0f;
	bool	bMySide = true, bStarted = true;
	do {
		CEntityAlive *m_tpCurrentMember = dynamic_cast<CEntityAlive *>(Members[j]);
		if (!m_tpCurrentMember) {
			i++;
			continue;
		}
		m_tpCurrentEnemy = dynamic_cast<CEntityAlive *>(VisibleEnemies[j].key);
		if (!m_tpCurrentEnemy) {
			j++;
			continue;
		}
		float fProbability = pfAttackSuccessProbability.ffGetValue(this,fpaBaseFunctions);
		if (bMySide) {
			if (fCurrentMemberProbability*fProbability < MIN_PROBABILITY) {
				if (bStarted)
					bMySide = false;
				fCurrentEnemyProbability = 1 - fProbability;
				i++;
				continue;
			}
			else {
				fCurrentMemberProbability *= fProbability;
				j++;
				continue;
			}
		}
		if (!bMySide) {
			if (fCurrentEnemyProbability*(1.0f - fProbability) > MIN_PROBABILITY) {
				if (bStarted)
					bMySide = true;
				fCurrentMemberProbability = fProbability;
				j++;
				continue;
			}
			else {
				fCurrentMemberProbability *= fProbability;
				i++;
				continue;
			}
		}
	}
	while ((i < (int)Members.size()) && (j < (int)VisibleEnemies.size()));
	return(aiRatAttackRun);
}
