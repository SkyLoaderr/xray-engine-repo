////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_primary.cpp
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary evaluation function classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "weapon.h"
#include "ai_space.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "alife_simulator.h"
#include "ef_storage.h"
#include "game_graph.h"
#include "inventory.h"
#include "xrServer_Objects_ALife_Monsters.h"

float CDistanceFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember)
		return(m_fLastValue = ai().ef_storage().m_tpCurrentMember->Position().distance_to(ai().ef_storage().m_tpCurrentEnemy->Position()));
	else
		return(m_fLastValue = ai().ef_storage().m_tpCurrentALifeMember->base()->Position().distance_to(ai().ef_storage().m_tpCurrentALifeEnemy->base()->Position()));
}

float CPersonalHealthFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
		m_fMaxResultValue = ai().ef_storage().m_tpCurrentMember->g_MaxHealth();
		return(m_fLastValue = ai().ef_storage().m_tpCurrentMember->g_Health());
	}
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		VERIFY3			(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		m_fMaxResultValue	= l_tpALifeMonsterAbstract->g_MaxHealth();
		return(m_fLastValue = l_tpALifeMonsterAbstract->g_Health());
	}
}

float CPersonalMoraleFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember)
		return(m_fLastValue = ai().ef_storage().m_tpCurrentMember->m_fMorale);
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		VERIFY3			(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		return(m_fLastValue = l_tpALifeMonsterAbstract->m_fMorale);
	}
}

float CPersonalCreatureTypeFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	
	CLASS_ID	l_tClassID;
	if (ai().ef_storage().m_tpCurrentMember) {
		l_tClassID = ai().ef_storage().m_tpCurrentMember->SUB_CLS_ID;
	}
	else {
		const CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ai().ef_storage().m_tpCurrentALifeMember);
		VERIFY3							(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function ",m_caName);
		l_tClassID = l_tpALifeDynamicObject->m_tClassID;
	}
	switch (l_tClassID) {
		case CLSID_AI_RAT				: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_RAT_WOLF			: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_ZOMBIE			: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_ZOMBIE_HUMAN		: {
			m_fLastValue =  4;
			break;
		}
		case CLSID_AI_POLTERGEIST		: {
			m_fLastValue =  5;
			break;
		}
		case CLSID_AI_DOG_RED			: {
			m_fLastValue =  6;
			break;
		}
		case CLSID_AI_FLESH				: {
			m_fLastValue =  7;
			break;
		}
		case CLSID_AI_DWARF				: {
			m_fLastValue =  8;
			break;
		}
		case CLSID_AI_SCIENTIST			: {
			m_fLastValue =  9;
			break;
		}
		case CLSID_AI_PHANTOM			: {
			m_fLastValue = 10;
			break;
		}
		case CLSID_AI_BOAR				: {
			m_fLastValue = 11;
			break;
		}
		case CLSID_AI_CONTROLLER		: {
			m_fLastValue = 12;
			break;
		}
		case CLSID_AI_BLOODSUCKER		: {
			m_fLastValue = 13;
			break;
		}
		case CLSID_AI_SOLDIER			: {
			m_fLastValue = 14;
			break;
		}
		case CLSID_AI_STALKER_DARK		: {
			m_fLastValue = 15;
			break;
		}
		case CLSID_AI_STALKER_MILITARY	: {
			m_fLastValue = 16;
			break;
		}
		case CLSID_OBJECT_ACTOR			: 
		case CLSID_AI_STALKER			: {
			m_fLastValue = 17;
			break;
		}
		case CLSID_AI_BURER				: {
			m_fLastValue = 18;
			break;
		}
		case CLSID_AI_GIANT				: {
			m_fLastValue = 19;
			break;
		}
		case CLSID_AI_CHIMERA	: {
			m_fLastValue = 20;
			break;
		}
		case CLSID_AI_FRACTURE	:
		case CLSID_AI_DOG_BLACK	: {
			m_fLastValue = 21;
			break;
		}
		case CLSID_AI_TRADER: break;

		default : NODEFAULT;
	}
	return(m_fLastValue);
}

u32 CPersonalWeaponTypeFunction::dwfGetWeaponType()
{
	VERIFY2		(ai().ef_storage().m_tpGameObject || ai().ef_storage().m_tpCurrentALifeObject,"No weapon passed to the PersonalWeaponType evaluation function");
	CLASS_ID	l_tClassID = ai().ef_storage().m_tpGameObject ? ai().ef_storage().m_tpGameObject->SUB_CLS_ID : ai().ef_storage().m_tpCurrentALifeObject->m_tClassID;
	switch (l_tClassID) {
		case CLSID_OBJECT_W_RPG7:
		case CLSID_OBJECT_W_M134:
			return(9);
		case CLSID_OBJECT_W_FN2000:
		case CLSID_OBJECT_W_SVD:
		case CLSID_OBJECT_W_SVU:
		case CLSID_OBJECT_W_VINTOREZ:
			return(8);
		case CLSID_GRENADE_F1:
		case CLSID_GRENADE_RGD5:
			return(7);
		case CLSID_OBJECT_W_SHOTGUN:
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
	
	if (ai().ef_storage().m_tpCurrentMember && ai().ef_storage().m_tpGameObject)
		return			(float(dwfGetWeaponType()));

	if (ai().ef_storage().m_tpCurrentMember) {
		const CInventoryOwner *tpInventoryOwner = smart_cast<const CInventoryOwner*>(ai().ef_storage().m_tpCurrentMember);
		if (tpInventoryOwner) {
			xr_vector<CInventorySlot>::const_iterator I = tpInventoryOwner->inventory().m_slots.begin();
			xr_vector<CInventorySlot>::const_iterator E = tpInventoryOwner->inventory().m_slots.end();
			for ( ; I != E; ++I)
				if ((*I).m_pIItem) {
					CWeapon *tpCustomWeapon = smart_cast<CWeapon*>((*I).m_pIItem);
					if (tpCustomWeapon && (tpCustomWeapon->GetAmmoCurrent() > tpCustomWeapon->GetAmmoMagSize()/10)) {
						ai().ef_storage().m_tpGameObject	= tpCustomWeapon;
						u32 dwCurrentBestWeapon = dwfGetWeaponType();
						if (dwCurrentBestWeapon > dwBestWeapon)
							dwBestWeapon = dwCurrentBestWeapon;
						ai().ef_storage().m_tpGameObject	= 0;
					}
				}
		}	
	}
	else {
		if (!ai().ef_storage().m_tpCurrentALifeMember || !ai().ef_storage().m_tpCurrentALifeMember->m_tpCurrentBestWeapon)
			return(0);
		ai().ef_storage().m_tpCurrentALifeObject	= ai().ef_storage().m_tpCurrentALifeMember->m_tpCurrentBestWeapon;
		dwBestWeapon			= dwfGetWeaponType();
	}
	return(float(dwBestWeapon));
}

float CPersonalWeaponTypeFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	
	CLASS_ID	l_tClassID;
	if (ai().ef_storage().m_tpCurrentMember) {
		l_tClassID = ai().ef_storage().m_tpCurrentMember->SUB_CLS_ID;
	}
	else {
		const CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ai().ef_storage().m_tpCurrentALifeMember);
		VERIFY3							(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function ",m_caName);
		l_tClassID = l_tpALifeDynamicObject->m_tClassID;
	}
	switch (l_tClassID) {
		case CLSID_AI_RAT				: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_RAT_WOLF			: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_ZOMBIE			: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_ZOMBIE_HUMAN		: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_POLTERGEIST		: {
			// 1 or 12
			m_fLastValue =  12;
			break;
		}
		case CLSID_AI_DOG_RED	: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_FLESH				: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_DWARF				: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_SCIENTIST			: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_PHANTOM			: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_SPONGER			: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_CONTROLLER		: {
			//2 or 11
			m_fLastValue =  11;
			break;
		}
		case CLSID_AI_BLOODSUCKER		: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_SOLDIER			: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_STALKER_DARK		: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_STALKER_MILITARY	: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_OBJECT_ACTOR			: 
		case CLSID_AI_STALKER			: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_BURER				: {
			m_fLastValue =  12;
			break;
		}
		case CLSID_AI_GIANT				: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_CHIMERA	: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_FRACTURE	: {
			m_fLastValue =  4;
			break;
		}
		case CLSID_AI_DOG_BLACK	: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_TRADER: 
			break;
		case CLSID_AI_BOAR: 
			m_fLastValue =  2;
			break;
		case CLSID_Z_MBALD : {
			m_fLastValue =  13;
			break;
		}
		case CLSID_Z_MINCER : {
			m_fLastValue =  14;
			break;
		}
		case CLSID_Z_RADIO : {
			m_fLastValue =  15;
			break;
		}
		case CLSID_Z_ACIDF : {
			m_fLastValue =  16;
			break;
		}
		case CLSID_Z_GALANT : {
			m_fLastValue =  17;
			break;
		}
		case CLSID_Z_BFUZZ : {
			m_fLastValue =  18;
			break;
		}
		case CLSID_Z_RUSTYH : {
			m_fLastValue =  19;
			break;
		}
		case CLSID_Z_FRYUP : {
			m_fLastValue =  20;
			break;
		}
		default : NODEFAULT;
	}
	return(m_fLastValue -= 1.f);
}

float CPersonalAccuracyFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember)
		return(m_fLastValue = ai().ef_storage().m_tpCurrentMember->m_fAccuracy);
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		VERIFY3						(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		return(m_fLastValue = l_tpALifeMonsterAbstract->m_fAccuracy);
	}
}

float CPersonalIntelligenceFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember)
		return(m_fLastValue = ai().ef_storage().m_tpCurrentMember->m_fIntelligence);
	else {
		const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		VERIFY3						(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
		return(m_fLastValue = l_tpALifeMonsterAbstract->m_fIntelligence);
	}
}

float CPersonalRelationFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
#pragma todo("Dima to Dima : Implement relation function")
	return(m_fLastValue = 0);
}

float CPersonalGreedFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
#pragma todo("Dima to Dima : Implement greed function")
	return(m_fLastValue = 0);
}

float CPersonalAggressivenessFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
#pragma todo("Dima to Dima : Implement aggressiveness function")
	return(m_fLastValue = 0);
}

// enemy inversion functions	
float CEnemyHealthFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return							(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
		const CEntityAlive *tpEntity			= ai().ef_storage().m_tpCurrentMember;
		ai().ef_storage().m_tpCurrentMember		= ai().ef_storage().m_tpCurrentEnemy;
		m_fLastValue							= ai().ef_storage().m_pfPersonalHealth->ffGetValue();
		ai().ef_storage().m_tpCurrentMember		= tpEntity;
	}
	else {
		CSE_ALifeSchedulable						*l_tpALifeSchedulable = ai().ef_storage().m_tpCurrentALifeMember;
		ai().ef_storage().m_tpCurrentALifeMember	= ai().ef_storage().m_tpCurrentALifeEnemy;
		m_fLastValue								= ai().ef_storage().m_pfPersonalHealth->ffGetValue();
		ai().ef_storage().m_tpCurrentALifeMember	= l_tpALifeSchedulable;
	}
	return								(m_fLastValue);
}

float CEnemyCreatureTypeFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return										(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
		const CEntityAlive *tpEntity				= ai().ef_storage().m_tpCurrentMember;
		ai().ef_storage().m_tpCurrentMember			= ai().ef_storage().m_tpCurrentEnemy;
		m_fLastValue								= ai().ef_storage().m_pfPersonalCreatureType->ffGetValue();
		ai().ef_storage().m_tpCurrentMember			= tpEntity;
	}
	else {
		CSE_ALifeSchedulable						*l_tpALifeSchedulable = ai().ef_storage().m_tpCurrentALifeMember;
		ai().ef_storage().m_tpCurrentALifeMember	= ai().ef_storage().m_tpCurrentALifeEnemy;
		m_fLastValue								= ai().ef_storage().m_pfPersonalCreatureType->ffGetValue();
		ai().ef_storage().m_tpCurrentALifeMember	= l_tpALifeSchedulable;
	}
	return											(m_fLastValue);
}

float CEnemyWeaponTypeFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return							(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
		const CEntityAlive					*tpEntity = ai().ef_storage().m_tpCurrentMember;
		const CGameObject					*l_tpGameObject = ai().ef_storage().m_tpGameObject;
		ai().ef_storage().m_tpCurrentMember	= ai().ef_storage().m_tpCurrentEnemy;
		m_fLastValue						= ai().ef_storage().m_pfPersonalWeaponType->ffGetValue();
		ai().ef_storage().m_tpCurrentMember	= tpEntity;
		ai().ef_storage().m_tpGameObject	= l_tpGameObject;
	}
	else {
		CSE_ALifeSchedulable						*l_tpALifeSchedulable = ai().ef_storage().m_tpCurrentALifeMember;
		const CSE_ALifeObject						*l_tpALifeObject = ai().ef_storage().m_tpCurrentALifeObject;
		ai().ef_storage().m_tpCurrentALifeObject	= ai().ef_storage().m_tpCurrentALifeEnemy->m_tpCurrentBestWeapon;
		ai().ef_storage().m_tpCurrentALifeMember	= ai().ef_storage().m_tpCurrentALifeEnemy;
		m_fLastValue								= ai().ef_storage().m_pfPersonalWeaponType->ffGetValue();
		ai().ef_storage().m_tpCurrentALifeMember	= l_tpALifeSchedulable;
		ai().ef_storage().m_tpCurrentALifeObject	= l_tpALifeObject;
	}
	return								(m_fLastValue);
}

float CEnemyEquipmentCostFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
#pragma todo("Dima to Dima : Implement enemy equipment cost function")
	return					(m_fLastValue = 0);
}

float CEnemyRukzakWeightFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	
	if (ai().ef_storage().m_tpCurrentMember) {
		const CInventoryOwner	*tpInventoryOwner = smart_cast<const CInventoryOwner*>(ai().ef_storage().m_tpCurrentMember);
		if (tpInventoryOwner)
			m_fLastValue	= tpInventoryOwner->inventory().TotalWeight();
		else
			m_fLastValue	= 0;
	}
	else {
		CSE_ALifeHumanAbstract *l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		if (l_tpALifeHumanAbstract)
			m_fLastValue	= l_tpALifeHumanAbstract->m_fCumulativeItemMass;
		else
			m_fLastValue	= 0;
	}
	return					(m_fLastValue);
}

float CEnemyAnomalityFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
#pragma todo("Dima to Dima : Implement enemy anomality function")
	return(m_fLastValue = 0);
}

float CGraphPointType0::ffGetValue()
{
	if (bfCheckForCachedResult())
		return						(m_fLastValue);
	return							(m_fLastValue = ai().game_graph().vertex(ai().ef_storage().m_tpCurrentALifeObject->m_tGraphID)->vertex_type()[0]);
}

float CPersonalEyeRange::ffGetValue()
{
	if (bfCheckForCachedResult())
		return						(m_fLastValue);
	const CSE_ALifeMonsterAbstract	*l_tpALifeMonsterAbstract = smart_cast<const CSE_ALifeMonsterAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
	VERIFY3							(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
	return							(m_fLastValue = l_tpALifeMonsterAbstract->m_fEyeRange);
}

float CEnemyEyeRange::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);

	CSE_ALifeSchedulable			*l_tpALifeSchedulable = ai().ef_storage().m_tpCurrentALifeMember;
	ai().ef_storage().m_tpCurrentALifeMember	= ai().ef_storage().m_tpCurrentALifeEnemy;
	m_fLastValue					= ai().ef_storage().m_pfPersonalEyeRange->ffGetValue();
	ai().ef_storage().m_tpCurrentALifeMember	= l_tpALifeSchedulable;
	return							(m_fLastValue);
}

float CPersonalMaxHealth::ffGetValue()
{
	if (bfCheckForCachedResult())
		return						(m_fLastValue);
	
	CSE_ALifeMonsterAbstract		*l_tpALifeMonsterAbstract = smart_cast<CSE_ALifeMonsterAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
	VERIFY3							(l_tpALifeMonsterAbstract,"Invalid object passed to the evaluation function ",m_caName);
	const CSE_ALifeGroupAbstract	*l_tpALifeGroupAbstract = smart_cast<const CSE_ALifeGroupAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
	if (!l_tpALifeGroupAbstract)
		return						(m_fLastValue = l_tpALifeMonsterAbstract->m_fMaxHealthValue);
	else
		return						(m_fLastValue = l_tpALifeMonsterAbstract->m_fMaxHealthValue*l_tpALifeGroupAbstract->m_wCount); 
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

float CEnemyMaxHealth::ffGetValue()
{
	if (bfCheckForCachedResult())
		return						(m_fLastValue);

	CSE_ALifeSchedulable			*l_tpALifeSchedulable = ai().ef_storage().m_tpCurrentALifeMember;
	ai().ef_storage().m_tpCurrentALifeMember	= ai().ef_storage().m_tpCurrentALifeEnemy;
	m_fLastValue					= ai().ef_storage().m_pfPersonalMaxHealth->ffGetValue();
	ai().ef_storage().m_tpCurrentALifeMember	= l_tpALifeSchedulable;
	return							(m_fLastValue);
}

u32 CEnemyMaxHealth::dwfGetDiscreteValue(u32 dwDiscretizationValue)
{
	CSE_ALifeSchedulable			*l_tpALifeSchedulable = ai().ef_storage().m_tpCurrentALifeMember;
	ai().ef_storage().m_tpCurrentALifeMember	= ai().ef_storage().m_tpCurrentALifeEnemy;
	u32								l_dwResult = ai().ef_storage().m_pfPersonalMaxHealth->dwfGetDiscreteValue(dwDiscretizationValue);
	ai().ef_storage().m_tpCurrentALifeMember	= l_tpALifeSchedulable;
	return							(l_dwResult);
}

float CEquipmentType::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	
	if (ai().ef_storage().m_tpGameObject) {
#pragma todo("Dima to Dima : Append EquipmentType with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		VERIFY2					(ai().ef_storage().m_tpCurrentALifeObject,"No object specified for EquipmentType evaluation function");
		switch (ai().ef_storage().m_tpCurrentALifeObject->m_tClassID) {
			case CLSID_EQUIPMENT_SIMPLE		: {
				m_fLastValue = 1;
				break;
			}
			case CLSID_EQUIPMENT_SCIENTIFIC	: {
				m_fLastValue = 2;
				break;
			}
			case CLSID_EQUIPMENT_STALKER	: {	
				m_fLastValue = 3;
				break;
			}
			case CLSID_EQUIPMENT_MILITARY	: {
				m_fLastValue = 4;
				break;
			}
			case CLSID_EQUIPMENT_EXO		: {	
				m_fLastValue = 5;
				break;
			}
			default							: {
				m_fLastValue = 6;
				break;
			}
		}
		return					(m_fLastValue);
	}
}

float CItemDeterioration::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpGameObject) {
#pragma todo("Dima to Dima : Append ItemDeterioration with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		const CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<const CSE_ALifeInventoryItem*>(ai().ef_storage().m_tpCurrentALifeObject);
		R_ASSERT2				(l_tpALifeInventoryItem,"Non-item object specified for the ItemDeterioration evaluation function");
		return					(m_fLastValue = l_tpALifeInventoryItem->m_fDeteriorationValue);
	}
}

float CEquipmentPreference::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
#pragma todo("Dima to Dima : Append EquipmentPreference with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human object in EquipmentPreference evaluation function");
		return					(m_fLastValue = l_tpALifeHumanAbstract->m_cpEquipmentPreferences[ai().ef_storage().m_pfEquipmentType->dwfGetDiscreteValue()]);
	}
}

float CMainWeaponType::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
#pragma todo("Dima to Dima : Append MainWeaponType with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		R_ASSERT2				(ai().ef_storage().m_tpCurrentALifeObject,"No object specified for MainWeaponType evaluation function");
		switch (ai().ef_storage().m_tpCurrentALifeObject->m_tClassID) {
			case CLSID_OBJECT_W_SHOTGUN: {
				m_fLastValue	= 1;
				break;
			}
			case CLSID_OBJECT_W_AK74:
			case CLSID_OBJECT_W_VAL:
			case CLSID_OBJECT_W_LR300: {
			case CLSID_OBJECT_W_GROZA:
				m_fLastValue	= 2.f;
				break;
			}
			case CLSID_OBJECT_W_FN2000:
			case CLSID_OBJECT_W_SVD:
			case CLSID_OBJECT_W_SVU:
			case CLSID_OBJECT_W_VINTOREZ: {
				m_fLastValue	= 3.f;
				break;
			}
			case CLSID_OBJECT_W_RPG7:
			case CLSID_OBJECT_W_M134: {
				m_fLastValue	= 4;
				break;
			}
			default				: {
				m_fLastValue	= 5;
				break;
			}
		}
		return					(m_fLastValue);
	}
}

float CMainWeaponPreference::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
#pragma todo("Dima to Dima : Append MainWeaponPreference with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human object in EquipmentPreference evaluation function");
		return					(m_fLastValue = l_tpALifeHumanAbstract->m_cpMainWeaponPreferences[ai().ef_storage().m_pfMainWeaponType->dwfGetDiscreteValue(iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f))]);
	}
}

float CItemValue::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
#pragma todo("Dima to Dima : Append ItemValue with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		const CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<const CSE_ALifeInventoryItem*>(ai().ef_storage().m_tpCurrentALifeObject);
		R_ASSERT2				(l_tpALifeInventoryItem,"Non-item object specified for the ItemDeterioration evaluation function");
		return					(m_fLastValue = float(l_tpALifeInventoryItem->m_dwCost));
	}
}

float CWeaponAmmoCount::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);
	if (ai().ef_storage().m_tpCurrentMember) {
#pragma todo("Dima to Dima : Append WeaponAmmoCount with non-ALife branch")
		return					(m_fLastValue);
	}
	else {
		CSE_ALifeHumanAbstract	*l_tpALifeHumanAbstract = smart_cast<CSE_ALifeHumanAbstract*>(ai().ef_storage().m_tpCurrentALifeMember);
		R_ASSERT2				(l_tpALifeHumanAbstract,"Non-human object in WeaponAmmoCount evaluation function");
		return					(m_fLastValue = l_tpALifeHumanAbstract->get_available_ammo_count(smart_cast<const CSE_ALifeItemWeapon*>(ai().ef_storage().m_tpCurrentALifeObject),l_tpALifeHumanAbstract->alife().m_temp_item_vector));
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
			const CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = smart_cast<const CSE_ALifeItemWeapon*>(ai().ef_storage().m_tpCurrentALifeObject);
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
	if (bfCheckForCachedResult())
		return					(m_fLastValue);

	CLASS_ID					l_tClassID;
	if (ai().ef_storage().m_tpCurrentMember)
		l_tClassID				= ai().ef_storage().m_tpCurrentMember->SUB_CLS_ID;
	else {
		CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(ai().ef_storage().m_tpCurrentALifeEnemy);
		R_ASSERT3				(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function ",m_caName);
		l_tClassID				= l_tpALifeDynamicObject->m_tClassID;
	}
	switch (l_tClassID) {
		case CLSID_Z_MBALD : {
			m_fLastValue		=  1;
			break;
		}
		case CLSID_Z_MINCER : {
			m_fLastValue		=  2;
			break;
		}
		case CLSID_Z_RADIO : {
			m_fLastValue		=  3;
			break;
		}
		case CLSID_Z_ACIDF : {
			m_fLastValue		=  4;
			break;
		}
		case CLSID_Z_GALANT : {
			m_fLastValue		=  5;
			break;
		}
		case CLSID_Z_BFUZZ : {
			m_fLastValue		=  6;
			break;
		}
		case CLSID_Z_RUSTYH : {
			m_fLastValue		=  7;
			break;
		}
		case CLSID_Z_FRYUP : {
			m_fLastValue		=  8;
			break;
		}
		default : NODEFAULT;
	}
	return						(m_fLastValue -= 1.f);
}

float CDetectorType::ffGetValue()
{
	if (bfCheckForCachedResult())
		return					(m_fLastValue);

	CLASS_ID					l_tClassID;
	if (ai().ef_storage().m_tpCurrentMember)
		if (!ai().ef_storage().m_tpGameObject)
			return				(m_fLastValue = 0);
		else
			l_tClassID			= ai().ef_storage().m_tpGameObject->SUB_CLS_ID;
	else {
		const CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<const CSE_ALifeDynamicObject*>(ai().ef_storage().m_tpCurrentALifeObject);
		if (!l_tpALifeDynamicObject)
			return				(m_fLastValue = 0);
		else
			l_tClassID			= l_tpALifeDynamicObject->m_tClassID;
	}
	
	switch (l_tClassID) {
		
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
		case CLSID_DETECTOR_SIMPLE		: {
			m_fLastValue		=  2;
			break;
		}
		case CLSID_DETECTOR_VISUAL		: {
			m_fLastValue		=  3;
			break;
		}
		default : NODEFAULT;
	}

	return						(m_fLastValue -= 1.f);
}

float CEnemyDistanceToGraphPoint::ffGetValue()
{
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(ai().ef_storage().m_tpCurrentALifeEnemy);
	R_ASSERT3				(l_tpALifeDynamicObject,"Invalid object passed to the evaluation function ",m_caName);
	if (l_tpALifeDynamicObject->m_fDistance < 5.f)
		return				(m_fLastValue = 0);
	if (l_tpALifeDynamicObject->m_fDistance < 10.f)
		return				(m_fLastValue = 1);
	if (l_tpALifeDynamicObject->m_fDistance < 15.f)
		return				(m_fLastValue = 2);
	if (l_tpALifeDynamicObject->m_fDistance < 20.f)
		return				(m_fLastValue = 3);
	return					(m_fLastValue = 4);
}
