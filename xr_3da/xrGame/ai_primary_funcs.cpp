////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_primary_funcs.cpp
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary function classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_primary_funcs.h"
#include "weapon.h"
#include "ai_space.h"
#include "Entity.h"
#include "inventory.h"

float CDistanceFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = getAI().m_tpCurrentMember->Position().distance_to(getAI().m_tpCurrentEnemy->Position()));
}

float CPersonalHealthFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	m_fMaxResultValue = getAI().m_tpCurrentMember->g_MaxHealth();
	return(m_fLastValue = getAI().m_tpCurrentMember->g_Health());
}

float CPersonalMoraleFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = getAI().m_tpCurrentMember->m_fMorale);
}

float CPersonalCreatureTypeFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	switch (getAI().m_tpCurrentMember->SUB_CLS_ID) {
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
		case CLSID_AI_DOG				: {
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
		case CLSID_AI_SPONGER			: {
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
		case CLSID_AI_DOG_RED	:
		case CLSID_AI_DOG_BLACK	: {
			m_fLastValue = 21;
			break;
		}
		case CLSID_AI_TRADER: break;

		default : NODEFAULT;
	}
	return(m_fLastValue);
}

float CPersonalWeaponTypeFunction::ffGetTheBestWeapon() 
{
	u32 dwBestWeapon = 0;
	CInventoryOwner *tpInventoryOwner = dynamic_cast<CInventoryOwner*>(getAI().m_tpCurrentMember);
	if (tpInventoryOwner) {
		xr_vector<CInventorySlot>::iterator I = tpInventoryOwner->m_inventory.m_slots.begin();
		xr_vector<CInventorySlot>::iterator E = tpInventoryOwner->m_inventory.m_slots.end();
		//		u32 best_slot = (u32)(-1);
		for ( ; I != E; I++)
			if ((*I).m_pIItem) {
				CWeapon *tpCustomWeapon = dynamic_cast<CWeapon*>((*I).m_pIItem);
				if (tpCustomWeapon && (tpCustomWeapon->GetAmmoCurrent() > tpCustomWeapon->GetAmmoMagSize()/10)) {
					u32 dwCurrentBestWeapon = 0;
					switch (tpCustomWeapon->SUB_CLS_ID) {
						case CLSID_OBJECT_W_RPG7:
						case CLSID_OBJECT_W_M134: {
							dwCurrentBestWeapon = 9;
							break;
												  }
						case CLSID_OBJECT_W_FN2000:
						case CLSID_OBJECT_W_SVD:
						case CLSID_OBJECT_W_SVU:
						case CLSID_OBJECT_W_VINTOREZ: {
							dwCurrentBestWeapon = 8;
							break;
													  }
						case CLSID_OBJECT_W_SHOTGUN:
						case CLSID_OBJECT_W_AK74:
						case CLSID_OBJECT_W_VAL:
						case CLSID_OBJECT_W_LR300:		{
							dwCurrentBestWeapon = 6;
							break;
														}
						case CLSID_OBJECT_W_HPSA:		
						case CLSID_OBJECT_W_PM:			
						case CLSID_OBJECT_W_FORT:		
						case CLSID_OBJECT_W_WALTHER:	
						case CLSID_OBJECT_W_USP45:		{
							dwCurrentBestWeapon = 5;
							break;
														}
						default						: {
							dwCurrentBestWeapon = 0;
							break;
													  }
					}
					if (dwCurrentBestWeapon > dwBestWeapon)
						dwBestWeapon = dwCurrentBestWeapon;
				}
			}
	}	
	return(float(dwBestWeapon));
}

float CPersonalWeaponTypeFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	switch (getAI().m_tpCurrentMember->SUB_CLS_ID) {
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
		case CLSID_AI_DOG				: {
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
			m_fLastValue =  3;
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
		case CLSID_AI_DOG_RED	:
		case CLSID_AI_DOG_BLACK	: 
			m_fLastValue =  4;
			break;

		case CLSID_AI_TRADER: 
			break;
		default : NODEFAULT;
	}
	return(m_fLastValue -= 1.f);
}

float CPersonalAccuracyFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = getAI().m_tpCurrentMember->m_fAccuracy);
}

float CPersonalIntelligenceFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = getAI().m_tpCurrentMember->m_fIntelligence);
}

float CPersonalRelationFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = 0);
}

float CPersonalGreedFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = 0);
}

float CPersonalAggressivenessFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = 0);
}

// enemy inversion functions	
float CEnemyHealthFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	CEntityAlive *tpEntity = getAI().m_tpCurrentMember;
	getAI().m_tpCurrentMember = getAI().m_tpCurrentEnemy;
	m_fLastValue = getAI().m_pfPersonalHealth->ffGetValue();
	getAI().m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
}

float CEnemyCreatureTypeFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	CEntityAlive *tpEntity = getAI().m_tpCurrentMember;
	getAI().m_tpCurrentMember = getAI().m_tpCurrentEnemy;
	m_fLastValue = getAI().m_pfPersonalCreatureType->ffGetValue();
	getAI().m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
}

float CEnemyWeaponTypeFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	CEntityAlive *tpEntity = getAI().m_tpCurrentMember;
	getAI().m_tpCurrentMember = getAI().m_tpCurrentEnemy;
	m_fLastValue = getAI().m_pfPersonalWeaponType->ffGetValue();
	getAI().m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
}

float CEnemyEquipmentCostFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = 0);
}

float CEnemyRukzakWeightFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	CInventoryOwner* tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpLastMonster);
	if (tpInventoryOwner)
		m_fLastValue	= tpInventoryOwner->m_inventory.TotalWeight();
	else
		m_fLastValue	= 0;
	return			(m_fLastValue);
}

float CEnemyAnomalityFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = 0);
}

float CGraphPointType0::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);
	return(m_fLastValue = getAI().m_tpaGraph[getAI().m_tpCurrentALifeObject->m_tGraphID].tVertexTypes[0]);
}