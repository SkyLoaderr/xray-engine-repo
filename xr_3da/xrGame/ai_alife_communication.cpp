#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"

#define KEEP_SORTED

#define PUSH_STACK(a,b,c,e,f) {\
	(e)[(f)].i1 = (a);\
	(e)[(f)].i2 = (b);\
	(e)[(f)].iCurrentSum = (c);\
	(f)++;\
}

#define POP_STACK(a,b,c,e,f) {\
	(f)--;\
	(a) = (e)[(f)].i1;\
	(b) = (e)[(f)].i2;\
	(c) = (e)[(f)].iCurrentSum;\
}

void CSE_ALifeSimulator::vfGenerateSums	(INT_VECTOR &tpTrader, INT_VECTOR &tpSums)
{
	int					l_iStackPointer = 0;
	tpSums.clear		();
	for (int i=0, n=tpTrader.size(); i<n; i++) {

		PUSH_STACK		(i+1,n-1,0,m_tpStack1,l_iStackPointer);

		for (;l_iStackPointer;) {
			int			i1,i2,iCurrentSum;

			POP_STACK	(i1,i2,iCurrentSum,m_tpStack1,l_iStackPointer);

			if (!i1) {
#ifdef KEEP_SORTED
				INT_IT I = std::lower_bound(tpSums.begin(),tpSums.end(),iCurrentSum);
				if ((I == tpSums.end()) || (*I != iCurrentSum)) {
					tpSums.insert	(I,iCurrentSum);
					if (tpSums.size() >= SUM_COUNT_THRESHOLD)
						return;
				}
#else
				INT_IT I = std::find(tpSums.begin(),tpSums.end(),iCurrentSum);
				if (I == tpSums.end()) {
					tpSums.push_back(iCurrentSum);
					if (tpSums.size() >= SUM_COUNT_THRESHOLD) {
						sort		(tpSums.begin(),tpSums.end());
						return;
					}
				}
#endif
				continue;
			}

			for (int i=i2; i>=0; i--)
				PUSH_STACK	(i1 - 1, i - 1, iCurrentSum + tpTrader[i],m_tpStack1,l_iStackPointer);
		}
	}
#ifndef KEEP_SORTED
	sort					(tpSums.begin(),tpSums.end());
#endif
}

bool CSE_ALifeSimulator::bfGetItemIndexes	(INT_VECTOR &tpTrader, int iSum1, INT_VECTOR &tpIndexes, SSumStackCell *tpStack, int iStartI, int iStackPointer)
{
	for (int j=iStartI, n=tpTrader.size(); j<n; j++) {

		PUSH_STACK			(j+1,n-1,0,tpStack,iStackPointer);
		tpIndexes.resize	(j + 2);

		for (;iStackPointer;) {
			int				i1,i2,iCurrentSum;

			POP_STACK		(i1,i2,iCurrentSum,tpStack,iStackPointer);

			tpIndexes[i1]	= i2 + 1;

			if (!i1) {
				if (iCurrentSum == iSum1) {
					tpIndexes.resize(j + 1);
					return	(true);
				}
				continue;
			}

			if (iCurrentSum >= iSum1)
				continue;

			for (int i=i2; i>=0; i--)
				PUSH_STACK	(i1 - 1, i - 1, iCurrentSum + tpTrader[i],tpStack,iStackPointer);
		}
	}
	return					(false);
}

bool CSE_ALifeSimulator::bfCheckInventoryCapacity(INT_VECTOR &tpTrader1, int iSum1, int iMoney1, INT_VECTOR &tpTrader2, int iSum2, int iMoney2, int iBallance)
{
	INT_VECTOR				l_tpIndexes1, l_tpIndexes2;
	int						l_iStartI1 = 0, l_iStackPointer1 = 0, l_iStartI2 = 0, l_iStackPointer2 = 0;
	for (;;) {
		l_tpIndexes1.clear	();
#ifdef ALIFE_LOG
		string4096			S;
		char				*S1 = S;	
		S1					+= sprintf(S1,"Trader1 -> ");
#endif
		if (iSum1) {
			if (!bfGetItemIndexes(tpTrader1,iSum1,l_tpIndexes1,m_tpStack1,l_iStartI1,l_iStackPointer1))
				return		(false);
#ifdef ALIFE_LOG
			for (int i=0, n=l_tpIndexes1.size(); i<n ;i++)
				S1			+= sprintf(S1,"%3d",l_tpIndexes1[i]);
#endif
		}
#ifdef ALIFE_LOG
		if (iSum1 < iBallance + iSum2)
			S1				+= sprintf(S1," + $%d",iBallance + iSum2 - iSum1);

		for (;;) {
			l_tpIndexes2.clear();
			S1				+= sprintf(S1,"\nTrader2 -> ");
#endif
			if (iSum2) {
				if (!bfGetItemIndexes(tpTrader2,iSum2,l_tpIndexes2,m_tpStack2,l_iStartI2,l_iStackPointer2))
					return	(false);
#ifdef ALIFE_LOG
				S1			+= sprintf(S1,"\nTrader2 -> ");
				for (int i=0, n=l_tpIndexes2.size(); i<n ;i++)
					S1		+= sprintf(S1,"%3d",l_tpIndexes2[i]);
#endif
			}
#ifdef ALIFE_LOG
			if (iSum1 > iBallance + iSum2)
				S1			+= sprintf(S1," + $%d",iSum1 - iBallance - iSum2);
			Msg				("%s",S1);
#endif
			return			(true);
		}
	}
//	return					(false);
}

bool CSE_ALifeSimulator::bfCheckForTrade	(INT_VECTOR &tpTrader1, INT_VECTOR &tpSums1, int iMoney1, INT_VECTOR &tpTrader2, INT_VECTOR	 &tpSums2, int iMoney2, int iBallance)
{
	INT_IT					I	= tpSums1.begin();
	INT_IT					E	= tpSums1.end();
	INT_IT					II	= tpSums2.begin();
	INT_IT					EE	= tpSums2.end();

	for ( ; I != E; I++) {
		if (*I + iMoney1 < iBallance)
			continue;

		if (*I < iBallance) {
			if (bfCheckInventoryCapacity(tpTrader1,*I,iMoney1,tpTrader2,0,iMoney2,iBallance))
				break;
		}
		else {
			bool			l_bOk = false;
			II				= tpSums2.begin();
			for ( ; II != EE; II++)
				if (*I >= *II + iBallance)
					if (*I - *II - iBallance <= iMoney2) {
						if (bfCheckInventoryCapacity(tpTrader1,*I,iMoney1,tpTrader2,*II,iMoney2,iBallance)) {
							l_bOk = true;
							break;
						}
					}
					else
						continue;
				else
					if (*I + iMoney1 >= *II + iBallance) {
						if (bfCheckInventoryCapacity(tpTrader1,*I,iMoney1,tpTrader2,*II,iMoney2,iBallance)) {
							l_bOk = true;
							break;
						}
					}
					else
						break;
			if (l_bOk)
				break;
		}
	}

	if (I == E) {
#ifdef ALIFE_LOG
		Msg					("Can't trade!\n");
#endif
		return				(false);
	}
	else
		return				(true);
}

void CSE_ALifeSimulator::vfNullTradersBallance(CSE_ALifeHumanAbstract *tpALifeHumanAbstract1, CSE_ALifeHumanAbstract *tpALifeHumanAbstract2, int iBallance)
{
	m_tpTrader1.clear	();
	m_tpTrader2.clear	();
	m_tpSums1.clear		();
	m_tpSums2.clear		();

	int					l_iMoney1 = tpALifeHumanAbstract1->m_dwMoney;
	int					l_iMoney2 = tpALifeHumanAbstract2->m_dwMoney;
	int					l_iBallance = iBallance;
	if ((l_iMoney1 >= l_iBallance) && (l_iMoney2 >= l_iBallance)) {
		return;
	}

	sort				(m_tpTrader1.begin(),m_tpTrader1.end());
	sort				(m_tpTrader2.begin(),m_tpTrader2.end());

#ifdef ALIFE_LOG
	{
		Msg				("%s [%3d]: ",tpALifeHumanAbstract1->s_name_replace,l_iMoney1);
		for (int i=0, n=m_tpTrader1.size(); i<n; i++)
			Msg			("%4d",m_tpTrader1[i]);
		Msg				("\n");
	}
	{
		Msg				("%s [%3d]: ",tpALifeHumanAbstract2->s_name_replace,l_iMoney2);
		for (int i=0, n=m_tpTrader2.size(); i<n; i++)
			Msg			("%4d",m_tpTrader2[i]);
		Msg				("\n");
	}
	Msg					("Ballance : %3d\n",l_iBallance);
#endif
	vfGenerateSums		(m_tpTrader1,m_tpSums1);
	vfGenerateSums		(m_tpTrader2,m_tpSums2);
	m_tpSums1.insert	(m_tpSums1.begin(),0);
	m_tpSums2.insert	(m_tpSums2.begin(),0);
#ifdef ALIFE_LOG
	{	
		Msg				("%s : ",tpALifeHumanAbstract1->s_name_replace);
		INT_IT			I = m_tpSums1.begin();
		INT_IT			E = m_tpSums1.end();
		for ( ; I != E; I++)
			Msg			("%6d",*I);
		Msg				("\n");
	}
	{
		Msg				("%s : ",tpALifeHumanAbstract2->s_name_replace);
		INT_IT			I = m_tpSums2.begin();
		INT_IT			E = m_tpSums2.end();
		for ( ; I != E; I++)
			Msg			("%6d",*I);
		Msg				("\n");
	}
#endif
	if (true)
		bfCheckForTrade	(m_tpTrader1, m_tpSums1, l_iMoney1, m_tpTrader2, m_tpSums2, l_iMoney2, l_iBallance);
	else
		bfCheckForTrade	(m_tpTrader2, m_tpSums2, l_iMoney2, m_tpTrader1, m_tpSums1, l_iMoney1, l_iBallance);
	Msg					("\n");
}

void CSE_ALifeSimulator::vfCommunicateWithCustomer(CSE_ALifeHumanAbstract *tpALifeHumanAbstract, CSE_ALifeTraderAbstract *tpTraderAbstract)
{
	// update items
	if (tpfGetTaskByID(tpALifeHumanAbstract->m_dwCurTaskID,true)) {
		OBJECT_IT								I;
		if (tpALifeHumanAbstract->bfCheckIfTaskCompleted(I)) {
			D_OBJECT_PAIR_IT					J = m_tObjectRegistry.find(*I);
			R_ASSERT2							(J != m_tObjectRegistry.end(), "Specified object hasn't been found in the Object registry!");
			CSE_ALifeInventoryItem				*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>((*J).second);
			R_ASSERT2							(l_tpALifeInventoryItem,"Non inventory item has a parent?!");
			if (tpTraderAbstract->m_dwMoney >= l_tpALifeInventoryItem->m_dwCost) {
				vfDetachItem					(*tpALifeHumanAbstract,l_tpALifeInventoryItem,tpALifeHumanAbstract->m_tGraphID);
				vfAttachItem					(*tpTraderAbstract,l_tpALifeInventoryItem,tpALifeHumanAbstract->m_tGraphID);
				// paying/receiving money
				tpTraderAbstract->m_dwMoney		-= l_tpALifeInventoryItem->m_dwCost;
				tpALifeHumanAbstract->m_dwMoney += l_tpALifeInventoryItem->m_dwCost;
			}
		}
	}
	// update events
#pragma todo("Dima to Dima: Update events")
}

void CSE_ALifeSimulator::vfPerformCommunication(CSE_ALifeHumanAbstract *tpALifeHumanAbstract1, CSE_ALifeHumanAbstract *tpALifeHumanAbstract2)
{
#pragma todo("Dima to Dima: Append communication")



	// update events
#pragma todo("Dima to Dima: Update events")
}