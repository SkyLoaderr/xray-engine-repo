////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_monster.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation of monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

void CSE_ALifeMonsterAbstract::Update		()
{
	bool				bContinue;
	do {
		bContinue		= false;
		if (fHealth <= 0)
			return;
		if (m_tNextGraphID != m_tGraphID) {
			_TIME_ID					tCurTime = m_tpALife->tfGetGameTime();
			m_fDistanceFromPoint		+= float(tCurTime - m_tTimeID)/1000.f * m_fCurSpeed;
			if (m_fDistanceToPoint - m_fDistanceFromPoint < EPS_L) {
				bContinue = true;
				if ((m_fDistanceFromPoint - m_fDistanceToPoint > EPS_L) && (m_fCurSpeed > EPS_L))
					m_tTimeID		= tCurTime - _TIME_ID(iFloor((m_fDistanceFromPoint - m_fDistanceToPoint)*1000.f/m_fCurSpeed));
				m_fDistanceToPoint		= m_fDistanceFromPoint	= 0.0f;
				m_tPrevGraphID			= m_tGraphID;
				m_tpALife->vfChangeObjectGraphPoint(this,m_tGraphID,m_tNextGraphID);
				CSE_ALifeAbstractGroup	*tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(this);
				if (tpALifeAbstractGroup)
					tpALifeAbstractGroup->m_bCreateSpawnPositions = true;
			}
		}
		if (m_tNextGraphID == m_tGraphID) {
			_GRAPH_ID					tGraphID = m_tNextGraphID;
			u16							wNeighbourCount = (u16)getAI().m_tpaGraph[tGraphID].tNeighbourCount;
			CSE_ALifeGraph::SGraphEdge	*tpaEdges = (CSE_ALifeGraph::SGraphEdge *)((BYTE *)getAI().m_tpaGraph + getAI().m_tpaGraph[tGraphID].dwEdgeOffset);

			TERRAIN_VECTOR				&tpaTerrain = m_tpaTerrain;
			int							iPointCount = (int)tpaTerrain.size();
			int							iBranches = 0;
			for (int i=0; i<wNeighbourCount; i++)
				if (tpaEdges[i].dwVertexNumber != m_tPrevGraphID)
					for (int j=0; j<iPointCount; j++)
						if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes))
							iBranches++;
			bool						bOk = false;
			if (!iBranches) {
				for (int i=0; i<wNeighbourCount; i++) {
					for (int j=0; j<iPointCount; j++)
						if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes)) {
							m_tNextGraphID = (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
							m_fDistanceToPoint = tpaEdges[i].fPathDistance;
							bOk = true;
							break;
						}
						if (bOk)
							break;
				}
			}
			else {
				int iChosenBranch = m_tpALife->randI(0,iBranches);
				iBranches = 0;
				for (int i=0; i<wNeighbourCount; i++)
					if (tpaEdges[i].dwVertexNumber != m_tPrevGraphID) {
						for (int j=0; j<iPointCount; j++)
							if (getAI().bfCheckMask(tpaTerrain[j].tMask,getAI().m_tpaGraph[tpaEdges[i].dwVertexNumber].tVertexTypes) && (tpaEdges[i].dwVertexNumber != m_tPrevGraphID)) {
								if (iBranches == iChosenBranch) {
									m_tNextGraphID	= (_GRAPH_ID)tpaEdges[i].dwVertexNumber;
									m_fDistanceToPoint = tpaEdges[i].fPathDistance;
									bOk = true;
									break;
								}
								iBranches++;
							}
							if (bOk)
								break;
					}
			}
			if (!bOk) {
				m_fCurSpeed			= 0.0f;
				m_fDistanceToPoint	= 0.0f;
				bContinue			= false;
			}
			else
				m_fCurSpeed			= m_fGoingSpeed;
		}
		m_tpALife->vfCheckForInteraction(this);
	}
	while (bContinue && (m_tpALife->m_tpActor->o_Position.distance_to(o_Position) > m_tpALife->m_fOnlineDistance));
	m_tTimeID					= m_tpALife->tfGetGameTime();
}

CSE_ALifeItemWeapon	*CSE_ALifeMonsterAbstract::tpfGetBestWeapon(EHitType &tHitType, float &fHitPower)
{
	fHitPower					= m_fHitPower;
	tHitType					= m_tHitType;
	return						(0);
}

EMeetActionType	CSE_ALifeMonsterAbstract::tfGetActionType(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	if (m_tpALife->m_tCombatType == eCombatTypeMonsterMonster) {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable);
		R_ASSERT2					(l_tpALifeMonsterAbstract,"Inconsistent meet action type");
		return						(m_tpALife->tfGetRelationType(this,dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable)) == eRelationTypeFriend ? eMeetActionTypeIgnore : ((bMutualDetection || m_tpALife->tfChooseCombatAction(iGroupIndex)==eCombatActionAttack) ? eMeetActionTypeAttack : eMeetActionTypeIgnore));
	}
	else
		return(eMeetActionTypeAttack);
}

bool CSE_ALifeMonsterAbstract::bfActive()
{
	CSE_ALifeAbstractGroup		*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(this);
	return						((l_tpALifeAbstractGroup && (l_tpALifeAbstractGroup->m_wCount > 0)) || (fHealth > 0));
}

CSE_ALifeDynamicObject *CSE_ALifeMonsterAbstract::tpfGetBestDetector()
{
	CSE_ALifeAbstractGroup		*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(this);
	if (!l_tpALifeAbstractGroup)
		return					(this);
	else {
		if (!l_tpALifeAbstractGroup->m_wCount)
			return				(0);
		else
			return					(m_tpALife->tpfGetObjectByID(l_tpALifeAbstractGroup->m_tpMembers[0]));
	}
}
