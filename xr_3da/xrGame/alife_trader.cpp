////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_trader.cpp
//	Created 	: 03.09.2003
//  Modified 	: 03.09.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life traders simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "ai_space.h"
#include "specific_character.h"

using namespace ALife;


void CSE_ALifeObject::spawn_supplies		(LPCSTR ini_string)
{
	if (!xr_strlen(ini_string))
		return;

#pragma warning(push)
#pragma warning(disable:4238)
	CInifile					ini(
		&IReader				(
		(void*)(ini_string),
		xr_strlen(ini_string)
		)
		);
#pragma warning(pop)

	if (ini.section_exist("spawn")) {
		LPCSTR					N,V;
		for (u32 k = 0, j; ini.r_line("spawn",k,&N,&V); k++) {
			VERIFY				(xr_strlen(N));
	
			BOOL A[3]			= {0,0,0};
			j					= 1;
			if (V && xr_strlen(V)) 
			{	
				string64 buf;
				int items_count = _GetItemCount(V);
				j				= atoi(_GetItem(V, 0, buf));
				if (!j)
					j			= 1;

				int addons_flags_num = (items_count-1)>3?3:items_count-1;
				for(int item_i = 0; item_i<addons_flags_num; item_i++)
				{
					A[item_i] = !!atoi(_GetItem(V, item_i+1, buf));
				}
			}

			for (u32 i=0; i<j; ++i)
			{
				CSE_Abstract* E = alife().spawn_item	(N,o_Position,m_tNodeID,m_tGraphID,ID);
				//подсоединить аддоны к оружию, если включены соответствующие флажки
				CSE_ALifeItemWeapon* W =  smart_cast<CSE_ALifeItemWeapon*>(E);
				if(W)
				{
					if (W->m_scope_status == CSE_ALifeItemWeapon::eAddonAttachable)
						W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, A[0]);
					if (W->m_silincer_status == CSE_ALifeItemWeapon::eAddonAttachable)
						W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, A[1]);
					if (W->m_grenade_launcher_status == CSE_ALifeItemWeapon::eAddonAttachable)
						W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, A[2]);
				}
			}
		}
	}
}

void CSE_ALifeObject::spawn_supplies		()
{
	spawn_supplies(*m_ini_string);
}

void CSE_ALifeTraderAbstract::spawn_supplies	()
{
	CSE_ALifeDynamicObject		*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(this);
	VERIFY						(dynamic_object);
	CSE_Abstract				*abstract = dynamic_object->alife().spawn_item("device_pda",base()->o_Position,dynamic_object->m_tNodeID,dynamic_object->m_tGraphID,base()->ID);
	CSE_ALifeItemPDA			*pda = smart_cast<CSE_ALifeItemPDA*>(abstract);
	pda->m_original_owner		= base()->ID;

#ifdef XRGAME_EXPORTS
	relation_registry.Init(base()->ID);
	character_profile();
	specific_character();
#endif

	if(NO_SPECIFIC_CHARACTER != m_iSpecificCharacter)
	{
		CSpecificCharacter selected_char;
		selected_char.Load(m_iSpecificCharacter);
		dynamic_object->spawn_supplies(selected_char.SupplySpawn());
	}
}

void CSE_ALifeTraderAbstract::vfInitInventory()
{
	m_fCumulativeItemMass		= 0.f;
	m_iCumulativeItemVolume		= 0;
}

void CSE_ALifeTraderAbstract::attach	(CSE_ALifeInventoryItem *tpALifeInventoryItem, bool bALifeRequest, bool bAddChildren)
{
	if (bALifeRequest) {
		if (bAddChildren) {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg						("[LSS] Adding item [%s][%d] to the [%s] children list",tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,base()->name_replace());
			}
#endif

			R_ASSERT2					(std::find(base()->children.begin(),base()->children.end(),tpALifeInventoryItem->base()->ID) == base()->children.end(),"Item is already inside the inventory");
			base()->children.push_back	(tpALifeInventoryItem->base()->ID);
		}
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] Assigning parent [%s] to item [%s][%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID);
		}
#endif
		tpALifeInventoryItem->base()->ID_Parent	= base()->ID;
	}
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg								("[LSS] Updating [%s] inventory with attached item [%s][%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID);
	}
#endif
	m_fCumulativeItemMass				+= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume				+= tpALifeInventoryItem->m_iVolume;
}

void CSE_ALifeTraderAbstract::detach(CSE_ALifeInventoryItem *tpALifeInventoryItem, OBJECT_IT *I, bool bALifeRequest, bool bRemoveChildren)
{
	if (bALifeRequest) {
		if (!I) {
			if (bRemoveChildren) {
				OBJECT_IT						I = std::find	(base()->children.begin(),base()->children.end(),tpALifeInventoryItem->base()->ID);
				R_ASSERT2						(base()->children.end() != I,"Can't detach an item which is not on my own");
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg							("[LSS] Removinng item [%s][%d] from [%s] children list",tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,base()->name_replace());
				}
#endif
				base()->children.erase			(I);
			}
		}
		else {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg								("[LSS] Removinng item [%s][%d] from [%s] children list",tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,base()->name_replace());
			}
#endif
			base()->children.erase				(*I);
		}
		tpALifeInventoryItem->base()->ID_Parent	= 0xffff;
	}
	
	CSE_ALifeDynamicObject					*l_tpALifeDynamicObject1 = smart_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem);
	CSE_ALifeDynamicObject					*l_tpALifeDynamicObject2 = smart_cast<CSE_ALifeDynamicObject*>(this);
	R_ASSERT2								(l_tpALifeDynamicObject1 && l_tpALifeDynamicObject2,"Invalid parent or children objects");
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg									("[LSS] Removing parent [%s] from the item [%s][%d] and updating its position and graph point [%f][%f][%f] : [%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,VPUSH(l_tpALifeDynamicObject2->o_Position),l_tpALifeDynamicObject2->m_tGraphID);
	}
#endif
	l_tpALifeDynamicObject1->o_Position		= l_tpALifeDynamicObject2->o_Position;
	l_tpALifeDynamicObject1->m_tNodeID		= l_tpALifeDynamicObject2->m_tNodeID;
	l_tpALifeDynamicObject1->m_tGraphID		= l_tpALifeDynamicObject2->m_tGraphID;
	l_tpALifeDynamicObject1->m_fDistance	= l_tpALifeDynamicObject2->m_fDistance;
	tpALifeInventoryItem->m_tPreviousParentID = l_tpALifeDynamicObject2->ID;

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Updating [%s] inventory with detached item [%s][%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID);
	}
#endif
	m_fCumulativeItemMass		-= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume		-= tpALifeInventoryItem->m_iVolume;
}

u32	CSE_ALifeTrader::dwfGetItemCost(CSE_ALifeInventoryItem *tpALifeInventoryItem)
{
#pragma todo("Dima to Dima : correct price for non-artefact objects")
	CSE_ALifeItemArtefact		*l_tpALifeItemArtefact = smart_cast<CSE_ALifeItemArtefact*>(tpALifeInventoryItem);
	if (!l_tpALifeItemArtefact)
		return					(tpALifeInventoryItem->m_dwCost);

	u32							l_dwPurchasedCount = 0;
#pragma todo("Dima to Dima : optimize this cycle by keeping additional data structure with bought items")
	{
		OBJECT_IT		i = children.begin();
		OBJECT_IT		e = children.end();
		for ( ; i != e; ++i)
			if (!xr_strcmp(ai().alife().objects().object(*i)->s_name,l_tpALifeItemArtefact->s_name))
				++l_dwPurchasedCount;
	}

	ARTEFACT_TRADER_ORDER_PAIR_IT	J = m_tpOrderedArtefacts.find(*tpALifeInventoryItem->base()->s_name);
	if ((m_tpOrderedArtefacts.end() != J) && (l_dwPurchasedCount < (*J).second->m_dwTotalCount)) {
		ARTEFACT_ORDER_IT		I = (*J).second->m_tpOrders.begin();
		ARTEFACT_ORDER_IT		E = (*J).second->m_tpOrders.end();
		for ( ; I != E; ++I) {
			if (l_dwPurchasedCount <= (*I).m_count)
				return			((*I).m_price);
			else
				l_dwPurchasedCount -= (*I).m_count;
		}
		Debug.fatal				("Data synchronization mismatch");
	}
	return						(tpALifeInventoryItem->m_dwCost);
}

void CSE_ALifeTrader::spawn_supplies	()
{
	inherited1::spawn_supplies	();
	inherited2::spawn_supplies	();
}