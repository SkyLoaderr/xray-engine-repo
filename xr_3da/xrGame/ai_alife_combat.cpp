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
			l_tpALifeMonsterAbstract->m_tpCurrentBestWeapon = 0;
		}
	}
	else
		tpGroupVector.push_back(tpALifeMonsterAbstract);
}

ECombatAction CSE_ALifeSimulator::tfChooseCombatAction(int iCombatGroupIndex)
{
	MONSTER_P_VECTOR	&Members = m_tpaCombatGroups[iCombatGroupIndex];
	MONSTER_P_VECTOR	&Enemies = m_tpaCombatGroups[iCombatGroupIndex ^ 1];
	int i = 0, j = 0, I = (int)Members.size(), J = (int)Enemies.size();
	float	fMinProbability = I ? Members[0]->m_fRetreatThreshold : 0.f;
	while ((i < I) && (j < J)) {
		getAI().m_tpCurrentALifeMember	= Members[i];
		getAI().m_tpCurrentALifeEnemy	= Enemies[j];
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
		if (randI(iFloor(getAI().m_pfNoticeProbability->ffGetMaxResultValue())) < (int)getAI().m_pfNoticeProbability->dwfGetDiscreteValue()) {
			iCombatGroupIndex	= 0;
			return				(true);
		}
		else {
			getAI().m_tpCurrentALifeMember = tpALifeMonsterAbstract2;
			getAI().m_tpCurrentALifeEnemy = tpALifeMonsterAbstract1;
			if (randI(iFloor(getAI().m_pfNoticeProbability->ffGetMaxResultValue())) < (int)getAI().m_pfNoticeProbability->dwfGetDiscreteValue()) {
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
	return(randI(iFloor(getAI().m_pfRetreatProbability->ffGetMaxResultValue())) < (int)getAI().m_pfRetreatProbability->dwfGetDiscreteValue());
}

void CSE_ALifeSimulator::vfPerformAttackAction(int iCombatGroupIndex)
{
	MONSTER_P_VECTOR		&l_tCombatGroup = m_tpaCombatGroups[iCombatGroupIndex];
	MONSTER_P_IT			I = l_tCombatGroup.begin();
	MONSTER_P_IT			E = l_tCombatGroup.end();
	for ( ; I != E; I++) {
		EHitType			l_tHitType = eHitTypeMax;
		float				l_fHitPower = 0.f;
		if (!(*I)->m_tpCurrentBestWeapon) {
			CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = (*I)->tpfGetBestWeapon(l_tHitType,l_fHitPower);
			if (!l_tpALifeItemWeapon && (l_fHitPower <= EPS_L))
				break;
		}
		
		getAI().m_tpCurrentALifeMember = *I;
		for (int i=0, n=iFloor(getAI().m_pfWeaponAttackTimes->ffGetValue()); i<n; i++) {
			if (randI(iFloor(getAI().m_pfWeaponSuccessProbability->ffGetMaxResultValue())) < (int)getAI().m_pfWeaponSuccessProbability->dwfGetDiscreteValue()) {
				// choose random enemy group member and perform hit with random power
				// multiplied by immunity factor
				int							l_iIndex = randI(m_tpaCombatGroups[iCombatGroupIndex ^ 1].size());
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = m_tpaCombatGroups[iCombatGroupIndex ^ 1][l_iIndex];
				l_tpALifeMonsterAbstract->fHealth -= l_tpALifeMonsterAbstract->m_fpImmunityFactors[l_tHitType]*randF(l_fHitPower*0.5f,l_fHitPower*1.5f);
				
				// check if victim became dead
				if (l_tpALifeMonsterAbstract->fHealth <= 0)
					m_tpaCombatGroups[iCombatGroupIndex ^ 1].erase(m_tpaCombatGroups[iCombatGroupIndex ^ 1].begin() + l_iIndex);
				
				// perform attack (if we use a weapon we should delete ammo we used)
				if (!(*I)->bfPerformAttack())
					break;
			}
		}
	}
}

void CSE_ALifeSimulator::vfAssignDeathPosition(CSE_ALifeCreatureAbstract *tpALifeCreatureAbstract, _GRAPH_ID tGraphID)
{
	tpALifeCreatureAbstract->fHealth		= 0;
	SLevelPoint*							l_tpaLevelPoints = (SLevelPoint*)(((u8*)getAI().m_tpaGraph) + getAI().m_tpaGraph[tGraphID].dwPointOffset);
	u32										l_dwDeathpointIndex = randI(getAI().m_tpaGraph[tGraphID].tDeathPointCount);
	tpALifeCreatureAbstract->m_tGraphID		= tGraphID;
	tpALifeCreatureAbstract->o_Position		= l_tpaLevelPoints[l_dwDeathpointIndex].tPoint;
	tpALifeCreatureAbstract->m_tNodeID		= l_tpaLevelPoints[l_dwDeathpointIndex].tNodeID;
	tpALifeCreatureAbstract->m_fDistance	= l_tpaLevelPoints[l_dwDeathpointIndex].fDistance;
}

void CSE_ALifeSimulator::vfAppendItemList(OBJECT_VECTOR &tObjectVector, ITEM_P_LIST &tItemList)
{
	OBJECT_IT	I = tObjectVector.begin();
	OBJECT_IT	E = tObjectVector.end();
	for ( ; I != E; I++) {
		CSE_ALifeItem *l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(tpfGetObjectByID(*I));
		if (l_tpALifeItem)
			tItemList.push_back				(l_tpALifeItem);
	}
}

void CSE_ALifeSimulator::vfFinishCombat(ECombatResult tCombatResult)
{
	// processing weapons and dead monsters
	m_tpItemList.clear();
	for (int i=0; i<2; i++) {
		CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(m_tpaCombatMonsters[i]);
		if (l_tpALifeAbstractGroup) {
			for (int I=0, N=l_tpALifeAbstractGroup->m_tpMembers.size() ; I<N; I++) {
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(l_tpALifeAbstractGroup->m_tpMembers[I]));
				R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid group member!");
				l_tpALifeMonsterAbstract->vfUpdateWeaponAmmo	();
				if (l_tpALifeMonsterAbstract->fHealth <= 0) {
					vfAppendItemList							(l_tpALifeMonsterAbstract->children,m_tpItemList);
					l_tpALifeMonsterAbstract->m_bDirectControl	= true;
					l_tpALifeAbstractGroup->m_tpMembers.erase	(l_tpALifeAbstractGroup->m_tpMembers.begin() + I);
					vfUpdateDynamicData							(l_tpALifeMonsterAbstract);
					vfAssignDeathPosition						(l_tpALifeMonsterAbstract, m_tpaCombatMonsters[0]->m_tGraphID);
					I--;
					N--;
				}
			}
		}
		else {
			m_tpaCombatMonsters[i]->vfUpdateWeaponAmmo			();
			if (m_tpaCombatMonsters[i]->fHealth <= 0) {
				vfAppendItemList								(m_tpaCombatMonsters[i]->children,m_tpItemList);
				vfAssignDeathPosition							(m_tpaCombatMonsters[i], m_tpaCombatMonsters[0]->m_tGraphID);
			}
		}
	}
	
	// processing items
	ITEM_P_LIST_IT	I = m_tpItemList.begin();
	ITEM_P_LIST_IT	E = m_tpItemList.end();
	for ( ; I != E; I++)
		vfAddObjectToGraphPoint(*I,m_tpaCombatMonsters[0]->m_tGraphID);

	int			l_iGroupIndex = -1;
	switch (tCombatResult) {
		case eCombatResultBothKilled	:
		case eCombatResultRetreat12		: break;
		case eCombatResultRetreat1		:
		case eCombatResult1Kill2		: {
			l_iGroupIndex				= 0;
			break;
		}
		case eCombatResultRetreat2		:
		case eCombatResult2Kill1		: {
			l_iGroupIndex				= 1;
			break;
		}
		default							: NODEFAULT;
	}
	
	if (l_iGroupIndex >= 0)
		m_tpaCombatMonsters[l_iGroupIndex]->vfAttachItems();
}
