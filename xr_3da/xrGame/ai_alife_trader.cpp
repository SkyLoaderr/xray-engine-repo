////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_trader.cpp
//	Created 	: 03.09.2003
//  Modified 	: 03.09.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life traders simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

void CSE_ALifeTraderAbstract::vfInitInventory()
{
	m_fCumulativeItemMass	= 0.f;
	m_iCumulativeItemVolume = 0;
}

void CSE_ALifeTraderAbstract::vfAttachItem(CSE_ALifeInventoryItem *tpALifeInventoryItem, bool bALifeRequest, bool bAddChildren)
{
	if (bALifeRequest) {
		if (bAddChildren) {
#ifdef ALIFE_LOG
			Msg							("[LSS] : Adding item [%s][%d] to the children list",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
#endif
			R_ASSERT2					(std::find(children.begin(),children.end(),tpALifeInventoryItem->ID) == children.end(),"Item is already inside the inventory");
			children.push_back			(tpALifeInventoryItem->ID);
		}
#ifdef ALIFE_LOG
		Msg							("[LSS] : Assigning parent to item [%s][%d]",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
#endif
		tpALifeInventoryItem->ID_Parent	= ID;
	}
#ifdef ALIFE_LOG
	Msg							("[LSS] : Updating inventory with attached item [%s][%d]",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
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
#ifdef ALIFE_LOG
				Msg								("[LSS] : Removinng item [%s][%d] from children list",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
#endif
				children.erase					(I);
			}
		}
		else {
#ifdef ALIFE_LOG
			Msg									("[LSS] : Removinng item [%s][%d] from children list",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
#endif
			children.erase						(*I);
		}
		tpALifeInventoryItem->ID_Parent			= 0xffff;
		CSE_ALifeDynamicObject					*l_tpALifeDynamicObject1 = dynamic_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem);
		CSE_ALifeDynamicObject					*l_tpALifeDynamicObject2 = dynamic_cast<CSE_ALifeDynamicObject*>(this);
		R_ASSERT2								(l_tpALifeDynamicObject1 && l_tpALifeDynamicObject2,"Invalid parent or children objects");
#ifdef ALIFE_LOG
		Msg										("[LSS] : Removing parent from the item [%s][%d] and updating its position and graph point [%f][%f][%f] : [%d]",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID,VPUSH(l_tpALifeDynamicObject2->o_Position),l_tpALifeDynamicObject2->m_tGraphID);
#endif
		l_tpALifeDynamicObject1->o_Position		= l_tpALifeDynamicObject2->o_Position;
		l_tpALifeDynamicObject1->m_tGraphID		= l_tpALifeDynamicObject2->m_tGraphID;
		l_tpALifeDynamicObject1->m_fDistance	= l_tpALifeDynamicObject2->m_fDistance;
		tpALifeInventoryItem->m_tPreviousParentID = l_tpALifeDynamicObject2->ID;
	}
#ifdef ALIFE_LOG
	Msg							("[LSS] : Updating inventory with detached item [%s][%d]",tpALifeInventoryItem->s_name_replace,tpALifeInventoryItem->ID);
#endif
	m_fCumulativeItemMass		-= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume		-= tpALifeInventoryItem->m_iVolume;
}