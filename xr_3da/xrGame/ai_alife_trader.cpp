////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_trader.cpp
//	Created 	: 03.09.2003
//  Modified 	: 03.09.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life traders simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_registries.h"

void CSE_ALifeTraderAbstract::vfInitInventory()
{
	m_fCumulativeItemMass	= 0.f;
	m_iCumulativeItemVolume = 0;
}

void CSE_ALifeTraderAbstract::vfAttachItem(CSE_ALifeInventoryItem *tpALifeInventoryItem, bool bALifeRequest, bool bAddChildren)
{
	if (bALifeRequest) {
		if (bAddChildren) {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg						("[LSS] Adding item [%s][%d] to the [%s] children list",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,s_name_replace);
			}
#endif

			R_ASSERT2					(std::find(children.begin(),children.end(),tpALifeInventoryItem->ID) == children.end(),"Item is already inside the inventory");
			children.push_back			(tpALifeInventoryItem->ID);
		}
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] Assigning parent [%s] to item [%s][%d]",s_name_replace,tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
		}
#endif
		tpALifeInventoryItem->ID_Parent	= ID;
	}
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg								("[LSS] Updating [%s] inventory with attached item [%s][%d]",s_name_replace,tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
	}
#endif
	m_fCumulativeItemMass				+= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume				+= tpALifeInventoryItem->m_iVolume;
}

void CSE_ALifeTraderAbstract::vfDetachItem(CSE_ALifeInventoryItem *tpALifeInventoryItem, OBJECT_IT *I, bool bALifeRequest, bool bRemoveChildren)
{
	if (bALifeRequest) {
		if (!I) {
			if (bRemoveChildren) {
				OBJECT_IT						I = std::find	(children.begin(),children.end(),tpALifeInventoryItem->ID);
				R_ASSERT2						(I != children.end(),"Can't detach an item which is not on my own");
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg							("[LSS] Removinng item [%s][%d] from [%s] children list",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,s_name_replace);
				}
#endif
				children.erase					(I);
			}
		}
		else {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg								("[LSS] Removinng item [%s][%d] from [%s] children list",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,s_name_replace);
			}
#endif
			children.erase						(*I);
		}
		tpALifeInventoryItem->ID_Parent			= 0xffff;
		CSE_ALifeDynamicObject					*l_tpALifeDynamicObject1 = dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem);
		CSE_ALifeDynamicObject					*l_tpALifeDynamicObject2 = dynamic_cast<CSE_ALifeDynamicObject*>(this);
		R_ASSERT2								(l_tpALifeDynamicObject1 && l_tpALifeDynamicObject2,"Invalid parent or children objects");
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg									("[LSS] Removing parent [%s] from the item [%s][%d] and updating its position and graph point [%f][%f][%f] : [%d]",s_name_replace,tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,VPUSH(l_tpALifeDynamicObject2->o_Position),l_tpALifeDynamicObject2->m_tGraphID);
		}
#endif
		l_tpALifeDynamicObject1->o_Position		= l_tpALifeDynamicObject2->o_Position;
		l_tpALifeDynamicObject1->m_tGraphID		= l_tpALifeDynamicObject2->m_tGraphID;
		l_tpALifeDynamicObject1->m_fDistance	= l_tpALifeDynamicObject2->m_fDistance;
		tpALifeInventoryItem->m_tPreviousParentID = l_tpALifeDynamicObject2->ID;
	}
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Updating [%s] inventory with detached item [%s][%d]",s_name_replace,tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
	}
#endif
	m_fCumulativeItemMass		-= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume		-= tpALifeInventoryItem->m_iVolume;
}

u32	CSE_ALifeTrader::dwfGetItemCost(CSE_ALifeInventoryItem *tpALifeInventoryItem, CSE_ALifeObjectRegistry *tpALifeObjectRegistry)
{
#pragma todo("Dima to Dima : correct price for non-artefact objects")
	CSE_ALifeItemArtefact		*l_tpALifeItemArtefact = dynamic_cast<CSE_ALifeItemArtefact*>(tpALifeInventoryItem);
	if (!l_tpALifeItemArtefact)
		return					(tpALifeInventoryItem->m_dwCost);

	u32							l_dwPurchasedCount = 0;
#pragma todo("Dima to Dima : optimize this cycle by keeping additional data structure with bought items")
	{
		OBJECT_IT				i = children.begin();
		OBJECT_IT				e = children.end();
		for ( ; i != e; i++)
			if (!strcmp(tpALifeObjectRegistry->tpfGetObjectByID(*i)->s_name,l_tpALifeItemArtefact->s_name))
				l_dwPurchasedCount++;
	}

	ARTEFACT_TRADER_ORDER_PAIR_IT	J = m_tpOrderedArtefacts.find(tpALifeInventoryItem->s_name);
	if ((J != m_tpOrderedArtefacts.end()) && (l_dwPurchasedCount < (*J).second->m_dwTotalCount)) {
		ARTEFACT_ORDER_IT		I = (*J).second->m_tpOrders.begin();
		ARTEFACT_ORDER_IT		E = (*J).second->m_tpOrders.end();
		for ( ; I != E; I++) {
			if (l_dwPurchasedCount <= (*I).m_dwCount)
				return			((*I).m_dwPrice);
			else
				l_dwPurchasedCount -= (*I).m_dwCount;
		}
		Debug.fatal				("Data synchronization mismatch");
	}
	return						(tpALifeInventoryItem->m_dwCost);
}