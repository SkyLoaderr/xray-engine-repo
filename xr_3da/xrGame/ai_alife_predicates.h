////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_predicates.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life predicates
////////////////////////////////////////////////////////////////////////////

#pragma once

IC ALife::_EVENT_ID					tfChooseEventKeyPredicate			(const CSE_ALifeEvent *T)
{
	return							(T->m_tEventID);
};

IC ALife::_TASK_ID					tfChooseTaskKeyPredicate			(const CSE_ALifeTask *T)
{
	return							(T->m_tTaskID);
};

IC LPCSTR							cafChooseDiscoveryKeyPredicate		(const CSE_ALifeDiscovery *T)
{
	return							(T->m_caDiscoveryIdentifier);
};

IC LPCSTR							cafChooseOrganizationKeyPredicate	(const CSE_ALifeOrganization *T)
{
	return							(T->m_caOrganizationIdentifier);
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
	ALife::OBJECT_MAP				*m_tpMap;
public:
									CSortItemPredicate					(ALife::OBJECT_MAP &tpMap)
	{
		m_tpMap						= &tpMap;
	};

	IC bool							operator()							(const ALife::_OBJECT_ID &tObjectID1, const ALife::_OBJECT_ID &tObjectID2)  const
	{
		ALife::OBJECT_PAIR_IT		it1 = m_tpMap->find(tObjectID1);
		VERIFY						(it1 != m_tpMap->end());
		ALife::OBJECT_PAIR_IT		it2 = m_tpMap->find(tObjectID2);
		VERIFY						(it2 != m_tpMap->end());
		CSE_ALifeItem				*tpItem1 = dynamic_cast<CSE_ALifeItem *>((*it1).second);
		VERIFY						(tpItem1);
		CSE_ALifeItem				*tpItem2 = dynamic_cast<CSE_ALifeItem *>((*it2).second);
		VERIFY						(tpItem2);
		return						(float(tpItem1->m_dwCost)/tpItem1->m_fMass > float(tpItem2->m_dwCost)/tpItem2->m_fMass);
	};
};
