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

void CSE_ALifeSimulator::vfFillCombatGroup(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract, int iGroupIndex)
{
	EHitType				l_tHitType;
	float					l_fHitPower;
	MONSTER_P_VECTOR		&tpGroupVector = m_tpaCombatGroups[iGroupIndex];
	CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeMonsterAbstract);
	tpGroupVector.clear		();
	if (l_tpALifeAbstractGroup) {
		OBJECT_IT			I = l_tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT			E = l_tpALifeAbstractGroup->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(*I));
			R_ASSERT2		(l_tpALifeMonsterAbstract,"Invalid group member!");
			tpGroupVector.push_back(l_tpALifeMonsterAbstract);
			l_tpALifeMonsterAbstract->m_tpCurrentBestWeapon = l_tpALifeMonsterAbstract->tpfGetBestWeapon(l_tHitType,l_fHitPower);
		}
	}
	else {
		tpGroupVector.push_back(tpALifeMonsterAbstract);
		tpALifeMonsterAbstract->m_tpCurrentBestWeapon = tpALifeMonsterAbstract->tpfGetBestWeapon(l_tHitType,l_fHitPower);
	}
	m_tpaCombatMonsters[iGroupIndex] = tpALifeMonsterAbstract;
}

ECombatAction CSE_ALifeSimulator::tfChooseCombatAction(int iCombatGroupIndex)
{
	MONSTER_P_VECTOR	&Members = m_tpaCombatGroups[iCombatGroupIndex];
	MONSTER_P_VECTOR	&Enemies = m_tpaCombatGroups[iCombatGroupIndex ^ 1];
	int i = 0, j = 0, I = (int)Members.size(), J = (int)Enemies.size();
	float	fMinProbability = I ? Members[0]->m_fRetreatThreshold : 0.f;
	while ((i < I) && (j < J)) {
		//getAI().m_tpCurrentALifeObject	= Members[i]->m_tpCurrentBestWeapon;
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
				//getAI().m_tpCurrentALifeObject	= Members[i]->m_tpCurrentBestWeapon;
				getAI().m_tpCurrentALifeMember	= Members[i];
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

bool CSE_ALifeSimulator::bfCheckForInteraction(CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract1, CSE_ALifeMonsterAbstract *tpALifeMonsterAbstract2, int &iCombatGroupIndex)
{
#pragma todo("Dima to Dima: Monster can't notice another monster if it is asleep")
#ifdef OFFLINE_LOG
	vfPrintTime("[LSS]",tfGetGameTime());
	Msg("[LSS] %s met %s on the graph point %d (level %s[%d][%d][%d][%d])",tpALifeMonsterAbstract1->s_name_replace,tpALifeMonsterAbstract2->s_name_replace,tpALifeMonsterAbstract1->m_tGraphID,getAI().GraphHeader().tpLevels[getAI().m_tpaGraph[tpALifeMonsterAbstract1->m_tGraphID].tLevelID].caLevelName,getAI().m_tpaGraph[tpALifeMonsterAbstract1->m_tGraphID].tVertexTypes[0],getAI().m_tpaGraph[tpALifeMonsterAbstract1->m_tGraphID].tVertexTypes[1],getAI().m_tpaGraph[tpALifeMonsterAbstract1->m_tGraphID].tVertexTypes[2],getAI().m_tpaGraph[tpALifeMonsterAbstract1->m_tGraphID].tVertexTypes[3]);
#endif
	if (tpALifeMonsterAbstract1->g_team() != tpALifeMonsterAbstract2->g_team()) {
		getAI().m_tpCurrentALifeObject	= tpALifeMonsterAbstract1;
		getAI().m_tpCurrentALifeMember	= tpALifeMonsterAbstract1;
		getAI().m_tpCurrentALifeEnemy	= tpALifeMonsterAbstract2;
		if (randF(100) < (int)getAI().m_pfNoticeProbability->ffGetValue()) {
			iCombatGroupIndex	= 0;
#ifdef OFFLINE_LOG
			Msg("[LSS] %s detected %s",tpALifeMonsterAbstract1->s_name_replace,tpALifeMonsterAbstract2->s_name_replace);
#endif
			return				(true);
		}
		else {
#ifdef OFFLINE_LOG
			Msg("[LSS] %s didn't detect %s",tpALifeMonsterAbstract1->s_name_replace,tpALifeMonsterAbstract2->s_name_replace);
#endif
			getAI().m_tpCurrentALifeObject	= tpALifeMonsterAbstract2;
			getAI().m_tpCurrentALifeMember	= tpALifeMonsterAbstract2;
			getAI().m_tpCurrentALifeEnemy	= tpALifeMonsterAbstract1;
			if (randF(100) < (int)getAI().m_pfNoticeProbability->ffGetValue()) {
				iCombatGroupIndex	= 1;
				return				(true);
			}
#ifdef OFFLINE_LOG
			Msg("[LSS] %s didn't detect %s",tpALifeMonsterAbstract2->s_name_replace,tpALifeMonsterAbstract1->s_name_replace);
			Msg("[LSS] There is no interaction");
#endif
		}
	}
	return(false);
}

bool CSE_ALifeSimulator::bfCheckIfRetreated(int iCombatGroupIndex)
{
	getAI().m_tpCurrentALifeObject	= m_tpaCombatGroups[iCombatGroupIndex][0];
	getAI().m_tpCurrentALifeMember	= m_tpaCombatGroups[iCombatGroupIndex][0];
	getAI().m_tpCurrentALifeEnemy	= m_tpaCombatGroups[iCombatGroupIndex ^ 1][0];
	return(randF(100) < (int)getAI().m_pfRetreatProbability->ffGetValue());
}

void CSE_ALifeSimulator::vfPerformAttackAction(int iCombatGroupIndex)
{
	if (m_tpaCombatGroups[iCombatGroupIndex ^ 1].empty())
		return;
	MONSTER_P_VECTOR		&l_tCombatGroup = m_tpaCombatGroups[iCombatGroupIndex];
	MONSTER_P_IT			I = l_tCombatGroup.begin();
	MONSTER_P_IT			E = l_tCombatGroup.end();
	for ( ; I != E; I++) {
		EHitType			l_tHitType = eHitTypeMax;
		float				l_fHitPower = 0.f;
		if (!(*I)->m_tpCurrentBestWeapon) {
			CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = (*I)->tpfGetBestWeapon(l_tHitType,l_fHitPower);
			if (!l_tpALifeItemWeapon && (l_fHitPower <= EPS_L))
				continue;
		}
		else {
			l_tHitType		= (*I)->m_tpCurrentBestWeapon->m_tHitType;
			l_fHitPower		= (*I)->m_tpCurrentBestWeapon->m_fHitPower;
		}
		
		//getAI().m_tpCurrentALifeObject = (*I)->m_tpCurrentBestWeapon;
		getAI().m_tpCurrentALifeMember = *I;
#ifdef OFFLINE_LOG
		Msg("[LSS] %s attacks with %s(%d ammo) %d times in a row",(*I)->s_name_replace,(*I)->m_tpCurrentBestWeapon ? (*I)->m_tpCurrentBestWeapon->s_name_replace : "its natural weapon",(*I)->m_tpCurrentBestWeapon ? (*I)->m_tpCurrentBestWeapon->m_dwAmmoAvailable : 0,iFloor(getAI().m_pfWeaponAttackTimes->ffGetValue() + .5f));
#endif
		for (int i=0, n=iFloor(getAI().m_pfWeaponAttackTimes->ffGetValue() + .5f); i<n; i++) {
			if (randF(100) < (int)getAI().m_pfWeaponSuccessProbability->ffGetValue()) {
				// choose random enemy group member and perform hit with random power
				// multiplied by immunity factor
				int							l_iIndex = randI(m_tpaCombatGroups[iCombatGroupIndex ^ 1].size());
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = m_tpaCombatGroups[iCombatGroupIndex ^ 1][l_iIndex];
				l_tpALifeMonsterAbstract->fHealth -= l_tpALifeMonsterAbstract->m_fpImmunityFactors[l_tHitType]*randF(l_fHitPower*0.5f,l_fHitPower*1.5f);
				
				// check if victim became dead
				if (l_tpALifeMonsterAbstract->fHealth <= 0) {
					m_tpaCombatGroups[iCombatGroupIndex ^ 1].erase(m_tpaCombatGroups[iCombatGroupIndex ^ 1].begin() + l_iIndex);
					if (m_tpaCombatGroups[iCombatGroupIndex ^ 1].empty())
						return;
				}
#ifdef OFFLINE_LOG
				Msg("[LSS] %s %s %s (%d left)",(*I)->s_name_replace,l_tpALifeMonsterAbstract->fHealth <= 0 ? "killed" : "attacked",l_tpALifeMonsterAbstract->s_name_replace,m_tpaCombatGroups[iCombatGroupIndex ^ 1].size());
#endif
			}
			else {
#ifdef OFFLINE_LOG
				Msg("[LSS] %s missed",(*I)->s_name_replace);
#endif
			}
			// perform attack (if we use a weapon we should delete ammo we used)
			if (!(*I)->bfPerformAttack())
				break;
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

void CSE_ALifeSimulator::vfAppendItemList(OBJECT_VECTOR &tObjectVector, ITEM_P_VECTOR &tItemList)
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
	m_tpItemVector.clear();
	for (int i=0; i<2; i++) {
		CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(m_tpaCombatMonsters[i]);
		if (l_tpALifeAbstractGroup) {
			for (int I=0, N=l_tpALifeAbstractGroup->m_tpMembers.size() ; I<N; I++) {
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpfGetObjectByID(l_tpALifeAbstractGroup->m_tpMembers[I]));
				R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid group member!");
				l_tpALifeMonsterAbstract->vfUpdateWeaponAmmo	();
				if (l_tpALifeMonsterAbstract->fHealth <= 0) {
					vfAppendItemList							(l_tpALifeMonsterAbstract->children,m_tpItemVector);
					l_tpALifeMonsterAbstract->m_bDirectControl	= true;
					l_tpALifeAbstractGroup->m_tpMembers.erase	(l_tpALifeAbstractGroup->m_tpMembers.begin() + I);
					vfUpdateDynamicData							(l_tpALifeMonsterAbstract);
					vfAssignDeathPosition						(l_tpALifeMonsterAbstract, m_tpaCombatMonsters[0]->m_tGraphID);
					l_tpALifeAbstractGroup->m_wCount--;
					I--;
					N--;
				}
			}
		}
		else {
			m_tpaCombatMonsters[i]->vfUpdateWeaponAmmo			();
			if (m_tpaCombatMonsters[i]->fHealth <= 0) {
				vfAppendItemList								(m_tpaCombatMonsters[i]->children,m_tpItemVector);
				vfAssignDeathPosition							(m_tpaCombatMonsters[i], m_tpaCombatMonsters[0]->m_tGraphID);
			}
		}
	}
	
	// processing items
	ITEM_P_IT	I = m_tpItemVector.begin();
	ITEM_P_IT	E = m_tpItemVector.end();
	for ( ; I != E; I++)
		vfAddObjectToGraphPoint(*I,m_tpaCombatMonsters[0]->m_tGraphID);

	if (m_tpItemVector.empty())
		return;

	int			l_iGroupIndex = -1;
	switch (tCombatResult) {
		case eCombatResultBothKilled	:
		case eCombatResultRetreat12		: break;
		case eCombatResultRetreat2		:
		case eCombatResult1Kill2		: {
			l_iGroupIndex				= 0;
			break;
		}
		case eCombatResultRetreat1		:
		case eCombatResult2Kill1		: {
			l_iGroupIndex				= 1;
			break;
		}
		default							: NODEFAULT;
	}
	
	if (l_iGroupIndex >= 0)
		m_tpaCombatMonsters[l_iGroupIndex]->vfAttachItems();
}
