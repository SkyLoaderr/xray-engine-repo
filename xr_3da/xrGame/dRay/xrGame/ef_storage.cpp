////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_storage.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Evaluation functions storage class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ef_storage.h"

CEF_Storage::CEF_Storage()
{	
	m_tpGameObject						= 0;
	m_tpCurrentMember					= 0;
	m_tpCurrentEnemy					= 0;
	m_tpCurrentALifeObject				= 0;
	m_tpCurrentALifeMember				= 0;
	m_tpCurrentALifeEnemy				= 0;

	Memory.mem_fill						(m_fpaBaseFunctions,0,sizeof(CBaseFunction*)*AI_MAX_EVALUATION_FUNCTION_COUNT);
	
	m_fpaBaseFunctions[0]				= m_pfDistance				= xr_new<CDistanceFunction>				();
	m_fpaBaseFunctions[1]				= m_pfGraphPointType0		= xr_new<CGraphPointType0>				();
	m_fpaBaseFunctions[2]				= m_pfEquipmentType			= xr_new<CEquipmentType>				();
	m_fpaBaseFunctions[3]				= m_pfItemDeterioration		= xr_new<CItemDeterioration>			();
	m_fpaBaseFunctions[4]				= m_pfEquipmentPreference	= xr_new<CEquipmentPreference>			();
	m_fpaBaseFunctions[5]				= m_pfMainWeaponType		= xr_new<CMainWeaponType>				();
	m_fpaBaseFunctions[6]				= m_pfMainWeaponPreference	= xr_new<CMainWeaponPreference>			();
	m_fpaBaseFunctions[7]				= m_pfItemValue				= xr_new<CItemValue>					();
	m_fpaBaseFunctions[8]				= m_pfWeaponAmmoCount		= xr_new<CWeaponAmmoCount>				();
	m_fpaBaseFunctions[9]				= m_pfDetectorType			= xr_new<CDetectorType>					();
										  
	m_fpaBaseFunctions[21]				= m_pfPersonalHealth		= xr_new<CPersonalHealthFunction>		();
	m_fpaBaseFunctions[22]				= m_pfPersonalMorale		= xr_new<CPersonalMoraleFunction>		();			
	m_fpaBaseFunctions[23]				= m_pfPersonalCreatureType	= xr_new<CPersonalCreatureTypeFunction>	();	
	m_fpaBaseFunctions[24]				= m_pfPersonalWeaponType	= xr_new<CPersonalWeaponTypeFunction>	();		
	m_fpaBaseFunctions[25]				= m_pfPersonalAccuracy		= xr_new<CPersonalAccuracyFunction>		();		
	m_fpaBaseFunctions[26]				= m_pfPersonalIntelligence	= xr_new<CPersonalIntelligenceFunction>	();	
	m_fpaBaseFunctions[27]				= m_pfPersonalRelation		= xr_new<CPersonalRelationFunction>		();		
	m_fpaBaseFunctions[28]				= m_pfPersonalGreed			= xr_new<CPersonalGreedFunction>		();			
	m_fpaBaseFunctions[29]				= m_pfPersonalAggressiveness= xr_new<CPersonalAggressivenessFunction>();	
	m_fpaBaseFunctions[30]				= m_pfPersonalEyeRange		= xr_new<CPersonalEyeRange>				();	
	m_fpaBaseFunctions[31]				= m_pfPersonalMaxHealth		= xr_new<CPersonalMaxHealth>			();	
										  
	m_fpaBaseFunctions[41]				= m_pfEnemyHealth			= xr_new<CEnemyHealthFunction>			();			
	m_fpaBaseFunctions[42]				= m_pfEnemyCreatureType		= xr_new<CEnemyCreatureTypeFunction>	();		
	m_fpaBaseFunctions[43]				= m_pfEnemyWeaponType		= xr_new<CEnemyWeaponTypeFunction>		();		
	m_fpaBaseFunctions[44]				= m_pfEnemyEquipmentCost	= xr_new<CEnemyEquipmentCostFunction>	();		
	m_fpaBaseFunctions[45]				= m_pfEnemyRukzakWeight		= xr_new<CEnemyRukzakWeightFunction>	();		
	m_fpaBaseFunctions[46]				= m_pfEnemyAnomality		= xr_new<CEnemyAnomalityFunction>		();			
	m_fpaBaseFunctions[47]				= m_pfEnemyEyeRange			= xr_new<CEnemyEyeRange>				();	
	m_fpaBaseFunctions[48]				= m_pfEnemyMaxHealth		= xr_new<CEnemyMaxHealth>				();	
	m_fpaBaseFunctions[49]				= m_pfEnemyAnomalyType		= xr_new<CEnemyAnomalyType>				();	
	m_fpaBaseFunctions[50]				= m_pfEnemyDistanceToGraphPoint= xr_new<CEnemyDistanceToGraphPoint>	();	

	m_pfWeaponEffectiveness				= xr_new<CPatternFunction>	("common\\WeaponEffectiveness.efd",		this);
	m_pfCreatureEffectiveness			= xr_new<CPatternFunction>	("common\\CreatureEffectiveness.efd",	this);
	m_pfIntellectCreatureEffectiveness	= xr_new<CPatternFunction>	("common\\IntCreatureEffectiveness.efd",this);
	m_pfAccuracyWeaponEffectiveness		= xr_new<CPatternFunction>	("common\\AccWeaponEffectiveness.efd",	this);
	m_pfFinalCreatureEffectiveness		= xr_new<CPatternFunction>	("common\\FinCreatureEffectiveness.efd",this);
	m_pfVictoryProbability				= xr_new<CPatternFunction>	("common\\VictoryProbability.efd",		this);
	m_pfEntityCost						= xr_new<CPatternFunction>	("common\\EntityCost.efd",				this);
	m_pfExpediency						= xr_new<CPatternFunction>	("common\\Expediency.efd",				this);
	m_pfSurgeDeathProbability			= xr_new<CPatternFunction>	("common\\SurgeDeathProbability.efd",	this);
	m_pfEquipmentValue					= xr_new<CPatternFunction>	("common\\EquipmentValue.efd",			this);
	m_pfMainWeaponValue					= xr_new<CPatternFunction>	("common\\MainWeaponValue.efd",			this);
	m_pfSmallWeaponValue				= xr_new<CPatternFunction>	("common\\SmallWeaponValue.efd",		this);
	m_pfTerrainType						= xr_new<CPatternFunction>	("alife\\TerrainType.efd",				this);
	m_pfWeaponAttackTimes				= xr_new<CPatternFunction>	("alife\\WeaponAttackTimes.efd",		this);
	m_pfWeaponSuccessProbability		= xr_new<CPatternFunction>	("alife\\WeaponSuccessProbability.efd",	this);
	m_pfEnemyDetectability				= xr_new<CPatternFunction>	("alife\\EnemyDetectability.efd",		this);
	m_pfEnemyDetectProbability			= xr_new<CPatternFunction>	("alife\\EnemyDetectProbability.efd",	this);
	m_pfEnemyRetreatProbability			= xr_new<CPatternFunction>	("alife\\EnemyRetreatProbability.efd",	this);
	m_pfAnomalyDetectProbability		= xr_new<CPatternFunction>	("alife\\AnomalyDetectProbability.efd",	this);
	m_pfAnomalyInteractProbability		= xr_new<CPatternFunction>	("alife\\AnomalyInteractProbability.efd",this);
	m_pfAnomalyRetreatProbability		= xr_new<CPatternFunction>	("alife\\AnomalyRetreatProbability.efd",this);
	m_pfBirthPercentage					= xr_new<CPatternFunction>	("alife\\BirthPercentage.efd",			this);
	m_pfBirthProbability				= xr_new<CPatternFunction>	("alife\\BirthProbability.efd",			this);
	m_pfBirthSpeed						= xr_new<CPatternFunction>	("alife\\BirthSpeed.efd",				this);
}

CEF_Storage::~CEF_Storage()
{
	for (int i=0; i<AI_MAX_EVALUATION_FUNCTION_COUNT; ++i)
		xr_delete						(m_fpaBaseFunctions[i]);
}