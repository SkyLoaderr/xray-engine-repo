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

void CSE_ALifeSimulator::vfFillCombatGroup(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex)
{
	EHitType				l_tHitType;
	float					l_fHitPower;
	SCHEDULE_P_VECTOR		&tpGroupVector = m_tpaCombatGroups[iGroupIndex];
	CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(tpALifeSchedulable);
	tpGroupVector.clear		();
	if (l_tpALifeAbstractGroup) {
		OBJECT_IT			I = l_tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT			E = l_tpALifeAbstractGroup->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeSchedulable	*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>(tpfGetObjectByID(*I));
			R_ASSERT2				(l_tpALifeSchedulable,"Invalid combat object");
			tpGroupVector.push_back(l_tpALifeSchedulable);
			l_tpALifeSchedulable->m_tpCurrentBestWeapon = l_tpALifeSchedulable->tpfGetBestWeapon(l_tHitType,l_fHitPower);
		}
	}
	else {
		tpGroupVector.push_back(tpALifeSchedulable);
		tpALifeSchedulable->m_tpCurrentBestWeapon = tpALifeSchedulable->tpfGetBestWeapon(l_tHitType,l_fHitPower);
	}
	m_tpaCombatObjects[iGroupIndex] = tpALifeSchedulable;
}

ECombatAction CSE_ALifeSimulator::tfChooseCombatAction(int iCombatGroupIndex)
{
	if (m_tCombatType != eCombatTypeMonsterMonster)
		if (((m_tCombatType == eCombatTypeAnomalyMonster) && !iCombatGroupIndex) || ((m_tCombatType == eCombatTypeMonsterAnomaly) && iCombatGroupIndex))
			return(eCombatActionAttack);
		else
			return(eCombatActionRetreat);

	SCHEDULE_P_VECTOR	&Members = m_tpaCombatGroups[iCombatGroupIndex];
	SCHEDULE_P_VECTOR	&Enemies = m_tpaCombatGroups[iCombatGroupIndex ^ 1];
	int i = 0, j = 0, I = (int)Members.size(), J = (int)Enemies.size();
	float	fMinProbability;
	if (!i)
		fMinProbability = 0;
	else {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(Members[0]);
		R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid combat object");
		fMinProbability				= l_tpALifeMonsterAbstract->m_fRetreatThreshold;
	}
	while ((i < I) && (j < J)) {
		getAI().m_tpCurrentALifeMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>(Members[i]);
		getAI().m_tpCurrentALifeEnemy	= dynamic_cast<CSE_ALifeMonsterAbstract*>(Enemies[j]);
		float fProbability = getAI().m_pfVictoryProbability->ffGetValue()/100.f, fCurrentProbability;
		if (fProbability > fMinProbability) {
			fCurrentProbability = fProbability;
			for (j++; (i < I) && (j < J); j++) {
				getAI().m_tpCurrentALifeEnemy = dynamic_cast<CSE_ALifeMonsterAbstract*>(Enemies[j]);
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
				getAI().m_tpCurrentALifeMember	= dynamic_cast<CSE_ALifeMonsterAbstract*>(Members[i]);
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

bool CSE_ALifeSimulator::bfCheckObjectDetection(CSE_ALifeSchedulable *tpALifeSchedulable1, CSE_ALifeSchedulable *tpALifeSchedulable2)
{
	switch (m_tCombatType) {
		case eCombatTypeMonsterMonster : {
			getAI().m_tpCurrentALifeObject	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable1);
			getAI().m_tpCurrentALifeMember	= tpALifeSchedulable1;
			getAI().m_tpCurrentALifeEnemy	= tpALifeSchedulable2;
			return							(randF(100) < (int)getAI().m_pfEnemyDetectProbability->ffGetValue());
		}
		case eCombatTypeAnomalyMonster : {
			getAI().m_tpCurrentALifeEnemy	= tpALifeSchedulable1;
			return							(randF(100) < (int)getAI().m_pfAnomalyInteractProbability->ffGetValue());
		}
		case eCombatTypeMonsterAnomaly : {
			getAI().m_tpCurrentALifeObject	= tpALifeSchedulable1->tpfGetBestDetector();
			getAI().m_tpCurrentALifeMember	= tpALifeSchedulable1;
			getAI().m_tpCurrentALifeEnemy	= tpALifeSchedulable2;
			return							(randF(100) < (int)getAI().m_pfAnomalyDetectProbability->ffGetValue());
		}
		default :							NODEFAULT;
	}
	return									(false);
}

bool CSE_ALifeSimulator::bfCheckForInteraction(CSE_ALifeSchedulable *tpALifeSchedulable1, CSE_ALifeSchedulable *tpALifeSchedulable2, int &iCombatGroupIndex, bool &bMutualDetection)
{
	if (!tpALifeSchedulable1->bfActive() || !tpALifeSchedulable2->bfActive())
		return(false);
	
	// determine combat type
	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract1	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable1);
	CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract2	= dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable2);
	if (!l_tpALifeMonsterAbstract1) {
		if (!l_tpALifeMonsterAbstract2)
			return(false);
		else {
			CSE_ALifeAnomalousZone	*l_tpALifeAnomalousZone	= dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeSchedulable1);
			R_ASSERT2				(l_tpALifeAnomalousZone,"Unknown schedulable object class");
			m_tCombatType			= eCombatTypeAnomalyMonster;
		}
	}
	else {
		if (!l_tpALifeMonsterAbstract2) {
			CSE_ALifeAnomalousZone	*l_tpALifeAnomalousZone	= dynamic_cast<CSE_ALifeAnomalousZone*>(tpALifeSchedulable2);
			R_ASSERT2				(l_tpALifeAnomalousZone,"Unknown schedulable object class");
			m_tCombatType			= eCombatTypeMonsterAnomaly;
		}
		else {
			if (tfGetRelationType(l_tpALifeMonsterAbstract1,l_tpALifeMonsterAbstract2) != eRelationTypeFriend)
				return(false);
			m_tCombatType			= eCombatTypeMonsterMonster;
		}
	}
	
	// perform interaction
	_GRAPH_ID						l_tGraphID = l_tpALifeMonsterAbstract1 ? l_tpALifeMonsterAbstract1->m_tGraphID : l_tpALifeMonsterAbstract2->m_tGraphID;
#ifdef OFFLINE_LOG
	vfPrintTime						("[LSS]",tfGetGameTime());
	Msg								("[LSS] %s met %s on the graph pont %d (level %s[%d][%d][%d][%d])",tpALifeSchedulable1->s_name_replace,tpALifeSchedulable2->s_name_replace,l_tGraphID,getAI().GraphHeader().tpLevels[getAI().m_tpaGraph[l_tGraphID].tLevelID].caLevelName,getAI().m_tpaGraph[l_tGraphID].tVertexTypes[0],getAI().m_tpaGraph[l_tGraphID].tVertexTypes[1],getAI().m_tpaGraph[l_tGraphID].tVertexTypes[2],getAI().m_tpaGraph[l_tGraphID].tVertexTypes[3]);
#endif
	
	bMutualDetection				= false;
	iCombatGroupIndex				= -1;

	if (bfCheckObjectDetection(tpALifeSchedulable1,tpALifeSchedulable2)) {
		iCombatGroupIndex			= 0;
#ifdef OFFLINE_LOG
		Msg							("[LSS] %s detected %s",tpALifeSchedulable1->s_name_replace,tpALifeSchedulable2->s_name_replace);
#endif
	}
	else {
#ifdef OFFLINE_LOG
		Msg							("[LSS] %s didn't detect %s",tpALifeSchedulable1->s_name_replace,tpALifeSchedulable2->s_name_replace);
#endif
	}

	if (bfCheckObjectDetection(tpALifeSchedulable2,tpALifeSchedulable1)) {
		if (!iCombatGroupIndex)
			bMutualDetection		= true;
		else {
			iCombatGroupIndex		= 1;
#ifdef OFFLINE_LOG
			Msg						("[LSS] %s detected %s",tpALifeSchedulable2->s_name_replace,tpALifeSchedulable1->s_name_replace);
#endif
		}
	}
	else {
#ifdef OFFLINE_LOG
		Msg							("[LSS] %s didn't detect %s",tpALifeSchedulable2->s_name_replace,tpALifeSchedulable1->s_name_replace);
#endif
	}

	if (iCombatGroupIndex < 0) {
#ifdef OFFLINE_LOG
		Msg							("[LSS] There is no interaction");
#endif
		return						(false);
	}
	else
		return						(true);
}

bool CSE_ALifeSimulator::bfCheckIfRetreated(int iCombatGroupIndex)
{
	getAI().m_tpCurrentALifeObject = (m_tCombatType == eCombatTypeMonsterMonster) ? dynamic_cast<CSE_ALifeObject*>(m_tpaCombatGroups[iCombatGroupIndex][0]) : m_tpaCombatGroups[iCombatGroupIndex][0]->m_tpBestDetector;
	getAI().m_tpCurrentALifeMember	= m_tpaCombatGroups[iCombatGroupIndex][0];
	getAI().m_tpCurrentALifeEnemy	= m_tpaCombatGroups[iCombatGroupIndex ^ 1][0];
	return							(randF(100) < ((m_tCombatType != eCombatTypeMonsterMonster) ? getAI().m_pfAnomalyRetreatProbability->ffGetValue() : getAI().m_pfEnemyRetreatProbability->ffGetValue()));
}

void CSE_ALifeSimulator::vfPerformAttackAction(int iCombatGroupIndex)
{
	SCHEDULE_P_VECTOR		&l_tCombatGroup = m_tpaCombatGroups[iCombatGroupIndex];
	SCHEDULE_P_IT			I = l_tCombatGroup.begin();
	SCHEDULE_P_IT			E = l_tCombatGroup.end();
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
		
		getAI().m_tpCurrentALifeObject = dynamic_cast<CSE_ALifeObject*>(*I);
		getAI().m_tpCurrentALifeMember = *I;
#ifdef OFFLINE_LOG
		Msg("[LSS] %s attacks with %s(%d ammo) %d times in a row",(*I)->s_name_replace,(*I)->m_tpCurrentBestWeapon ? (*I)->m_tpCurrentBestWeapon->s_name_replace : "its natural weapon",(*I)->m_tpCurrentBestWeapon ? (*I)->m_tpCurrentBestWeapon->m_dwAmmoAvailable : 0,iFloor(getAI().m_pfWeaponAttackTimes->ffGetValue() + .5f));
#endif
		for (int i=0, n=iFloor(getAI().m_pfWeaponAttackTimes->ffGetValue() + .5f); i<n; i++) {
			if (randF(100) < (int)getAI().m_pfWeaponSuccessProbability->ffGetValue()) {
				// choose random enemy group member and perform hit with random power
				// multiplied by immunity factor
				int							l_iIndex = randI(m_tpaCombatGroups[iCombatGroupIndex ^ 1].size());
				CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(m_tpaCombatGroups[iCombatGroupIndex ^ 1][l_iIndex]);
				R_ASSERT2					(l_tpALifeMonsterAbstract,"Invalid combat object");
				l_tpALifeMonsterAbstract->fHealth -= l_tpALifeMonsterAbstract->m_fpImmunityFactors[l_tHitType]*randF(l_fHitPower*0.5f,l_fHitPower*1.5f);
				
#ifdef OFFLINE_LOG
				Msg("[LSS] %s %s %s (%d left)",(*I)->s_name_replace,l_tpALifeMonsterAbstract->fHealth <= 0 ? "killed" : "attacked",l_tpALifeMonsterAbstract->s_name_replace,m_tpaCombatGroups[iCombatGroupIndex ^ 1].size());
#endif
				// check if victim became dead
				if (l_tpALifeMonsterAbstract->fHealth <= 0) {
					m_tpaCombatGroups[iCombatGroupIndex ^ 1].erase(m_tpaCombatGroups[iCombatGroupIndex ^ 1].begin() + l_iIndex);
					if (m_tpaCombatGroups[iCombatGroupIndex ^ 1].empty())
						return;
				}
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
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(m_tpaCombatObjects[0]);
	R_ASSERT2				(l_tpALifeDynamicObject,"Unknown schedulable object class");
	_GRAPH_ID				l_tGraphID = l_tpALifeDynamicObject->m_tGraphID;
	m_tpItemVector.clear();
	for (int i=0; i<2; i++) {
		CSE_ALifeAbstractGroup	*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(m_tpaCombatObjects[i]);
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
					vfAssignDeathPosition						(l_tpALifeMonsterAbstract, l_tGraphID);
					l_tpALifeAbstractGroup->m_wCount--;
					I--;
					N--;
				}
			}
		}
		else {
			m_tpaCombatGroups[i][0]->vfUpdateWeaponAmmo			();
			CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(m_tpaCombatGroups[i][0]);
			if (l_tpALifeMonsterAbstract && (l_tpALifeMonsterAbstract->fHealth <= 0)) {
				vfAppendItemList								(l_tpALifeMonsterAbstract->children,m_tpItemVector);
				vfAssignDeathPosition							(l_tpALifeMonsterAbstract, l_tGraphID);
			}
		}
	}
	
	// processing items
	ITEM_P_IT	I = m_tpItemVector.begin();
	ITEM_P_IT	E = m_tpItemVector.end();
	for ( ; I != E; I++)
		vfAddObjectToGraphPoint(*I,l_tGraphID);

	if (m_tpItemVector.empty() || (m_tCombatType != eCombatTypeMonsterMonster))
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
		m_tpaCombatObjects[l_iGroupIndex]->vfAttachItems();
}
