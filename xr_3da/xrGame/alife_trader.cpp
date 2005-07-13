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
#include "alife_artefact_order.h"
#include "ai_debug.h"

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
		float					p;
		for (u32 k = 0, j; ini.r_line("spawn",k,&N,&V); k++) {
			VERIFY				(xr_strlen(N));
	
			float f_cond						= 1.0f;
			bool bScope							= false;
			bool bSilencer						= false;
			bool bLauncher						= false;

			
			j					= 1;
			p					= 1.f;
			
			if (V && xr_strlen(V)) {
				string64			buf;
				j					= atoi(_GetItem(V, 0, buf));
				if (!j)		j		= 1;

				bScope		=	(NULL!=strstr(V,"scope"));
				bSilencer	=	(NULL!=strstr(V,"silencer"));
				bLauncher	=	(NULL!=strstr(V,"launcher"));
			//probability
			if(NULL!=strstr(V,"prob=")){
				string16						c_prob;
				sscanf							(strstr(V,"prob=")+5,"%[^ ] ",c_prob);
				p								=(float)atof(c_prob);
			}			

			if (fis_zero(p))
				p								= 1.0f;

			if(NULL!=strstr(V,"cond=")){
				string16						c_cond;
				sscanf							(strstr(V,"cond=")+5,"%[^ ] ",c_cond);
				f_cond							= (float)atof(c_cond);
			}
			}
			for (u32 i=0; i<j; ++i) {
				if (randF(1.f) < p) {
					CSE_Abstract* E = alife().spawn_item	(N,o_Position,m_tNodeID,m_tGraphID,ID);
					//подсоединить аддоны к оружию, если включены соответствующие флажки
					CSE_ALifeItemWeapon* W =  smart_cast<CSE_ALifeItemWeapon*>(E);
					if (W) {
						if (W->m_scope_status == CSE_ALifeItemWeapon::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
						if (W->m_silencer_status == CSE_ALifeItemWeapon::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
						if (W->m_grenade_launcher_status == CSE_ALifeItemWeapon::eAddonAttachable)
							W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
					}
					CSE_ALifeInventoryItem* IItem = smart_cast<CSE_ALifeInventoryItem*>(E);
					if(IItem)
						IItem->m_fCondition				= f_cond;
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
	character_profile();
	pda->m_specific_character = specific_character();
#endif

	if(m_SpecificCharacter.size())
	{
		//если в custom data объекта есть
		//секция [dont_spawn_character_supplies]
		//то не вызывать spawn из selected_char.SupplySpawn()
		bool specific_character_supply = true;	

		if (xr_strlen(dynamic_object->m_ini_string))
		{
#pragma warning(push)
#pragma warning(disable:4238)
			CInifile					ini(
				&IReader				(
				(void*)(*dynamic_object->m_ini_string),
				xr_strlen(dynamic_object->m_ini_string)
				)
				);
#pragma warning(pop)

			if (ini.section_exist("dont_spawn_character_supplies")) 
				specific_character_supply = false;
		}

		if(specific_character_supply)
		{
			CSpecificCharacter selected_char;
			selected_char.Load(m_SpecificCharacter);
			dynamic_object->spawn_supplies(selected_char.SupplySpawn());
		}
	}
}

void CSE_ALifeTraderAbstract::vfInitInventory()
{
	m_fCumulativeItemMass		= 0.f;
	m_iCumulativeItemVolume		= 0;
}

#ifdef DEBUG
bool CSE_ALifeTraderAbstract::check_inventory_consistency	()
{
	int							volume = 0;
	float						mass = 0.f;
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = base()->children.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = base()->children.end();
	for ( ; I != E; ++I) {
		CSE_ALifeDynamicObject	*object = ai().alife().objects().object(*I,true);
		if (!object)
			continue;

		CSE_ALifeInventoryItem	*item = smart_cast<CSE_ALifeInventoryItem*>(object);
		if (!item)
			continue;

		volume					+= item->m_iVolume;
		mass					+= item->m_fMass;
	}

	VERIFY2						(fis_zero(m_fCumulativeItemMass - mass,EPS_L),base()->name_replace());
	if (!fis_zero(m_fCumulativeItemMass - mass,EPS_L))
		return					(false);

	VERIFY2						(m_iCumulativeItemVolume == volume,base()->name_replace());
	if (m_iCumulativeItemVolume != volume)
		return					(false);

#ifdef DEBUG
//	if (psAI_Flags.test(aiALife)) {
//		Msg						("[LSS] [%s] inventory is consistent [%f][%d]",base()->name_replace(),mass,volume);
//	}
#endif

	return						(true);
}
#endif

void CSE_ALifeTraderAbstract::attach	(CSE_ALifeInventoryItem *tpALifeInventoryItem, bool bALifeRequest, bool bAddChildren)
{
	VERIFY						(!bAddChildren || check_inventory_consistency());

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

	VERIFY								(m_fCumulativeItemMass >= 0.f);
	VERIFY								(!bALifeRequest || !bAddChildren || check_inventory_consistency());
}

void CSE_ALifeTraderAbstract::detach(CSE_ALifeInventoryItem *tpALifeInventoryItem, OBJECT_IT *I, bool bALifeRequest, bool bRemoveChildren)
{
	VERIFY								(!bRemoveChildren || check_inventory_consistency());
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

//	if(!(fis_zero(m_fCumulativeItemMass) || m_fCumulativeItemMass > 0.f)){
//		Msg("ERROR !!! m_fCumulativeItemMass = %f",m_fCumulativeItemMass);
//	}
//	VERIFY									(m_fCumulativeItemMass >= 0.f);
	VERIFY									(!bALifeRequest || check_inventory_consistency());
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