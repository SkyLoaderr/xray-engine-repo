////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_storage.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation functions storage class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

#define AI_MAX_EVALUATION_FUNCTION_COUNT	128

class CGameObject;
class CEntityAlive;
class CSE_ALifeSchedulable;
class CSE_ALifeObject;
class CBaseFunction;
class CPatternFunction;

class CDistanceFunction;
class CGraphPointType0;
class CEquipmentType;
class CItemDeterioration;
class CEquipmentPreference;
class CMainWeaponType;
class CMainWeaponPreference;
class CItemValue;
class CWeaponAmmoCount;
class CDetectorType;
class CPersonalHealthFunction;
class CPersonalMoraleFunction;
class CPersonalCreatureTypeFunction;
class CPersonalWeaponTypeFunction;
class CPersonalAccuracyFunction;
class CPersonalIntelligenceFunction;
class CPersonalRelationFunction;
class CPersonalGreedFunction;
class CPersonalAggressivenessFunction;
class CPersonalEyeRange;
class CPersonalMaxHealth;
class CEnemyHealthFunction;
class CEnemyCreatureTypeFunction;
class CEnemyWeaponTypeFunction;
class CEnemyEquipmentCostFunction;
class CEnemyRukzakWeightFunction;
class CEnemyAnomalityFunction;
class CEnemyEyeRange;
class CEnemyMaxHealth;
class CEnemyAnomalyType;
class CEnemyDistanceToGraphPoint;

class CEF_Storage {
public:
	const CGameObject						*m_tpGameObject;
	const CEntityAlive						*m_tpCurrentMember;
	const CEntityAlive						*m_tpCurrentEnemy;
	const CSE_ALifeObject					*m_tpCurrentALifeObject;
	CSE_ALifeSchedulable					*m_tpCurrentALifeMember;
	CSE_ALifeSchedulable					*m_tpCurrentALifeEnemy;
	// primary functions
	CBaseFunction							*m_fpaBaseFunctions		[AI_MAX_EVALUATION_FUNCTION_COUNT];

	CDistanceFunction						*m_pfDistance;
	CGraphPointType0						*m_pfGraphPointType0;
	CEquipmentType							*m_pfEquipmentType;
	CItemDeterioration						*m_pfItemDeterioration;
	CEquipmentPreference					*m_pfEquipmentPreference;
	CMainWeaponType							*m_pfMainWeaponType;
	CMainWeaponPreference					*m_pfMainWeaponPreference;
	CItemValue								*m_pfItemValue;
	CWeaponAmmoCount						*m_pfWeaponAmmoCount;
	CDetectorType							*m_pfDetectorType;

	CPersonalHealthFunction					*m_pfPersonalHealth;
	CPersonalMoraleFunction					*m_pfPersonalMorale;
	CPersonalCreatureTypeFunction			*m_pfPersonalCreatureType;
	CPersonalWeaponTypeFunction				*m_pfPersonalWeaponType;
	CPersonalAccuracyFunction				*m_pfPersonalAccuracy;
	CPersonalIntelligenceFunction			*m_pfPersonalIntelligence;
	CPersonalRelationFunction				*m_pfPersonalRelation;
	CPersonalGreedFunction					*m_pfPersonalGreed;
	CPersonalAggressivenessFunction			*m_pfPersonalAggressiveness;
	CPersonalEyeRange						*m_pfPersonalEyeRange;
	CPersonalMaxHealth						*m_pfPersonalMaxHealth;

	CEnemyHealthFunction					*m_pfEnemyHealth;
	CEnemyCreatureTypeFunction				*m_pfEnemyCreatureType;
	CEnemyWeaponTypeFunction				*m_pfEnemyWeaponType;
	CEnemyEquipmentCostFunction				*m_pfEnemyEquipmentCost;
	CEnemyRukzakWeightFunction				*m_pfEnemyRukzakWeight;
	CEnemyAnomalityFunction					*m_pfEnemyAnomality;
	CEnemyEyeRange							*m_pfEnemyEyeRange;
	CEnemyMaxHealth							*m_pfEnemyMaxHealth;
	CEnemyAnomalyType						*m_pfEnemyAnomalyType;
	CEnemyDistanceToGraphPoint				*m_pfEnemyDistanceToGraphPoint;

	// complex functions
	CPatternFunction						*m_pfWeaponEffectiveness;
	CPatternFunction						*m_pfCreatureEffectiveness;
	CPatternFunction						*m_pfIntellectCreatureEffectiveness;
	CPatternFunction						*m_pfAccuracyWeaponEffectiveness;
	CPatternFunction						*m_pfFinalCreatureEffectiveness;
	CPatternFunction						*m_pfVictoryProbability;
	CPatternFunction						*m_pfEntityCost;
	CPatternFunction						*m_pfExpediency;
	CPatternFunction						*m_pfSurgeDeathProbability;
	CPatternFunction						*m_pfEquipmentValue;
	CPatternFunction						*m_pfMainWeaponValue;
	CPatternFunction						*m_pfSmallWeaponValue;
	CPatternFunction						*m_pfTerrainType;
	CPatternFunction						*m_pfWeaponAttackTimes;
	CPatternFunction						*m_pfWeaponSuccessProbability;
	CPatternFunction						*m_pfEnemyDetectability;
	CPatternFunction						*m_pfEnemyDetectProbability;
	CPatternFunction						*m_pfEnemyRetreatProbability;
	CPatternFunction						*m_pfAnomalyDetectProbability;
	CPatternFunction						*m_pfAnomalyInteractProbability;
	CPatternFunction						*m_pfAnomalyRetreatProbability;
	CPatternFunction						*m_pfBirthPercentage;
	CPatternFunction						*m_pfBirthProbability;
	CPatternFunction						*m_pfBirthSpeed;

											CEF_Storage		();
	virtual									~CEF_Storage	();
			CBaseFunction					*function		(LPCSTR function) const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CEF_Storage)
#undef script_type_list
#define script_type_list save_type_list(CEF_Storage)
