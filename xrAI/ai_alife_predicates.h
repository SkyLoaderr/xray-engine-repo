////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_predicates.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life predicates
////////////////////////////////////////////////////////////////////////////

#ifndef AI_ALIFE_PREDICATESH
#define AI_ALIFE_PREDICATESH
#pragma once

class CArtefactOrderPredicate {
public:
	bool							operator()							(const SArtefactOrder &tArtefactOrder1, const SArtefactOrder &tArtefactOrder2) const
	{
		return						(tArtefactOrder1.m_dwPrice > tArtefactOrder2.m_dwPrice);
	};
};

class CArtefactPredicate {
public:
	bool							operator()							(const SArtefactTraderOrder tArtefactOrder1, const SArtefactTraderOrder tArtefactOrder2) const
	{
		return						(tArtefactOrder1.m_tpOrders[0].m_dwPrice > tArtefactOrder2.m_tpOrders[0].m_dwPrice);
	};
};

IC const ALife::_EVENT_ID			tfChooseEventKeyPredicate			(const CSE_ALifeEvent *T)
{
	return							(T->m_tEventID);
};

IC const ALife::_TASK_ID			tfChooseTaskKeyPredicate			(const CSE_ALifeTask *T)
{
	return							(T->m_tTaskID);
};

class CCompareTraderRanksPredicate {
public:
	bool							operator()							(const CSE_ALifeTrader *tpALifeTrader1, const CSE_ALifeTrader *tpALifeTrader2) const
	{
		return						(tpALifeTrader1->m_tRank < tpALifeTrader2->m_tRank);
	};
};

class CRemovePersonalEventPredicate {
private:
	ALife::EVENT_MAP				*m_tpMap;
public:
									CRemovePersonalEventPredicate		(ALife::EVENT_MAP &tpMap)
	{
		m_tpMap						= &tpMap;
	};

	IC bool							operator()							(const CSE_ALifePersonalEvent *tPersonalEvent)  const
	{
		return						(m_tpMap->find(tPersonalEvent->m_tEventID) == m_tpMap->end());
	};
};

class CSortItemPredicate {
private:
	ALife::D_OBJECT_MAP				*m_tpMap;
public:
	IC bool							operator()							(const CSE_ALifeItem *tpALifeItem1, const CSE_ALifeItem *tpALifeItem2)  const
	{
		return						(float(tpALifeItem1->m_dwCost)/1/**tpALifeItem1->m_fMass/**/ > float(tpALifeItem2->m_dwCost)/1/**tpALifeItem2->m_fMass/**/);
	};
};

#endif
