////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_predicates.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life predicates
////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace ALife;

IC _EVENT_ID tfChooseEventKeyPredicate(const CALifeEvent *T)
{
	return(T->m_tEventID);
};

IC _TASK_ID tfChooseTaskKeyPredicate(const CALifeTask *T)
{
	return(T->m_tTaskID);
};

class CCompareTraderRanksPredicate {
public:
	bool operator()(const CALifeTrader *tpALifeTrader1, const CALifeTrader *tpALifeTrader2) const
	{
		return(tpALifeTrader1->m_tRank < tpALifeTrader2->m_tRank);
	};
};

class CRemovePersonalEventPredicate {
private:
	EVENT_MAP						*m_tpMap;
public:
	CRemovePersonalEventPredicate	(EVENT_MAP &tpMap)
	{
		m_tpMap = &tpMap;
	};

	IC bool operator()(const CALifePersonalEvent *tPersonalEvent)  const
	{
		return(m_tpMap->find(tPersonalEvent->m_tEventID) == m_tpMap->end());
	};
};

class CSortItemPredicate {
private:
	OBJECT_MAP						*m_tpMap;
public:
	CSortItemPredicate				(OBJECT_MAP &tpMap)
	{
		m_tpMap = &tpMap;
	};

	IC bool operator()(const _OBJECT_ID &tObjectID1, const _OBJECT_ID &tObjectID2)  const
	{
		OBJECT_PAIR_IT it1 = m_tpMap->find(tObjectID1);
		VERIFY(it1 != m_tpMap->end());
		OBJECT_PAIR_IT it2 = m_tpMap->find(tObjectID2);
		VERIFY(it2 != m_tpMap->end());
		CALifeItem *tpItem1 = dynamic_cast<CALifeItem *>((*it1).second);
		VERIFY(tpItem1);
		CALifeItem *tpItem2 = dynamic_cast<CALifeItem *>((*it2).second);
		VERIFY(tpItem2);
		return(float(tpItem1->m_dwCost)/tpItem1->m_fMass > float(tpItem2->m_dwCost)/tpItem2->m_fMass);
	};
};
