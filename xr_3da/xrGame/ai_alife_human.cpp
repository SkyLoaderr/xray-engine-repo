////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation of humans
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"
#include "ai_primary_funcs.h"

#define MAX_ITEM_FOOD_COUNT		3
#define MAX_ITEM_MEDIKIT_COUNT	3
#define MAX_AMMO_ATTACH_COUNT	10

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted(OBJECT_IT &I)
{
	if (int(m_dwCurTaskID) < 0)
		return		(false);
	I				= children.begin();
	OBJECT_IT		E = children.end();
	CSE_ALifeTask	&tTask = *m_tpALife->tpfGetTaskByID(m_dwCurTaskID);
	for ( ; I != E; I++) {
		switch (tTask.m_tTaskType) {
			case eTaskTypeSearchForItemCL :
			case eTaskTypeSearchForItemCG : {
				if (!strcmp(m_tpALife->tpfGetObjectByID(*I)->s_name,tTask.m_caSection))
					return(true);
				break;
											}
			case eTaskTypeSearchForItemOL :
			case eTaskTypeSearchForItemOG : {
				if (m_tpALife->tpfGetObjectByID(*I)->ID == tTask.m_tObjectID)
					return(true);
				break;
											}
		}
	}
	return			(false);
}

bool CSE_ALifeHumanAbstract::bfCheckIfTaskCompleted	()
{
	OBJECT_IT					I;
	return						(bfCheckIfTaskCompleted(I));
}

void CSE_ALifeHumanAbstract::vfSetCurrentTask(_TASK_ID &tTaskID)
{
	m_dwCurTaskID				= m_tpALife->tpfGetTaskByID(tTaskID)->m_tTaskID;
}

bool CSE_ALifeHumanAbstract::bfChooseNextRoutePoint()
{
	bool			bContinue = false;
	if (m_tTaskState != eTaskStateSearchItem) {
		if (m_tNextGraphID != m_tGraphID) {
			_TIME_ID				tCurTime = m_tpALife->tfGetGameTime();
			m_fDistanceFromPoint	+= float(tCurTime - m_tTimeID)/1000.f*m_fCurSpeed;
			if (m_fDistanceToPoint - m_fDistanceFromPoint < EPS_L) {
				bContinue			= true;
				if ((m_fDistanceFromPoint - m_fDistanceToPoint > EPS_L) && (m_fCurSpeed > EPS_L))
					m_tTimeID		= tCurTime - _TIME_ID(iFloor((m_fDistanceFromPoint - m_fDistanceToPoint)*1000.f/m_fCurSpeed));
				m_fDistanceToPoint	= m_fDistanceFromPoint	= 0.0f;
				m_tPrevGraphID		= m_tGraphID;
				m_tpALife->vfChangeObjectGraphPoint(this,m_tGraphID,m_tNextGraphID);
			}
		}
		else {
			if (++m_dwCurNode < m_tpPath.size()) {
				m_tNextGraphID		= _GRAPH_ID(m_tpPath[m_dwCurNode]);
				m_fDistanceToPoint	= getAI().ffGetDistanceBetweenGraphPoints(m_tGraphID,m_tNextGraphID);
				bContinue = true;
			}
			else
				m_fCurSpeed	= 0.f;
		}
	}
	else {
	}
	return			(bContinue);
}

void CSE_ALifeHumanAbstract::vfCheckForDeletedEvents()
{
	PERSONAL_EVENT_P_IT I = std::remove_if(m_tpEvents.begin(),m_tpEvents.end(),CRemovePersonalEventPredicate(m_tpALife->m_tEventRegistry));
	m_tpEvents.erase	(I,m_tpEvents.end());
}

void CSE_ALifeHumanAbstract::vfChooseHumanTask()
{
	OBJECT_IT					I = m_tpKnownCustomers.begin();
	OBJECT_IT					E = m_tpKnownCustomers.end();
	for ( ; I != E; I++) {
		OBJECT_TASK_PAIR_IT		J = m_tpALife->m_tTaskCrossMap.find(*I);
		R_ASSERT2				(J != m_tpALife->m_tTaskCrossMap.end(),"Can't find a specified customer in the Task registry!");
		
		TASK_SET_IT				i = (*J).second.begin();
		TASK_SET_IT				e = (*J).second.end();
		for ( ; i != e; i++)
			if (!m_tpALife->tpfGetTaskByID(*i)->m_dwTryCount)
				break;
		
		if (i != e) {
			vfSetCurrentTask	(*i);
			break;
		}
	}
}

CSE_ALifeItemWeapon	*CSE_ALifeHumanAbstract::tpfGetBestWeapon(EHitType &tHitType, float &fHitPower)
{
	fHitPower					= 0.f;
	CSE_ALifeItemWeapon			*l_tpBestWeapon = 0;
	u32							l_dwBestWeapon = 0;
	OBJECT_IT					I = children.begin();
	OBJECT_IT					E = children.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemWeapon		*l_tpALifeItemWeapon = dynamic_cast<CSE_ALifeItemWeapon*>(m_tpALife->tpfGetObjectByID(*I));
		if (!l_tpALifeItemWeapon)
			continue;

		l_tpALifeItemWeapon->m_dwAmmoAvailable = get_available_ammo_count(l_tpALifeItemWeapon,children);
		if (l_tpALifeItemWeapon->m_dwAmmoAvailable || (l_tpALifeItemWeapon->m_dwSlot == 0) || (l_tpALifeItemWeapon->m_dwSlot == 3)) {
			u32					l_dwCurrentBestWeapon = 0; 
			switch (l_tpALifeItemWeapon->m_tClassID) {
				case CLSID_OBJECT_W_RPG7:
				case CLSID_OBJECT_W_M134: {
					l_dwCurrentBestWeapon = 9;
					break;
				}
				case CLSID_OBJECT_W_FN2000:
				case CLSID_OBJECT_W_SVD:
				case CLSID_OBJECT_W_SVU:
				case CLSID_OBJECT_W_VINTOREZ: {
					l_dwCurrentBestWeapon = 8;
					break;
				}
				case CLSID_OBJECT_W_SHOTGUN:
				case CLSID_OBJECT_W_AK74:
				case CLSID_OBJECT_W_VAL:
				case CLSID_OBJECT_W_LR300:		{
					l_dwCurrentBestWeapon = 6;
					break;
				}
				case CLSID_OBJECT_W_HPSA:		
				case CLSID_OBJECT_W_PM:			
				case CLSID_OBJECT_W_FORT:		
				case CLSID_OBJECT_W_WALTHER:	
				case CLSID_OBJECT_W_USP45:		{
					l_dwCurrentBestWeapon = 5;
					break;
				}
				case CLSID_OBJECT_W_KNIFE: {
					l_dwCurrentBestWeapon = 1;
					break;
				}
				default : NODEFAULT;
			}
			if (l_dwCurrentBestWeapon > l_dwBestWeapon) {
				l_dwBestWeapon = l_dwCurrentBestWeapon;
				l_tpBestWeapon = l_tpALifeItemWeapon;
			}
		}
	}
	if (l_tpBestWeapon) {
		fHitPower				= l_tpBestWeapon->m_fHitPower;
		tHitType				= l_tpBestWeapon->m_tHitType;
	}
	return						(l_tpBestWeapon);
}

bool CSE_ALifeHumanAbstract::bfPerformAttack()
{
	switch (m_tpCurrentBestWeapon->m_dwSlot) {
		case 0 :
			return						(true);
		case 3 : {
			bool						l_bOk = false;
			OBJECT_IT					I = children.begin();
			OBJECT_IT					E = children.end();
			for ( ; I != E; I++)
				if (*I == m_tpCurrentBestWeapon->ID) {
					l_bOk				= true;
					CSE_ALifeItem		*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(m_tpALife->tpfGetObjectByID(*I));
					m_tpALife->vfDetachItem(*this,l_tpALifeItem,m_tGraphID);
					m_tpALife->vfReleaseObject(l_tpALifeItem,true);
					break;
				}
			R_ASSERT2					(l_bOk,"Cannot find specified weapon in the inventory");
			return						(false);
		}
		default : {
			R_ASSERT2					(m_tpCurrentBestWeapon->m_dwAmmoAvailable,"No ammo for the selected weapon!");
			m_tpCurrentBestWeapon->m_dwAmmoAvailable--;
			if (m_tpCurrentBestWeapon->m_dwAmmoAvailable)
				return					(true);

			for (int i=0, n=children.size() ; i<n; i++) {
				CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[i]));
				if (l_tpALifeItemAmmo && strstr(m_tpCurrentBestWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name) && l_tpALifeItemAmmo->a_elapsed) {
					m_tpALife->vfDetachItem(*this,l_tpALifeItemAmmo,m_tGraphID);
					m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
					i--;
					n--;
				}
			}
			m_tpCurrentBestWeapon		= 0;
			return						(false);
		}
	}
}

void CSE_ALifeHumanAbstract::vfUpdateWeaponAmmo()
{
	if (!m_tpCurrentBestWeapon)
		return;

	switch (m_tpCurrentBestWeapon->m_dwSlot) {
		case 0 :
		case 3 :
			break;
		default : {
			for (int i=0, n=children.size() ; i<n; i++) {
				CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[i]));
				if (l_tpALifeItemAmmo && strstr(m_tpCurrentBestWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name)) {
					if (m_tpCurrentBestWeapon->m_dwAmmoAvailable > l_tpALifeItemAmmo->a_elapsed) {
						m_tpCurrentBestWeapon->m_dwAmmoAvailable	-= l_tpALifeItemAmmo->a_elapsed;
						continue;
					}
					if (m_tpCurrentBestWeapon->m_dwAmmoAvailable) {
						l_tpALifeItemAmmo->a_elapsed = u16(l_tpALifeItemAmmo->a_elapsed - u16(m_tpCurrentBestWeapon->m_dwAmmoAvailable));
						continue;
					}
					m_tpALife->vfDetachItem(*this,l_tpALifeItemAmmo,m_tGraphID);
					m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
					i--;
					n--;
				}
			}
			break;
		}
	}
	for (int i=0, n=children.size() ; i<n; i++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[i]));
		
		if (!l_tpALifeItemAmmo)
			continue;

		if (!l_tpALifeItemAmmo->a_elapsed) {
			m_tpALife->vfDetachItem(*this,l_tpALifeItemAmmo,m_tGraphID);
			m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
			i--;
			n--;
			continue;
		}

		for (int j=0; j<i; j++) {
			CSE_ALifeItemAmmo		*l_tpALifeItemAmmo1 = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[i]));
			if (!l_tpALifeItemAmmo1 || strcmp(l_tpALifeItemAmmo->s_name,l_tpALifeItemAmmo1->s_name) || (l_tpALifeItemAmmo1->a_elapsed == l_tpALifeItemAmmo1->m_boxSize))
				continue;
			u16						l_wDiff = l_tpALifeItemAmmo1->m_boxSize - l_tpALifeItemAmmo1->a_elapsed;
			if (l_wDiff >= l_tpALifeItemAmmo->a_elapsed) {
				l_tpALifeItemAmmo1->a_elapsed	= l_tpALifeItemAmmo1->a_elapsed + l_tpALifeItemAmmo->a_elapsed;
				l_tpALifeItemAmmo->a_elapsed	= 0;
				i--;
				break;
			}
			else {
				l_tpALifeItemAmmo1->a_elapsed	= l_tpALifeItemAmmo1->m_boxSize;
				l_tpALifeItemAmmo->a_elapsed	= l_tpALifeItemAmmo->a_elapsed - l_wDiff;
			}
		}
	}
}

u16	CSE_ALifeHumanAbstract::get_available_ammo_count(CSE_ALifeItemWeapon *tpALifeItemWeapon, OBJECT_VECTOR &tpObjectVector)
{
	if (!tpALifeItemWeapon->m_caAmmoSections)
		return(u16(-1));
	u32							l_dwResult = 0;
	OBJECT_IT					I = tpObjectVector.begin();
	OBJECT_IT					E = tpObjectVector.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(*I));
		if (l_tpALifeItemAmmo && strstr(tpALifeItemWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name))
			l_dwResult			+= l_tpALifeItemAmmo->a_elapsed;
	}
	return						(u16(l_dwResult));
}

u16	CSE_ALifeHumanAbstract::get_available_ammo_count(CSE_ALifeItemWeapon *tpALifeItemWeapon, ITEM_P_LIST &tpItemVector)
{
	if (!tpALifeItemWeapon->m_caAmmoSections)
		return(u16(-1));
	u32							l_dwResult = 0;
	ITEM_P_LIST_IT					I = tpItemVector.begin();
	ITEM_P_LIST_IT					E = tpItemVector.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(*I);
		if (l_tpALifeItemAmmo && strstr(tpALifeItemWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name))
			l_dwResult			+= l_tpALifeItemAmmo->a_elapsed;
	}
	return						(u16(l_dwResult));
}

void CSE_ALifeHumanAbstract::attach_available_ammo(CSE_ALifeItemWeapon *tpALifeItemWeapon, ITEM_P_LIST &tpItemVector)
{
	if (!tpALifeItemWeapon->m_caAmmoSections)
		return;
	u32							l_dwCount = 0;
	ITEM_P_LIST_IT					I = tpItemVector.begin();
	ITEM_P_LIST_IT					E = tpItemVector.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(*I);
		if (l_tpALifeItemAmmo && strstr(tpALifeItemWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name) && bfCanGetItem(l_tpALifeItemAmmo)) {
			m_tpALife->vfAttachItem(*this,l_tpALifeItemAmmo,m_tGraphID);
			l_dwCount++;
			if (l_dwCount > MAX_AMMO_ATTACH_COUNT)
				break;
		}
	}
}

void CSE_ALifeHumanAbstract::vfProcessItems()
{
	m_tpALife->m_tpItemList.clear();
	for (int I=0; I<(int)m_tpALife->m_tpGraphObjects[m_tGraphID].tpObjects.size(); I++) {
		CSE_ALifeItem			*tpALifeItem = dynamic_cast<CSE_ALifeItem *>(m_tpALife->m_tpGraphObjects[m_tGraphID].tpObjects[I]);
		if (tpALifeItem && !tpALifeItem->m_bOnline && (m_tpALife->randF(1.0f) < m_fProbability))
			m_tpALife->m_tpItemList.push_back(tpALifeItem);
	}
	if (!m_tpALife->m_tpItemList.empty())
		vfAttachItems();
}

void CSE_ALifeHumanAbstract::vfAttachItems(ETakeType tTakeType)
{
	CSE_ALifeAbstractGroup			*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(this);
	if (l_tpALifeAbstractGroup) {
		{
			OBJECT_IT					I = l_tpALifeAbstractGroup->m_tpMembers.begin();
			OBJECT_IT					E = l_tpALifeAbstractGroup->m_tpMembers.end();
			for ( ; I != E; I++) {
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(m_tpALife->tpfGetObjectByID(*I));
				R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
				l_tpALifeHumanAbstract->vfAttachItems(eTakeTypeMin);
			}
		}
		{
			OBJECT_IT					I = l_tpALifeAbstractGroup->m_tpMembers.begin();
			OBJECT_IT					E = l_tpALifeAbstractGroup->m_tpMembers.end();
			for ( ; I != E; I++) {
				CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(m_tpALife->tpfGetObjectByID(*I));
				R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
				l_tpALifeHumanAbstract->vfAttachItems(eTakeTypeRest);
			}
		}
		return;
	}
	
	if (tTakeType == eTakeTypeAll) {
		m_tpALife->vfAppendItemList		(children,m_tpALife->m_tpItemList);
		for (int i=0, n=children.size(); i<n; n--) {
			CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(m_tpALife->tpfGetObjectByID(children[i]));
			R_ASSERT2				(l_tpALifeItem,"Invalid inventory object");
			m_tpALife->vfDetachItem	(*this,l_tpALifeItem,m_tGraphID);
		}
		R_ASSERT2						((m_fCumulativeItemMass < EPS_L) && (m_iCumulativeItemVolume < EPS_L),"Invalid cumulative item mass or volume value");
		m_fCumulativeItemMass			= 0.f;
		m_iCumulativeItemVolume			= 0;
	}
	
	CSE_ALifeItem					*l_tpALifeItemBest;
	float							l_fItemBestValue;
	
	getAI().m_tpCurrentALifeMember	= this;
	if ((tTakeType == eTakeTypeAll) || (tTakeType == eTakeTypeMin)) {
		// choosing equipment
		l_tpALifeItemBest				= 0;
		l_fItemBestValue				= -1.f;
		{
			ITEM_P_LIST_IT				I = m_tpALife->m_tpItemList.begin(), X;
			ITEM_P_LIST_IT				E = m_tpALife->m_tpItemList.end();
			for ( ; I != E; I++) {
				// checking if it is an item
				CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(*I);
				R_ASSERT2				(l_tpALifeItem,"Non-item object!");
				if (!l_tpALifeItem)
					continue;
				// checking if it is an equipment item
				getAI().m_tpCurrentALifeObject = l_tpALifeItem;
				if (getAI().m_pfEquipmentType->ffGetValue() > getAI().m_pfEquipmentType->ffGetMaxResultValue())
					continue;
				// evaluating item
				float					l_fCurrentValue = getAI().m_pfEquipmentType->ffGetValue();
				// choosing the best item
				if ((l_fCurrentValue > l_fItemBestValue) && bfCanGetItem(l_tpALifeItem)) {
					l_fItemBestValue = l_fCurrentValue;
					l_tpALifeItemBest = l_tpALifeItem;
					X = I;
				}
			}
			if (l_tpALifeItemBest) {
				m_tpALife->vfAttachItem	(*this,l_tpALifeItemBest,m_tGraphID);
				m_tpALife->m_tpItemList.erase(X);
			}
		}
		
		// choosing weapon
		for (int i=0; i<4; i++) {
			l_tpALifeItemBest				= 0;
			l_fItemBestValue				= -1.f;
			{
				ITEM_P_LIST_IT				I = m_tpALife->m_tpItemList.begin();
				ITEM_P_LIST_IT				E = m_tpALife->m_tpItemList.end();
				for ( ; I != E; I++) {
					// checking if it is an item
					CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(*I);
					if (!l_tpALifeItem)
						continue;
					// checking if it is a hand weapon
					getAI().m_tpCurrentALifeObject = l_tpALifeItem;
					int						j = getAI().m_pfPersonalWeaponType->dwfGetWeaponType();
					float					l_fCurrentValue = -1.f;
					switch (i) {
						case 0 : {
							if (j != 1)
								continue;
							l_fCurrentValue = getAI().m_pfItemValue->ffGetValue();
							break;
						}
						case 1 : {
							if (j != 5)
								continue;
							l_fCurrentValue = getAI().m_pfSmallWeaponValue->ffGetValue();
							break;
						}
						case 2 : {
							if ((j != 6) && (j != 8) && (j != 9))
								continue;
							l_fCurrentValue = getAI().m_pfMainWeaponValue->ffGetValue();
							break;
						}
						case 3 : {
							if (j != 7)
								continue;
							l_fCurrentValue = getAI().m_pfItemValue->ffGetValue();
							break;
						}
					}
					// choosing the best item
					if ((l_fCurrentValue > l_fItemBestValue) && bfCanGetItem(l_tpALifeItem)) {
						l_fItemBestValue = l_fCurrentValue;
						l_tpALifeItemBest = l_tpALifeItem;
					}
				}
				if (l_tpALifeItemBest) {
					m_tpALife->vfAttachItem	(*this,l_tpALifeItemBest,m_tGraphID);
					attach_available_ammo	(dynamic_cast<CSE_ALifeItemWeapon*>(l_tpALifeItemBest),m_tpALife->m_tpItemList);
					ITEM_P_LIST_IT			I = remove_if(m_tpALife->m_tpItemList.begin(),m_tpALife->m_tpItemList.end(),CRemoveAttachedItemsPredicate());
					m_tpALife->m_tpItemList.erase(I,m_tpALife->m_tpItemList.end());
				}
			}
		}
		
#pragma todo("Add food and medikit items support")
		m_tpALife->m_tpItemList.sort	(CSortItemPredicate());
		
		// choosing food
		{
			u32							l_dwCount = 0;
			ITEM_P_LIST_IT				I = m_tpALife->m_tpItemList.begin();
			ITEM_P_LIST_IT				E = m_tpALife->m_tpItemList.end();
			for ( ; I != E; I++) {
				CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(*I);
				if (!l_tpALifeItem || (l_tpALifeItem->m_iFoodValue <= 0))
					continue;
				if (bfCanGetItem(l_tpALifeItem)) {
					m_tpALife->vfAttachItem	(*this,l_tpALifeItem,m_tGraphID);
					l_dwCount++;
					if (l_dwCount > MAX_ITEM_FOOD_COUNT)
						break;
				}
			}
			if (l_dwCount) {
				ITEM_P_LIST_IT			I = remove_if(m_tpALife->m_tpItemList.begin(),m_tpALife->m_tpItemList.end(),CRemoveAttachedItemsPredicate());
				m_tpALife->m_tpItemList.erase(I,m_tpALife->m_tpItemList.end());
			}
		}
		
		// choosing medikits
		{
			u32							l_dwCount = 0;
			ITEM_P_LIST_IT				I = m_tpALife->m_tpItemList.begin();
			ITEM_P_LIST_IT				E = m_tpALife->m_tpItemList.end();
			for ( ; I != E; I++) {
				CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(*I);
				if (!l_tpALifeItem || (l_tpALifeItem->m_iHealthValue <= 0))
					continue;
				if (bfCanGetItem(l_tpALifeItem)) {
					m_tpALife->vfAttachItem	(*this,l_tpALifeItem,m_tGraphID);
					l_dwCount++;
					if (l_dwCount > MAX_ITEM_MEDIKIT_COUNT)
						break;
				}
			}
			if (l_dwCount) {
				ITEM_P_LIST_IT			I = remove_if(m_tpALife->m_tpItemList.begin(),m_tpALife->m_tpItemList.end(),CRemoveAttachedItemsPredicate());
				m_tpALife->m_tpItemList.erase(I,m_tpALife->m_tpItemList.end());
			}
		}
		// choosing detector
		l_tpALifeItemBest				= 0;
		l_fItemBestValue				= -1.f;
		{
			ITEM_P_LIST_IT				I = m_tpALife->m_tpItemList.begin(), X;
			ITEM_P_LIST_IT				E = m_tpALife->m_tpItemList.end();
			for ( ; I != E; I++) {
				// checking if it is an item
				CSE_ALifeItemDetector	*l_tpALifeItem = dynamic_cast<CSE_ALifeItemDetector*>(*I);
				if (!l_tpALifeItem)
					continue;
				// evaluating item
				getAI().m_tpCurrentALifeObject = l_tpALifeItem;
				float					l_fCurrentValue = getAI().m_pfEquipmentType->ffGetValue();
				// choosing the best item
				if ((l_fCurrentValue > l_fItemBestValue) && bfCanGetItem(l_tpALifeItem)) {
					l_fItemBestValue = l_fCurrentValue;
					l_tpALifeItemBest = l_tpALifeItem;
					X = I;
				}
			}
			if (l_tpALifeItemBest) {
				m_tpALife->vfAttachItem	(*this,l_tpALifeItemBest,m_tGraphID);
				m_tpALife->m_tpItemList.erase(X);
			}
		}
	}

	if ((tTakeType == eTakeTypeAll) || (tTakeType == eTakeTypeRest)) {
		// choosing the rest objects
		if (tTakeType == eTakeTypeRest)
			m_tpALife->m_tpItemList.sort(CSortItemPredicate());
		ITEM_P_LIST_IT				I = m_tpALife->m_tpItemList.begin();
		ITEM_P_LIST_IT				E = m_tpALife->m_tpItemList.end();
		for ( ; I != E; I++) {
			CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(*I);
			if (!l_tpALifeItem)
				continue;
			if (bfCanGetItem(l_tpALifeItem))
				m_tpALife->vfAttachItem	(*this,l_tpALifeItem,m_tGraphID);
		}
		I							= remove_if(m_tpALife->m_tpItemList.begin(),m_tpALife->m_tpItemList.end(),CRemoveAttachedItemsPredicate());
		m_tpALife->m_tpItemList.erase(I,m_tpALife->m_tpItemList.end());
	}
}

bool CSE_ALifeHumanAbstract::bfCanGetItem(CSE_ALifeItem *tpALifeItem)
{
	if ((m_fCumulativeItemMass + tpALifeItem->m_fMass > m_fMaxItemMass) || (m_iCumulativeItemVolume + tpALifeItem->m_iVolume > MAX_ITEM_VOLUME))
		return		(false);
	
	m_tpTempItemBuffer.resize(children.size() + 1);
	
	{
		OBJECT_IT	i = children.begin();
		OBJECT_IT	e = children.end();
		ITEM_P_IT	I = m_tpTempItemBuffer.begin();
		for ( ; i != e; i++, I++)
			*I		= dynamic_cast<CSE_ALifeItem*>(m_tpALife->tpfGetObjectByID(*i));
		*I			= tpALifeItem;
	}

	m_tpALife->m_tpWeaponVector.assign(m_tpALife->m_tpWeaponVector.size(),0);
	{
		ITEM_P_IT		I = m_tpTempItemBuffer.begin();
		ITEM_P_IT		E = m_tpTempItemBuffer.end();
		for ( ; I != E; I++) {
			CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = dynamic_cast<CSE_ALifeItemWeapon*>(*I);
			if (l_tpALifeItemWeapon && (!m_tpALife->m_tpWeaponVector[l_tpALifeItemWeapon->m_dwSlot] || (m_tpALife->m_tpWeaponVector[l_tpALifeItemWeapon->m_dwSlot]->m_iVolume < l_tpALifeItemWeapon->m_iVolume)))
				m_tpALife->m_tpWeaponVector[l_tpALifeItemWeapon->m_dwSlot] = l_tpALifeItemWeapon;
		}
	}
	{
		ITEM_P_IT		I = remove_if(m_tpTempItemBuffer.begin(),m_tpTempItemBuffer.end(),CRemoveSlotAndCellItemsPredicate(&m_tpALife->m_tpWeaponVector,6));
		m_tpTempItemBuffer.erase(I,m_tpTempItemBuffer.end());
	}

	sort			(m_tpTempItemBuffer.begin(),m_tpTempItemBuffer.end(),CSortItemVolumePredicate());

#pragma todo("Dima to Dima : Instead of greeding algorithm implement faster algorithm which _always_ computes _correct_ result")
	u64				l_qwInventoryBitMask = 0;
	ITEM_P_IT		I = m_tpTempItemBuffer.begin();
	ITEM_P_IT		E = m_tpTempItemBuffer.end();
	for ( ; I != E; I++) {
		bool		l_bOk = true;
		u64			l_qwItemBitMask = (*I)->m_qwGridBitMask;
		for (int i=0, n=RUCK_HEIGHT - (*I)->m_iGridHeight, m=RUCK_WIDTH - (*I)->m_iGridWidth; i<=n && l_bOk; i++, l_qwItemBitMask >>= RUCK_WIDTH - m)
			for (int j=0; j<=m; j++, l_qwItemBitMask >>= 1)
				if ((l_qwInventoryBitMask ^ l_qwItemBitMask) == (l_qwInventoryBitMask | l_qwItemBitMask)) {
					l_qwInventoryBitMask |= l_qwItemBitMask;
					l_bOk = false;
					break;
				}
		if (l_bOk)
			return	(false);
	}
	return			(true);
}