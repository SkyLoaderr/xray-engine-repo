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

void CSE_ALifeHumanAbstract::vfProcessItems()
{
	m_tpALife->m_tpObjectVector.clear();
	for (int I=0; I<(int)m_tpALife->m_tpGraphObjects[m_tGraphID].tpObjects.size(); I++) {
		u16						wID = m_tpALife->m_tpGraphObjects[m_tGraphID].tpObjects[I]->ID;
		CSE_ALifeDynamicObject	*tpALifeDynamicObject = m_tpALife->tpfGetObjectByID(wID);
		CSE_ALifeItem			*tpALifeItem = dynamic_cast<CSE_ALifeItem *>(tpALifeDynamicObject);
		if (tpALifeItem && !tpALifeItem->m_bOnline && (m_tpALife->randF(1.0f) < m_fProbability))
			m_tpALife->m_tpObjectVector.push_back(tpALifeItem->ID);
	}
	if (!m_tpALife->m_tpObjectVector.empty())
		vfAttachItems();
}

void CSE_ALifeHumanAbstract::vfCheckForDeletedEvents()
{
	PERSONAL_EVENT_P_IT I = std::remove_if(m_tpEvents.begin(),m_tpEvents.end(),CRemovePersonalEventPredicate(m_tpALife->m_tEventRegistry));
	m_tpEvents.erase(I,m_tpEvents.end());
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
	u32							l_dwBestWeapon = u32(-1);
	OBJECT_IT					I = children.begin();
	OBJECT_IT					E = children.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemWeapon		*l_tpALifeItemWeapon = dynamic_cast<CSE_ALifeItemWeapon*>(m_tpALife->tpfGetObjectByID(*I));
		if (!l_tpALifeItemWeapon)
			continue;

		l_tpALifeItemWeapon->m_dwAmmoAvailable = get_available_ammo_count(l_tpALifeItemWeapon);
		if (l_tpALifeItemWeapon->m_dwAmmoAvailable || (l_tpALifeItemWeapon->m_dwSlot == 0) || (l_tpALifeItemWeapon->m_dwSlot == 3)) {
			u32					l_dwCurrentBestWeapon = u32(-1); 
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
					m_tpALife->vfReleaseObject(m_tpALife->tpfGetObjectByID(*I),true);
					children.erase		(I);
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
					m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
					children.erase		(children.begin() + i);
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
			return;
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
					m_tpALife->vfReleaseObject(l_tpALifeItemAmmo,true);
					children.erase		(children.begin() + i);
					i--;
					n--;
				}
			}
			break;
		}
	}
}

u16	CSE_ALifeHumanAbstract::get_available_ammo_count(CSE_ALifeItemWeapon *tpALifeItemWeapon)
{
	u32							l_dwResult = 0;
	OBJECT_IT					I = children.begin();
	OBJECT_IT					E = children.end();
	for ( ; I != E; I++) {
		CSE_ALifeItemAmmo		*l_tpALifeItemAmmo = dynamic_cast<CSE_ALifeItemAmmo*>(m_tpALife->tpfGetObjectByID(*I));
		if (l_tpALifeItemAmmo && strstr(tpALifeItemWeapon->m_caAmmoSections,l_tpALifeItemAmmo->s_name))
			l_dwResult			+= l_tpALifeItemAmmo->a_elapsed;
	}
	return						(u16(l_dwResult));
}

void CSE_ALifeHumanAbstract::vfAttachItems()
{
	CSE_ALifeAbstractGroup		*l_tpALifeAbstractGroup = dynamic_cast<CSE_ALifeAbstractGroup*>(this);
	if (l_tpALifeAbstractGroup) {
		OBJECT_IT				I = l_tpALifeAbstractGroup->m_tpMembers.begin();
		OBJECT_IT				E = l_tpALifeAbstractGroup->m_tpMembers.end();
		for ( ; I != E; I++) {
			CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = dynamic_cast<CSE_ALifeHumanAbstract*>(m_tpALife->tpfGetObjectByID(*I));
			R_ASSERT2				(l_tpALifeHumanAbstract,"Invalid group member");
			l_tpALifeHumanAbstract->vfAttachItems();
		}
		return;
	}
	m_tpALife->m_tpObjectVector.insert(m_tpALife->m_tpObjectVector.end(),children.begin(),children.end());
	OBJECT_IT					I = l_tpALifeAbstractGroup->m_tpMembers.begin();
	OBJECT_IT					E = l_tpALifeAbstractGroup->m_tpMembers.end();
	for ( ; I != E; I++) {
		CSE_ALifeItem			*l_tpALifeItem = dynamic_cast<CSE_ALifeItem*>(m_tpALife->tpfGetObjectByID(*I));
		R_ASSERT2				(l_tpALifeItem,"Invalid inventory object");
		m_tpALife->vfDetachItem	(*this,l_tpALifeItem,m_tGraphID);
	}
	R_ASSERT2					(m_fCumulativeItemMass < EPS_L,"Invalid cumulative mass value");
	m_fCumulativeItemMass		= 0.f;
#pragma todo("Append with item choice logic")

}