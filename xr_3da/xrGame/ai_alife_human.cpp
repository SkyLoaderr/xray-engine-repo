
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
	m_tpCurrentBestWeapon		= 0;
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
				m_tpCurrentBestWeapon = l_tpALifeItemWeapon;
			}
		}
	}
	if (m_tpCurrentBestWeapon) {
		fHitPower				= m_tpCurrentBestWeapon->m_fHitPower;
		tHitType				= m_tpCurrentBestWeapon->m_tHitType;
		return					(m_tpCurrentBestWeapon);
	}
	else
		return(inherited2::tpfGetBestWeapon(tHitType,fHitPower));
}

bool CSE_ALifeHumanAbstract::bfPerformAttack()
{
	if (!m_tpCurrentBestWeapon)
		return(false);

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

void CSE_ALifeHumanAbstract::vfCollectAmmoBoxes()
{
	for (int i=0, n=children.size() ; i<n; i++) {
		
		if (m_tpALife->m_baMarks[i])
			continue;
		
		m_tpALife->m_baMarks[i]	= true;
		
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[i]));
		if (!l_tpALifeItemAmmo)
			continue;

		for (int j=i+1; j<n; j++) {
			if (m_tpALife->m_baMarks[j])
				continue;

			CSE_ALifeItemAmmo	*l_tpALifeItemAmmo1 = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[j]));
			if (!l_tpALifeItemAmmo1) {
				m_tpALife->m_baMarks[j]	= true;
				continue;
			}

			if (!strstr(l_tpALifeItemAmmo->s_name,l_tpALifeItemAmmo1->s_name))
				continue;

			m_tpALife->m_baMarks[j]	= true;

			if (l_tpALifeItemAmmo->a_elapsed + l_tpALifeItemAmmo1->a_elapsed > l_tpALifeItemAmmo->m_boxSize) {
				l_tpALifeItemAmmo1->a_elapsed	= l_tpALifeItemAmmo->a_elapsed + l_tpALifeItemAmmo1->a_elapsed - l_tpALifeItemAmmo->m_boxSize;
				l_tpALifeItemAmmo->a_elapsed	= l_tpALifeItemAmmo->m_boxSize;
				l_tpALifeItemAmmo				= l_tpALifeItemAmmo1;
			}
			else {
				l_tpALifeItemAmmo->a_elapsed	= l_tpALifeItemAmmo->a_elapsed + l_tpALifeItemAmmo1->a_elapsed;
				l_tpALifeItemAmmo1->a_elapsed	= 0;
			}
		}
	}

	for (int i=0, j=0; i<n; i++,j++) {
		m_tpALife->m_baMarks[j] = false;

		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(children[i]));
		if (!l_tpALifeItemAmmo || l_tpALifeItemAmmo->a_elapsed)
			continue;

		m_tpALife->vfDetachItem	(*this,l_tpALifeItemAmmo,m_tGraphID);
		m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
		i--;
		n--;
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
						l_tpALifeItemAmmo->a_elapsed = (u16)m_tpCurrentBestWeapon->m_dwAmmoAvailable;
						m_tpCurrentBestWeapon->m_dwAmmoAvailable = 0;
						continue;
					}
					m_tpALife->vfDetachItem(*this,l_tpALifeItemAmmo,m_tGraphID);
					m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
					i--;
					n--;
				}
			}
			m_tpCurrentBestWeapon = 0;
			break;
		}
	}
	vfCollectAmmoBoxes();
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

u16	CSE_ALifeHumanAbstract::get_available_ammo_count(CSE_ALifeItemWeapon *tpALifeItemWeapon, ITEM_P_VECTOR &tpItemVector)
{
	if (!tpALifeItemWeapon->m_caAmmoSections)
		return(u16(-1));
	u32							l_dwResult = 0;
	ITEM_P_IT					I = tpItemVector.begin();
	ITEM_P_IT					E = tpItemVector.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(*I);
		if (l_tpALifeItemAmmo && strstr(tpALifeItemWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name) && (l_tpALifeItemAmmo->m_dwCost <= m_dwTotalMoney)) {
			l_dwResult			+= l_tpALifeItemAmmo->a_elapsed;
			m_dwTotalMoney		-= l_tpALifeItemAmmo->m_dwCost;
		}
	}
	return						(u16(l_dwResult));
}

void CSE_ALifeHumanAbstract::attach_available_ammo(CSE_ALifeItemWeapon *tpALifeItemWeapon, ITEM_P_VECTOR &tpItemVector, OBJECT_VECTOR *tpObjectVector)
{
	if (!tpALifeItemWeapon->m_caAmmoSections)
		return;
	u32							l_dwCount = 0;
	ITEM_P_IT					I = tpItemVector.begin();
	ITEM_P_IT					E = tpItemVector.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(*I);
		if (l_tpALifeItemAmmo && strstr(tpALifeItemWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name) && bfCanGetItem(l_tpALifeItemAmmo) && (!tpObjectVector || (std::find(tpObjectVector->begin(),tpObjectVector->end(),l_tpALifeItemAmmo->ID) == tpObjectVector->end()))) {
			if (!tpObjectVector)
				m_tpALife->vfAttachItem(*this,l_tpALifeItemAmmo,l_tpALifeItemAmmo->m_tGraphID);
			else
				children.push_back(l_tpALifeItemAmmo->ID);
			l_dwCount++;
			if (l_dwCount > MAX_AMMO_ATTACH_COUNT)
				break;
		}
	}
}

void CSE_ALifeHumanAbstract::vfProcessItems()
{
	m_tpALife->m_tpItemVector.clear();
	D_OBJECT_PAIR_IT	I = m_tpALife->m_tpGraphObjects[m_tGraphID].tpObjects.begin();
	D_OBJECT_PAIR_IT	E = m_tpALife->m_tpGraphObjects[m_tGraphID].tpObjects.end();
	for ( ; I != E; I++) {
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>((*I).second);
		if (l_tpALifeInventoryItem && l_tpALifeInventoryItem->bfUseful() && !(*I).second->m_bOnline && (m_tpALife->randF(1.0f) < m_fProbability))
			m_tpALife->m_tpItemVector.push_back(l_tpALifeInventoryItem);
	}
	if (!m_tpALife->m_tpItemVector.empty())
		vfAttachItems();
}

void CSE_ALifeHumanAbstract::vfDetachAll()
{
	while (!children.empty()) {
		CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = m_tpALife->tpfGetObjectByID(*children.begin());
		CSE_ALifeInventoryItem		*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
		R_ASSERT2					(l_tpALifeInventoryItem,"Invalid inventory object");
		m_tpALife->vfDetachItem		(*this,l_tpALifeInventoryItem,m_tGraphID);
	}
	R_ASSERT2						((m_fCumulativeItemMass < EPS_L) && (m_iCumulativeItemVolume < EPS_L),"Invalid cumulative item mass or volume value");
	m_fCumulativeItemMass			= 0.f;
	m_iCumulativeItemVolume			= 0;
}

EMeetActionType	CSE_ALifeHumanAbstract::tfGetActionType(CSE_ALifeSchedulable *tpALifeSchedulable, int iGroupIndex, bool bMutualDetection)
{
	if (m_tpALife->m_tCombatType == eCombatTypeMonsterMonster) {
		CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable);
		R_ASSERT2					(l_tpALifeMonsterAbstract,"Inconsistent meet action type");
		return						(m_tpALife->tfGetRelationType(this,dynamic_cast<CSE_ALifeMonsterAbstract*>(tpALifeSchedulable)) == eRelationTypeFriend ? eMeetActionTypeInteract : ((bMutualDetection || m_tpALife->tfChooseCombatAction(iGroupIndex)==eCombatActionAttack) ? eMeetActionTypeAttack : eMeetActionTypeIgnore));
	}
	else
		return(eMeetActionTypeAttack);
}

CSE_ALifeDynamicObject *CSE_ALifeHumanAbstract::tpfGetBestDetector()
{
	m_tpBestDetector				= 0;
	CSE_ALifeGroupAbstract			*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(this);
	if (l_tpALifeGroupAbstract) {
		u32							l_dwBestValue = 0;
		if (!l_tpALifeGroupAbstract->m_wCount)
			return					(0);
		OBJECT_IT					I = l_tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = l_tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(m_tpALife->tpfGetObjectByID(l_tpALifeGroupAbstract->m_tpMembers[0]));
			R_ASSERT				(l_tpALifeHumanAbstract);
			getAI().m_tpCurrentALifeObject = l_tpALifeHumanAbstract->tpfGetBestDetector();
			u32						l_dwCurrentValue = iFloor(getAI().m_pfDetectorType->ffGetValue()+.5f);
			if (l_dwCurrentValue > l_dwBestValue) {
				l_dwBestValue		= l_dwCurrentValue;
				m_tpBestDetector	= dynamic_cast<CSE_ALifeDynamicObject*>(getAI().m_tpCurrentALifeObject);
			}
		}
		return						(m_tpBestDetector);
	}
	
	OBJECT_IT						I = children.begin();
	OBJECT_IT						E = children.end();
	for ( ; I != E; I++) {
		CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = m_tpALife->tpfGetObjectByID(*I);
		CSE_ALifeInventoryItem		*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
		R_ASSERT2					(l_tpALifeInventoryItem,"Non-item object in the inventory found");
		switch (l_tpALifeDynamicObject->m_tClassID) {
			case CLSID_DETECTOR_SIMPLE		: {
				m_tpBestDetector	= l_tpALifeDynamicObject;
				break;
			}
			case CLSID_DETECTOR_VISUAL		: {
				m_tpBestDetector	= l_tpALifeDynamicObject;
				return				(m_tpBestDetector);
			}
		}
	}
	return							(m_tpBestDetector);
}

bool CSE_ALifeHumanAbstract::bfCanGetItem(CSE_ALifeInventoryItem *tpALifeInventoryItem)
{
	if (tpALifeInventoryItem && ((m_fCumulativeItemMass + tpALifeInventoryItem->m_fMass > m_fMaxItemMass) || (m_iCumulativeItemVolume + tpALifeInventoryItem->m_iVolume > MAX_ITEM_VOLUME)))
		return		(false);
	
	if (tpALifeInventoryItem)
		m_tpTempItemBuffer.resize(children.size() + 1);
	
	{
		OBJECT_IT	i = children.begin();
		OBJECT_IT	e = children.end();
		ITEM_P_IT	I = m_tpTempItemBuffer.begin();
		for ( ; i != e; i++, I++)
			*I		= dynamic_cast<CSE_ALifeInventoryItem*>(m_tpALife->tpfGetObjectByID(*i));
		if (tpALifeInventoryItem)
			*I		= tpALifeInventoryItem;
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

#pragma todo("Dima to Dima,Oles,AlexMX,Yura,Jim,Kostia : Instead of greeding algorithm implement faster algorithm which _always_ computes _correct_ result (though this problem seems to be NP)")

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

void CSE_ALifeHumanAbstract::vfChooseGroup(CSE_ALifeGroupAbstract *tpALifeGroupAbstract)
{
	{
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(m_tpALife->tpfGetObjectByID(*I));
			R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
			l_tpALifeHumanAbstract->vfAttachItems(eTakeTypeMin);
		}
	}

	{
		OBJECT_IT					I = tpALifeGroupAbstract->m_tpMembers.begin();
		OBJECT_IT					E = tpALifeGroupAbstract->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(m_tpALife->tpfGetObjectByID(*I));
			R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
			l_tpALifeHumanAbstract->vfAttachItems(eTakeTypeRest);
		}
	}
}

bool CSE_ALifeHumanAbstract::bfChooseFast()
{
	// fast check if I can pick up all the items
	u32								l_dwCurrentItemCount = children.size();
	float							l_fCumulativeItemMass = m_fCumulativeItemMass;
	int								l_iCumulativeItemVolume = m_iCumulativeItemVolume;
	bool							l_bOk = true;
	ITEM_P_IT						I = m_tpALife->m_tpItemVector.begin();
	ITEM_P_IT						E = m_tpALife->m_tpItemVector.end(), J = E - 1;
	for ( ; I != E; I++)
		if ((I != J) || bfCanGetItem(*I)) {
			m_fCumulativeItemMass	+= (*I)->m_fMass;
			m_iCumulativeItemVolume	+= (*I)->m_iVolume;
			children.push_back		((*I)->ID);
		}
		else {
			l_bOk = false;
			break;
		}

	m_fCumulativeItemMass			= l_fCumulativeItemMass;
	m_iCumulativeItemVolume			= l_iCumulativeItemVolume;
	children.resize					(l_dwCurrentItemCount);

	if (l_bOk) {
		I							= m_tpALife->m_tpItemVector.begin();
		for ( ; I != E; I++)
			m_tpALife->vfAttachItem	(*this,*I,dynamic_cast<CSE_ALifeDynamicObject*>(*I)->m_tGraphID);
		return						(true);
	}
	
	return							(false);
}

int CSE_ALifeHumanAbstract::ifChooseEquipment(OBJECT_VECTOR *tpObjectVector)
{
	// choosing equipment
	CSE_ALifeInventoryItem			*l_tpALifeItemBest	= 0;
	float							l_fItemBestValue	= -1.f;
	getAI().m_tpCurrentALifeMember	= this;

	ITEM_P_IT					I = m_tpALife->m_tpItemVector.begin(), X;
	ITEM_P_IT					E = m_tpALife->m_tpItemVector.end();
	for ( ; I != E; I++) {
		// checking if it is an equipment item
		getAI().m_tpCurrentALifeObject = dynamic_cast<CSE_ALifeObject*>(*I);
		if (getAI().m_pfEquipmentType->ffGetValue() > getAI().m_pfEquipmentType->ffGetMaxResultValue())
			continue;
		if (m_dwTotalMoney < (*I)->m_dwCost)
			continue;
		// evaluating item
		float					l_fCurrentValue = getAI().m_pfEquipmentType->ffGetValue();
		// choosing the best item
		if ((l_fCurrentValue > l_fItemBestValue) && bfCanGetItem(*I) && (!tpObjectVector || (std::find(tpObjectVector->begin(),tpObjectVector->end(),(*I)->ID) == tpObjectVector->end()))) {
			l_fItemBestValue	= l_fCurrentValue;
			l_tpALifeItemBest	= *I;
			X					= I;
		}
	}
	if (l_tpALifeItemBest) {
		if (!tpObjectVector) {
			m_tpALife->vfAttachItem	(*this,l_tpALifeItemBest,dynamic_cast<CSE_ALifeDynamicObject*>(l_tpALifeItemBest)->m_tGraphID);
			m_tpALife->m_tpItemVector.erase(X);
		}
		else
			children.push_back	(l_tpALifeItemBest->ID);
		return					(1);
	}
	return						(0);
}

int  CSE_ALifeHumanAbstract::ifChooseWeapon(EWeaponPriorityType tWeaponPriorityType, OBJECT_VECTOR *tpObjectVector)
{
	CSE_ALifeInventoryItem			*l_tpALifeItemBest	= 0;
	float							l_fItemBestValue	= -1.f;
	getAI().m_tpCurrentALifeMember	= this;

	u32						l_dwSafeMoney = m_dwTotalMoney;
	ITEM_P_IT				I = m_tpALife->m_tpItemVector.begin();
	ITEM_P_IT				E = m_tpALife->m_tpItemVector.end();
	for ( ; I != E; I++) {
		// checking if it is a hand weapon
		getAI().m_tpCurrentALifeObject = dynamic_cast<CSE_ALifeObject*>(*I);
		if (m_dwTotalMoney < (*I)->m_dwCost)
			continue;
		m_dwTotalMoney			-= (*I)->m_dwCost;
		int						j = getAI().m_pfPersonalWeaponType->dwfGetWeaponType();
		float					l_fCurrentValue = -1.f;
		switch (tWeaponPriorityType) {
			case eWeaponPriorityTypeKnife : {
				if (j != 1)
					continue;
				l_fCurrentValue = getAI().m_pfItemValue->ffGetValue();
				break;
			}
			case eWeaponPriorityTypeSecondary : {
				if (j != 5)
					continue;
				l_fCurrentValue = getAI().m_pfSmallWeaponValue->ffGetValue();
				break;
			}
			case eWeaponPriorityTypePrimary : {
				if ((j != 6) && (j != 8) && (j != 9))
					continue;
				l_fCurrentValue = getAI().m_pfMainWeaponValue->ffGetValue();
				break;
			}
			case eWeaponPriorityTypeGrenade : {
				if (j != 7)
					continue;
				l_fCurrentValue = getAI().m_pfItemValue->ffGetValue();
				break;
			}
			default : NODEFAULT;
		}
		m_dwTotalMoney			= l_dwSafeMoney;
		// choosing the best item
		if ((l_fCurrentValue > l_fItemBestValue) && bfCanGetItem(*I) && (!tpObjectVector || (std::find(tpObjectVector->begin(),tpObjectVector->end(),(*I)->ID) == tpObjectVector->end()))) {
			l_fItemBestValue = l_fCurrentValue;
			l_tpALifeItemBest = *I;
		}
	}
	if (l_tpALifeItemBest) {
		u32						l_dwCount = children.size();
		
		if (!tpObjectVector)
			m_tpALife->vfAttachItem	(*this,l_tpALifeItemBest,dynamic_cast<CSE_ALifeDynamicObject*>(l_tpALifeItemBest)->m_tGraphID);
		else
			children.push_back	(l_tpALifeItemBest->ID);
		
		attach_available_ammo	(dynamic_cast<CSE_ALifeItemWeapon*>(l_tpALifeItemBest),m_tpALife->m_tpItemVector,tpObjectVector);
		
		if (!tpObjectVector) {
			ITEM_P_IT				I = remove_if(m_tpALife->m_tpItemVector.begin(),m_tpALife->m_tpItemVector.end(),CRemoveAttachedItemsPredicate());
			m_tpALife->m_tpItemVector.erase(I,m_tpALife->m_tpItemVector.end());
		}
		else
			remove_if			(children.begin(),children.end(),CRemoveNonAttachedItemsPredicate(m_tpALife));

		return					(children.size() - l_dwCount);
	}
	return						(0);
}

int  CSE_ALifeHumanAbstract::ifChooseFood(OBJECT_VECTOR *tpObjectVector)
{
#pragma todo("Dima to Dima : Add food and medikit items need count computations")
	// choosing food
	getAI().m_tpCurrentALifeMember	= this;
	u32							l_dwCount = 0, l_dwSafeMoney = m_dwTotalMoney;
	ITEM_P_IT					I = m_tpALife->m_tpItemVector.begin();
	ITEM_P_IT					E = m_tpALife->m_tpItemVector.end();
	for ( ; I != E; I++) {
		if ((*I)->m_iFoodValue <= 0)
			continue;
		if (m_dwTotalMoney < (*I)->m_dwCost)
			continue;
		if (bfCanGetItem(*I) && (!tpObjectVector || (std::find(tpObjectVector->begin(),tpObjectVector->end(),(*I)->ID) == tpObjectVector->end()))) {
			m_dwTotalMoney		-= (*I)->m_dwCost;
			if (!tpObjectVector)
				m_tpALife->vfAttachItem	(*this,*I,dynamic_cast<CSE_ALifeDynamicObject*>(*I)->m_tGraphID);
			else
				children.push_back((*I)->ID);
			l_dwCount++;
			if (l_dwCount > MAX_ITEM_FOOD_COUNT)
				break;
		}
	}
	m_dwTotalMoney = l_dwSafeMoney;
	if (l_dwCount) {
		if (!tpObjectVector) {
			ITEM_P_IT			I = remove_if(m_tpALife->m_tpItemVector.begin(),m_tpALife->m_tpItemVector.end(),CRemoveAttachedItemsPredicate());
			m_tpALife->m_tpItemVector.erase(I,m_tpALife->m_tpItemVector.end());
		}
		else
			remove_if(children.begin(),children.end(),CRemoveNonAttachedItemsPredicate(m_tpALife));
	}
	return					(l_dwCount);
}

int  CSE_ALifeHumanAbstract::ifChooseMedikit(OBJECT_VECTOR *tpObjectVector)
{
	// choosing medikits
	u32						l_dwCount = 0, l_dwSafeMoney = m_dwTotalMoney;
	ITEM_P_IT				I = m_tpALife->m_tpItemVector.begin();
	ITEM_P_IT				E = m_tpALife->m_tpItemVector.end();
	for ( ; I != E; I++) {
		if ((*I)->m_iHealthValue <= 0)
			continue;
		if (m_dwTotalMoney < (*I)->m_dwCost)
			continue;
		if (bfCanGetItem(*I) && (!tpObjectVector || (std::find(tpObjectVector->begin(),tpObjectVector->end(),(*I)->ID) == tpObjectVector->end()))) {
			m_dwTotalMoney	-= (*I)->m_dwCost;
			if (!tpObjectVector)
				m_tpALife->vfAttachItem	(*this,*I,dynamic_cast<CSE_ALifeDynamicObject*>(*I)->m_tGraphID);
			else
				children.push_back((*I)->ID);
			l_dwCount++;
			if (l_dwCount > MAX_ITEM_MEDIKIT_COUNT)
				break;
		}
	}
	m_dwTotalMoney = l_dwSafeMoney;
	if (l_dwCount) {
		if (!tpObjectVector) {
			ITEM_P_IT		I = remove_if(m_tpALife->m_tpItemVector.begin(),m_tpALife->m_tpItemVector.end(),CRemoveAttachedItemsPredicate());
			m_tpALife->m_tpItemVector.erase(I,m_tpALife->m_tpItemVector.end());
		}
		else
			remove_if		(children.begin(),children.end(),CRemoveNonAttachedItemsPredicate(m_tpALife));

	}
	return					(l_dwCount);
}

int  CSE_ALifeHumanAbstract::ifChooseDetector(OBJECT_VECTOR *tpObjectVector)
{
	// choosing detector
	CSE_ALifeInventoryItem		*l_tpALifeItemBest	= 0;
	float						l_fItemBestValue	= -1.f;
	getAI().m_tpCurrentALifeMember	= this;
	ITEM_P_IT					I = m_tpALife->m_tpItemVector.begin(), X;
	ITEM_P_IT					E = m_tpALife->m_tpItemVector.end();
	for ( ; I != E; I++) {
		// checking if it is an item
		CSE_ALifeItemDetector	*l_tpALifeItem = dynamic_cast<CSE_ALifeItemDetector*>(*I);
		if (!l_tpALifeItem)
			continue;
		if (m_dwTotalMoney < l_tpALifeItem->m_dwCost)
			continue;
		// evaluating item
		getAI().m_tpCurrentALifeObject = l_tpALifeItem;
		float					l_fCurrentValue = getAI().m_pfEquipmentType->ffGetValue();
		// choosing the best item
		if ((l_fCurrentValue > l_fItemBestValue) && bfCanGetItem(l_tpALifeItem) && (!tpObjectVector || (std::find(tpObjectVector->begin(),tpObjectVector->end(),l_tpALifeItem->ID) == tpObjectVector->end()))) {
			l_fItemBestValue = l_fCurrentValue;
			l_tpALifeItemBest = l_tpALifeItem;
			X = I;
		}
	}
	if (l_tpALifeItemBest) {
		if (!tpObjectVector) {
			m_tpALife->vfAttachItem	(*this,l_tpALifeItemBest,dynamic_cast<CSE_ALifeDynamicObject*>(l_tpALifeItemBest)->m_tGraphID);
			m_tpALife->m_tpItemVector.erase(X);
		}
		else
			children.push_back(l_tpALifeItemBest->ID);
		return					(1);
	}
	return						(0);
}

int  CSE_ALifeHumanAbstract::ifChooseValuables(OBJECT_VECTOR *tpObjectVector)
{
	// choosing the rest objects
	ITEM_P_IT				I = m_tpALife->m_tpItemVector.begin();
	ITEM_P_IT				E = m_tpALife->m_tpItemVector.end();
	for ( ; I != E; I++)
		if (bfCanGetItem(*I))
			m_tpALife->vfAttachItem	(*this,*I,dynamic_cast<CSE_ALifeDynamicObject*>(*I)->m_tGraphID);

	u32						l_dwCount = children.size();
	I						= remove_if(m_tpALife->m_tpItemVector.begin(),m_tpALife->m_tpItemVector.end(),CRemoveAttachedItemsPredicate());
	if (!tpObjectVector)
		m_tpALife->m_tpItemVector.erase(I,m_tpALife->m_tpItemVector.end());
	return					(children.size() - l_dwCount);
}

void CSE_ALifeHumanAbstract::vfAttachItems(ETakeType tTakeType)
{
	R_ASSERT2					(fHealth >= EPS_L,"Cannot attach items to dead human");
	
	CSE_ALifeGroupAbstract		*l_tpALifeGroupAbstract = dynamic_cast<CSE_ALifeGroupAbstract*>(this);
	if (l_tpALifeGroupAbstract) {
		vfChooseGroup			(l_tpALifeGroupAbstract);
		return;
	}
	else
		if (bfChooseFast())
			return;
	
	if (tTakeType == eTakeTypeAll) {
		m_tpALife->vfAppendItemVector	(children,m_tpALife->m_tpItemVector);
		vfDetachAll				();
	}
	
	sort						(m_tpALife->m_tpItemVector.begin(),m_tpALife->m_tpItemVector.end(),CSortItemPredicate());

	if ((tTakeType == eTakeTypeAll) || (tTakeType == eTakeTypeMin)) {
		ifChooseEquipment		();
		ifChooseWeapon			(eWeaponPriorityTypeKnife);
		ifChooseWeapon			(eWeaponPriorityTypeSecondary);
		ifChooseWeapon			(eWeaponPriorityTypePrimary);
		ifChooseWeapon			(eWeaponPriorityTypeGrenade);
		ifChooseFood			();
		ifChooseMedikit			();
		ifChooseDetector		();
	}

	if ((tTakeType == eTakeTypeAll) || (tTakeType == eTakeTypeRest))
		ifChooseValuables		();
}