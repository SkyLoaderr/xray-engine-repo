////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_primary.cpp
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary evaluation function classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "weapon.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "alife_simulator.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "game_graph.h"
#include "inventory.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "clsid_game.h"
#include "ef_primary.h"

IC	CLASS_ID CBaseFunction::clsid_member_item() const
{
	CLASS_ID					result;
	if (ef_storage().non_alife().member_item())
		result					= ef_storage().non_alife().member_item()->SUB_CLS_ID;
	else {
		VERIFY2					(ef_storage().alife().member_item(),"No object specified for evaluation function");
		result					= ef_storage().alife().member_item()->m_tClassID;
	}
	return						(result);
}

IC	CLASS_ID CBaseFunction::clsid_enemy_item() const
{
	CLASS_ID					result;
	if (ef_storage().non_alife().enemy_item())
		result					= ef_storage().non_alife().enemy_item()->SUB_CLS_ID;
	else {
		VERIFY2					(ef_storage().alife().enemy_item(),"No object specified for evaluation function");
		result					= ef_storage().alife().enemy_item()->m_tClassID;
	}
	return						(result);
}

IC	CLASS_ID CBaseFunction::clsid_member() const
{
	CLASS_ID							result;
	if (ef_storage().non_alife().member())
		result							= ef_storage().non_alife().member()->SUB_CLS_ID;
	else {
		VERIFY2							(ef_storage().alife().member(),"No object specified for evaluation function");
		const CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ef_storage().alife().member());
		VERIFY2							(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function");
		result							= l_tpALifeDynamicObject->m_tClassID;
	}
	return								(result);
}

IC	CLASS_ID CBaseFunction::clsid_enemy() const
{
	CLASS_ID							result;
	if (ef_storage().non_alife().enemy())
		result							= ef_storage().non_alife().enemy()->SUB_CLS_ID;
	else {
		VERIFY2							(ef_storage().alife().enemy(),"No object specified for evaluation function");
		const CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ef_storage().alife().enemy());
		VERIFY2							(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function");
		result							= l_tpALifeDynamicObject->m_tClassID;
	}
	return								(result);
}

float CDistanceFunction::ffGetValue()
{
	if (ef_storage().non_alife().member())
		return(ef_storage().non_alife().member()->Position().distance_to(ef_storage().non_alife().enemy()->Position()));
	else
		return(ef_storage().alife().member()->base()->Position().distance_to(ef_storage().alife().enemy()->base()->Position()));
}

float CPersonalHealthFunction::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
		m_fMaxResultValue = ef_storage().non_alife().member()->g_MaxHealth();
		return(ef_storage().non_alife().member()->g_Health());
	}
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ef_storage().alife().member());
		VERIFY3			(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		m_fMaxResultValue	= l_tpALifeMonsterAbstract->g_MaxHealth();
		return(l_tpALifeMonsterAbstract->g_Health());
	}
}

float CPersonalMoraleFunction::ffGetValue()
{
	if (ef_storage().non_alife().member())
		return(ef_storage().non_alife().member()->m_fMorale);
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ef_storage().alife().member());
		VERIFY3			(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		return(l_tpALifeMonsterAbstract->m_fMorale);
	}
}

float CPersonalCreatureTypeFunction::ffGetValue()
{
	switch (clsid_member()) {
		case CLSID_AI_RAT				: return 1;
		case CLSID_AI_CAT:
		case CLSID_AI_RAT_WOLF			: return 2;
		case CLSID_AI_ZOMBIE			: return 3;
		case CLSID_AI_ZOMBIE_HUMAN		: return 4;
		case CLSID_AI_POLTERGEIST		: return 5;
		case CLSID_AI_DOG_RED			: return 6;
		case CLSID_AI_FLESH				: return 7;
		case CLSID_AI_DWARF				: return 8;
		case CLSID_AI_SCIENTIST			: return 9;
		case CLSID_AI_PHANTOM			: return 10;
		case CLSID_AI_SNORK				:
		case CLSID_AI_BOAR				: return 11;
		case CLSID_AI_CONTROLLER		: return 12;
		case CLSID_AI_BLOODSUCKER		: return 13;
		case CLSID_AI_SOLDIER			: return 14;
		case CLSID_AI_STALKER_DARK		: return 15;
		case CLSID_AI_STALKER_MILITARY	: return 16;
		case CLSID_OBJECT_ACTOR			: 
		case CLSID_AI_STALKER			: return 17;
		case CLSID_AI_BURER				: return 18;
		case CLSID_AI_GIANT				: return 19;
		case CLSID_AI_CHIMERA			: return 20;
		case CLSID_AI_FRACTURE			:
		case CLSID_AI_DOG_BLACK			: return 21;
		case CLSID_AI_TRADER			: return 1;
		default							: NODEFAULT; return 0;
	}
}

u32 CPersonalWeaponTypeFunction::dwfGetWeaponType()
{
	switch (clsid_member_item()) {
		case CLSID_GRENADE_F1:
		case CLSID_GRENADE_RGD5:
			return(10);
		case CLSID_OBJECT_W_RPG7:
		case CLSID_OBJECT_W_M134:
			return(9);
		case CLSID_OBJECT_W_FN2000:
		case CLSID_OBJECT_W_SVD:
		case CLSID_OBJECT_W_SVU:
		case CLSID_OBJECT_W_VINTOREZ:
			return(8);
		case CLSID_OBJECT_W_SHOTGUN:
			return(7);
		case CLSID_OBJECT_W_AK74:
		case CLSID_OBJECT_W_VAL:
		case CLSID_OBJECT_W_GROZA:
		case CLSID_OBJECT_W_LR300:
			return(6);
		case CLSID_OBJECT_W_HPSA:		
		case CLSID_OBJECT_W_PM:			
		case CLSID_OBJECT_W_FORT:		
		case CLSID_OBJECT_W_WALTHER:	
		case CLSID_OBJECT_W_USP45:
			return(5);
		case CLSID_IITEM_BOLT:
			return(1);
		default	:
			return(0);
	}
}

float CPersonalWeaponTypeFunction::ffGetTheBestWeapon() 
{
	u32 dwBestWeapon = 0;
	
	if (ef_storage().non_alife().member() && ef_storage().non_alife().member_item())
		return			(float(dwfGetWeaponType()));

	if (ef_storage().non_alife().member()) {
		const CInventoryOwner *tpInventoryOwner = smart_cast<const CInventoryOwner*>(ef_storage().non_alife().member());
		if (tpInventoryOwner) {
			xr_vector<CInventorySlot>::const_iterator I = tpInventoryOwner->inventory().m_slots.begin();
			xr_vector<CInventorySlot>::const_iterator E = tpInventoryOwner->inventory().m_slots.end();
			for ( ; I != E; ++I)
				if ((*I).m_pIItem) {
					CWeapon *tpCustomWeapon = smart_cast<CWeapon*>((*I).m_pIItem);
					if (tpCustomWeapon && (tpCustomWeapon->GetAmmoCurrent() > tpCustomWeapon->GetAmmoMagSize()/10)) {
						ef_storage().non_alife().member_item()	= tpCustomWeapon;
						u32 dwCurrentBestWeapon = dwfGetWeaponType();
						if (dwCurrentBestWeapon > dwBestWeapon)
							dwBestWeapon = dwCurrentBestWeapon;
						ef_storage().non_alife().member_item()	= 0;
					}
				}
		}	
	}
	else {
		if (!ef_storage().alife().member() || !ef_storage().alife().member()->m_tpCurrentBestWeapon)
			return(0);
		ef_storage().alife().member_item()	= ef_storage().alife().member()->m_tpCurrentBestWeapon;
		dwBestWeapon			= dwfGetWeaponType();
	}
	return(float(dwBestWeapon));
}

float CPersonalWeaponTypeFunction::ffGetValue()
{
	switch (clsid_member()) {
		case CLSID_AI_RAT				: return 1;
		case CLSID_AI_CAT:
		case CLSID_AI_RAT_WOLF			: return 2;
		case CLSID_AI_ZOMBIE			: return 1;
		case CLSID_AI_ZOMBIE_HUMAN		: return 1;
		case CLSID_AI_POLTERGEIST		: return 12;
		case CLSID_AI_DOG_RED			: return 2;
		case CLSID_AI_FLESH				: return 2;
		case CLSID_AI_DWARF				: return 1;
		case CLSID_AI_SCIENTIST			: return ffGetTheBestWeapon();
		case CLSID_AI_PHANTOM			: return 3;
		case CLSID_AI_SPONGER			: return 2;
		case CLSID_AI_CONTROLLER		: return 11;
		case CLSID_AI_BLOODSUCKER		: return 3;
		case CLSID_AI_SOLDIER			: return ffGetTheBestWeapon();
		case CLSID_AI_STALKER_DARK		: return ffGetTheBestWeapon();
		case CLSID_AI_STALKER_MILITARY	: return ffGetTheBestWeapon();
		case CLSID_OBJECT_ACTOR			: 
		case CLSID_AI_STALKER			: return ffGetTheBestWeapon();
		case CLSID_AI_BURER				: return 12;
		case CLSID_AI_GIANT				: return 3;
		case CLSID_AI_CHIMERA			: return 3;
		case CLSID_AI_FRACTURE			: return 4;
		case CLSID_AI_DOG_BLACK			: return 3;
		case CLSID_AI_SNORK				:
		case CLSID_AI_BOAR				: return 2;
		case CLSID_Z_TORRID				:
		case CLSID_Z_MBALD				: return 13;
		case CLSID_Z_MINCER				: return 14;
		case CLSID_Z_DEAD				:
		case CLSID_Z_RADIO				: return 15;
		case CLSID_Z_ACIDF				: return 16;
		case CLSID_Z_GALANT				: return 17;
		case CLSID_Z_BFUZZ				: return 18;
		case CLSID_Z_RUSTYH				: return 19;
		case CLSID_Z_FRYUP				: return 20;
		case CLSID_AI_TRADER			: return 1;
		default							: NODEFAULT; return(0);
	}
}

float CPersonalAccuracyFunction::ffGetValue()
{
	if (ef_storage().non_alife().member())
		return(ef_storage().non_alife().member()->m_fAccuracy);
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ef_storage().alife().member());
		VERIFY3						(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		return(l_tpALifeMonsterAbstract->m_fAccuracy);
	}
}

float CPersonalIntelligenceFunction::ffGetValue()
{
	if (ef_storage().non_alife().member())
		return(ef_storage().non_alife().member()->m_fIntelligence);
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ef_storage().alife().member());
		VERIFY3						(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		return(l_tpALifeMonsterAbstract->m_fIntelligence);
	}
}

float CPersonalRelationFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement relation function")
	return(0);
}

float CPersonalGreedFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement greed function")
	return(0);
}

float CPersonalAggressivenessFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement aggressiveness function")
	return(0);
}

float CEnemyEquipmentCostFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement enemy equipment cost function")
	return					(0);
}

float CEnemyRukzakWeightFunction::ffGetValue()
{
	float m_fLastValue;
	if (ef_storage().non_alife().member()) {
		const CInventoryOwner	*tpInventoryOwner = smart_cast<const CInventoryOwner*>(ef_storage().non_alife().member());
		if (tpInventoryOwner)
			m_fLastValue	= tpInventoryOwner->inventory().TotalWeight();
		else
			m_fLastValue	= 0;
	}
	else {
		CSE_ALifeHumanAbstract *l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ef_storage().alife().member());
		if (l_tpALifeHumanAbstract)
			m_fLastValue	= l_tpALifeHumanAbstract->m_fCumulativeItemMass;
		else
			m_fLastValue	= 0;
	}
	return					(m_fLastValue);
}

float CEnemyAnomalityFunction::ffGetValue()
{
#pragma todo("Dima to Dima : Implement enemy anomality function")
	return(0);
}

float CGraphPointType0::ffGetValue()
{
	return							(ai().game_graph().vertex(ef_storage().alife().member_item()->m_tGraphID)->vertex_type()[0]);
}

float CPersonalEyeRange::ffGetValue()
{
	const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ef_storage().alife().member());
	VERIFY3							(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
	return							(l_tpALifeMonsterAbstract->m_fEyeRange);
}

float CPersonalMaxHealth::ffGetValue()
{
	CSE_ALifeMonsterAbstract		*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(ef_storage().alife().member());
	VERIFY3							(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
	const CSE_ALifeGroupAbstract	*l_tpALifeGroupAbstract = smart_cast<const CSE_ALifeGroupAbstract*>(ef_storage().alife().member());
	if (!l_tpALifeGroupAbstract)
		return						(l_tpALifeMonsterAbstract->m_fMaxHealthValue);
	else
		return						(l_tpALifeMonsterAbstract->m_fMaxHealthValue*l_tpALifeGroupAbstract->m_wCount); 
}

u32 CPersonalMaxHealth::dwfGetDiscreteValue(u32 dwDiscretizationValue)
{
	float fTemp = ffGetValue();
	if (fTemp <= m_fMinResultValue)
		return(0);
	else
		if (fTemp >= m_fMaxResultValue)
			return(dwDiscretizationValue - 1);
		else {
			if (fTemp <= 30)
				return(iFloor(1*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 50)
				return(iFloor(2*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 80)
				return(iFloor(3*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 100)
				return(iFloor(4*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 150)
				return(iFloor(5*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 250)
				return(iFloor(6*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 500)
				return(iFloor(7*float(dwDiscretizationValue)/10 + .5f));
			if (fTemp <= 750)
				return(iFloor(8*float(dwDiscretizationValue)/10 + .5f));
			return(iFloor(9*float(dwDiscretizationValue)/10 + .5f));
		}
}

float CEquipmentType::ffGetValue()
{
	switch (clsid_member_item()) {
		case CLSID_EQUIPMENT_SIMPLE		: return 1;
		case CLSID_EQUIPMENT_SCIENTIFIC	: return 2;
		case CLSID_EQUIPMENT_STALKER	: return 3;
		case CLSID_EQUIPMENT_MILITARY	: return 4;
		case CLSID_EQUIPMENT_EXO		: return 5;
		default							: return 6;
	}
}

float CItemDeterioration::ffGetValue()
{
	if (ef_storage().non_alife().member_item()) {
#pragma todo("Dima to Dima : Append ItemDeterioration with non-ALife branch")
		return					(0);
	}
	else {
		const CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<const CSE_ALifeInventoryItem*>(ef_storage().alife().member_item());
		R_ASSERT2				(l_tpALifeInventoryItem,"Non-item object specified for the ItemDeterioration evaluation function");
		return					(l_tpALifeInventoryItem->m_fDeteriorationValue);
	}
}

float CEquipmentPreference::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append EquipmentPreference with non-ALife branch")
		return					(0);
	}
	else {
		CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ef_storage().alife().member());
		R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human object in EquipmentPreference evaluation function");
		return					(l_tpALifeHumanAbstract->m_cpEquipmentPreferences[ef_storage().m_pfEquipmentType->dwfGetDiscreteValue()]);
	}
}

float CMainWeaponType::ffGetValue()
{
	switch (clsid_member_item()) {
		case CLSID_OBJECT_W_SHOTGUN	: return 1.f;
		case CLSID_OBJECT_W_AK74	:
		case CLSID_OBJECT_W_VAL		:
		case CLSID_OBJECT_W_LR300	:
		case CLSID_OBJECT_W_GROZA	: return 2.f;
		case CLSID_OBJECT_W_FN2000	:
		case CLSID_OBJECT_W_SVD		:
		case CLSID_OBJECT_W_SVU		:
		case CLSID_OBJECT_W_VINTOREZ: return 3.f;
		case CLSID_OBJECT_W_RPG7	:
		case CLSID_OBJECT_W_M134	: return 4.f;
		default						: return 5.f;
	}
}

float CMainWeaponPreference::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append MainWeaponPreference with non-ALife branch")
		return					(0);
	}
	else {
		CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ef_storage().alife().member());
		R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human object in EquipmentPreference evaluation function");
		return					(l_tpALifeHumanAbstract->m_cpMainWeaponPreferences[ef_storage().m_pfMainWeaponType->dwfGetDiscreteValue(iFloor(ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f))]);
	}
}

float CItemValue::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append ItemValue with non-ALife branch")
		return					(0);
	}
	else {
		const CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<const CSE_ALifeInventoryItem*>(ef_storage().alife().member_item());
		R_ASSERT2				(l_tpALifeInventoryItem,"Non-item object specified for the ItemDeterioration evaluation function");
		return					(float(l_tpALifeInventoryItem->m_dwCost));
	}
}

float CWeaponAmmoCount::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
#pragma todo("Dima to Dima : Append WeaponAmmoCount with non-ALife branch")
		return					(0);
	}
	else {
		CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ef_storage().alife().member());
		R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human object in WeaponAmmoCount evaluation function");
		return					(l_tpALifeHumanAbstract->get_available_ammo_count(smart_cast<const CSE_ALifeItemWeapon*>(ef_storage().alife().member_item()),l_tpALifeHumanAbstract->alife().m_temp_item_vector));
	}
}

u32	 CWeaponAmmoCount::dwfGetDiscreteValue(u32 dwDiscretizationValue)
{
	float fTemp = ffGetValue();
	if (fTemp <= m_fMinResultValue)
		return(0);
	else
		if (fTemp >= m_fMaxResultValue)
			return(dwDiscretizationValue - 1);
		else {
			const CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = smart_cast<const CSE_ALifeItemWeapon*>(ef_storage().alife().member_item());
			if (l_tpALifeItemWeapon && l_tpALifeItemWeapon->m_caAmmoSections) {
				string32		S;
				_GetItem		(l_tpALifeItemWeapon->m_caAmmoSections,0,S);
				u32				l_dwBoxSize = pSettings->r_s32(S,"box_size");
				if (fTemp <= 3*l_dwBoxSize)
					return(iFloor(1*float(dwDiscretizationValue)/10 + .5f));
				return(iFloor(2*float(dwDiscretizationValue)/10 + .5f));
			}
			else
				return(dwDiscretizationValue - 1);
		}
}

float CEnemyAnomalyType::ffGetValue()
{
	switch (clsid_enemy()) {
		case CLSID_Z_TORRID :
		case CLSID_Z_MBALD	: return 1;
		case CLSID_Z_MINCER : return 2;
		case CLSID_Z_DEAD	:
		case CLSID_Z_RADIO	: return 3;
		case CLSID_Z_ACIDF	: return 4;
		case CLSID_Z_GALANT : return 5;
		case CLSID_Z_BFUZZ	: return 6;
		case CLSID_Z_RUSTYH : return 7;
		case CLSID_Z_FRYUP	: return 8;
		default				: NODEFAULT; return(0);
	}
}

float CDetectorType::ffGetValue()
{
	if (ef_storage().non_alife().member()) {
		if (!ef_storage().non_alife().member_item())
			return				(0);
	}
	else {
		if (!ef_storage().alife().member_item())
			return				(0);
	}

	switch (clsid_member_item()) {
		case CLSID_AI_RAT				: 
		case CLSID_AI_RAT_WOLF			: 
		case CLSID_AI_ZOMBIE			: 
		case CLSID_AI_ZOMBIE_HUMAN		: 
		case CLSID_AI_POLTERGEIST		: 
		case CLSID_AI_FLESH				: 
		case CLSID_AI_DWARF				: 
		case CLSID_AI_SCIENTIST			: 
		case CLSID_AI_PHANTOM			: 
		case CLSID_AI_SPONGER			: 
		case CLSID_AI_CONTROLLER		: 
		case CLSID_AI_BLOODSUCKER		: 
		case CLSID_AI_BURER				: 
		case CLSID_AI_GIANT				: 
		case CLSID_AI_CHIMERA			: 
		case CLSID_AI_FRACTURE			: 
		case CLSID_AI_DOG_RED			:
		case CLSID_AI_DOG_BLACK			: 
		case CLSID_AI_BOAR				: 
		case CLSID_AI_SNORK				:
		case CLSID_AI_CAT				:
		case CLSID_DETECTOR_SIMPLE		: return 2;
		case CLSID_DETECTOR_VISUAL		: return 3;
		default							: NODEFAULT; return(0.f);
	}
}

float CEnemyDistanceToGraphPoint::ffGetValue()
{
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(ef_storage().alife().enemy());
	R_ASSERT3				(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function ",m_caName);
	if (l_tpALifeDynamicObject->m_fDistance < 5.f)
		return				(0);
	if (l_tpALifeDynamicObject->m_fDistance < 10.f)
		return				(1);
	if (l_tpALifeDynamicObject->m_fDistance < 15.f)
		return				(2);
	if (l_tpALifeDynamicObject->m_fDistance < 20.f)
		return				(3);
	return					(4);
}
