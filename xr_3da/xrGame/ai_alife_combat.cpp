////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_combat.cpp
//	Created 	: 12.08.2003
//  Modified 	: 12.08.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation combats
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"

void CSE_ALifeSimulator::vfFillCombatGroup(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract, MONSTER_P_VECTOR &tpGroupVector)
{
	CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeMonsterAbstract);
	if (l_tpALifeAbstractGroup) {
		OBJECT_IT			I = l_tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT			E = l_tpALifeAbstractGroup->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(*I));
			R_ASSERT2		(l_tpALifeMonsterAbstract,"Invalid group member!");
			tpGroupVector.push_back(l_tpALifeMonsterAbstract);
		}
	}
}

ECombatAction CSE_ALifeSimulator::tfChooseCombatAction(int iCombatGroupIndex)
{
	MONSTER_P_VECTOR	&Members = m_tpaCombatGroups[iCombatGroupIndex];
	MONSTER_P_VECTOR	&Enemies = m_tpaCombatGroups[iCombatGroupIndex ^ 1];
	int i = 0, j = 0, I = (int)Members.size(), J = (int)Enemies.size();
	float	fMinProbability = I ? Members[0]->m_fRetreatThreshold : 0.f;
	while ((i < I) && (j < J)) {
		getAI().m_tpCurrentALifeMember = Members[i];
		getAI().m_tpCurrentALifeEnemy = Enemies[j];
		float fProbability = getAI().m_pfVictoryProbability->ffGetValue()/100.f, fCurrentProbability;
		if (fProbability > fMinProbability) {
			fCurrentProbability = fProbability;
			for (j++; (i < I) && (j < J); j++) {
				getAI().m_tpCurrentALifeEnemy = Enemies[j];
				fProbability = getAI().m_pfVictoryProbability->ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < fMinProbability) {
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
				getAI().m_tpCurrentALifeMember = Members[i];
				fProbability = 1.0f - getAI().m_pfVictoryProbability->ffGetValue()/100.f;
				if (fCurrentProbability*fProbability < fMinProbability) {
					j++;
					break;
				}
				else
					fCurrentProbability *= fProbability;
			}
		}
	}
	return((j >= J) ? eCombatActionAttack : eCombatActionRetreat);
}

bool CSE_ALifeSimulator::bfCheckForCombat(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract1, CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract2, int &iCombatGroupIndex)
{
#pragma todo("Dima to Dima: Monster can't notice another monster if it is asleep")
	if ((tpALifeMonsterAbstract1->g_team() != tpALifeMonsterAbstract2->g_team()) && randI(2)) {
		getAI().m_tpCurrentALifeMember = tpALifeMonsterAbstract1;
		getAI().m_tpCurrentALifeEnemy = tpALifeMonsterAbstract2;
		if (randI(100) < (int)getAI().m_pfNoticeProbability->dwfGetDiscreteValue(100)) {
			iCombatGroupIndex	= 0;
			return				(true);
		}
		else {
			getAI().m_tpCurrentALifeMember = tpALifeMonsterAbstract2;
			getAI().m_tpCurrentALifeEnemy = tpALifeMonsterAbstract1;
			if (randI(100) < (int)getAI().m_pfNoticeProbability->dwfGetDiscreteValue(100)) {
				iCombatGroupIndex	= 1;
				return				(true);
			}
		}
	}
	return(false);
}

bool CSE_ALifeSimulator::bfCheckIfRetreated(int iCombatGroupIndex)
{
	getAI().m_tpCurrentALifeMember	= m_tpaCombatGroups[iCombatGroupIndex][0];
	getAI().m_tpCurrentALifeEnemy	= m_tpaCombatGroups[iCombatGroupIndex ^ 1][0];
	return(randI(100) < (int)getAI().m_pfRetreatProbability->dwfGetDiscreteValue(100));
}

void CSE_ALifeSimulator::vfPerformAttackAction(int iCombatGroupIndex)
{
	MONSTER_P_VECTOR	&l_tCombatGroup = m_tpaCombatGroups[iCombatGroupIndex];
	MONSTER_P_IT		I = l_tCombatGroup.begin();
	MONSTER_P_IT		E = l_tCombatGroup.end();
	for ( ; I != E; I++) {
		vfChooseBestWeapon(*I);
		vfUseWeapon		(*I,m_tpaCombatGroups[iCombatGroupIndex ^ 1]);
	}
}

void CSE_ALifeSimulator::vfChooseBestWeapon(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract)
{

}

void CSE_ALifeSimulator::vfUseWeapon(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract, MONSTER_P_VECTOR	&l_tCombatGroup)
{
}

void CSE_ALifeSimulator::vfFinishCombat(ECombatResult tCombatResult)
{
#pragma todo("Dima to Dima: Append finish combat actions")
}
