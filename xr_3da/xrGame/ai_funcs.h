////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Entity.h"
#include "xr_weapon_list.h"

#define AI_MAX_EVALUATION_FUNCTION_COUNT	128
#define OUT_MESSAGE(s1,s2)					//Msg(s1,s2);

class CAI_DDD;

class CBaseFunction {
protected:
	u32				m_dwLastUpdate;
	float			m_fLastValue;
	CEntity			*m_tpLastMonster;
	float			m_fMinResultValue;
	float			m_fMaxResultValue;
	char			m_caName[260];
	CAI_DDD			*m_tpAI_DDD;

public:

					CBaseFunction()
	{
		m_caName[0] = 0;
	};

	virtual	void	vfLoadEF(LPCSTR caFileName, CAI_DDD *tpAI_DDD)
	{
		m_tpAI_DDD = tpAI_DDD;
	};

	virtual float	ffGetValue() = 0;
	virtual u32		dwfGetDiscreteValue(u32 dwDiscretizationValue)
	{
		float fTemp = ffGetValue();
		if (fTemp <= m_fMinResultValue)
			return(0);
		else
			if (fTemp >= m_fMaxResultValue)
				return(dwDiscretizationValue - 1);
			else {
				float fDummy = (m_fMaxResultValue - m_fMinResultValue)/float(dwDiscretizationValue);
				fDummy = (fTemp - m_fMinResultValue)/fDummy;
				return(iFloor(fDummy));
			}
	}
};

class CPatternFunction : public CBaseFunction {
	
private:
	typedef CBaseFunction inherited;
	enum {EFC_VERSION	= 1};
	typedef struct tagSEFHeader {
		u32			dwBuilderVersion;
		u32			dwDataFormat;
	} SEFHeader;

	typedef struct tagSPattern {
		u32			dwCardinality;
		u32			*dwaVariableIndexes;
	} SPattern;

	u32				*m_dwaAtomicFeatureRange;
	u32				*m_dwaPatternIndexes;
	SPattern		*m_tpPatterns;
	float			*m_faParameters;
	u32				m_dwPatternCount;
	u32				m_dwParameterCount;
	SEFHeader		m_tEFHeader;

	IC u32 dwfGetPatternIndex(u32 *dwpTest, int iPatternIndex)
	{
		SPattern &tPattern = m_tpPatterns[iPatternIndex];
		for (u32 i=1, dwIndex = dwpTest[tPattern.dwaVariableIndexes[0]]; i<(int)tPattern.dwCardinality; i++)
			dwIndex = dwIndex*m_dwaAtomicFeatureRange[tPattern.dwaVariableIndexes[i]] + dwpTest[tPattern.dwaVariableIndexes[i]];
		return(dwIndex + m_dwaPatternIndexes[iPatternIndex]);
	}
	
	float			ffEvaluate();

public:
	
	u32				m_dwVariableCount;
	u32				m_dwFunctionType;
	u32				*m_dwaVariableTypes;
	u32				*m_dwaVariableValues;

					CPatternFunction();
					CPatternFunction(LPCSTR caEFFileName, CAI_DDD *tpAI_DDD);
	virtual			~CPatternFunction();

	virtual	void	vfLoadEF(LPCSTR caEFFileName, CAI_DDD *tpAI_DDD);
	virtual float	ffGetValue();
};

class CDistanceFunction : public CBaseFunction {

public:
	
	CDistanceFunction()
	{
		m_fMinResultValue = 3.0;
		m_fMaxResultValue = 20.0;
		strcat(m_caName,"Distance");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
	
	virtual u32	dwfGetDiscreteValue(u32 dwDiscretizationValue)
	{
		float fTemp = ffGetValue();
		if (fTemp <= m_fMinResultValue)
			return(0);
		else
			if (fTemp >= m_fMaxResultValue)
				return(dwDiscretizationValue - 1);
			else
				return(1);
	}
};
 
class CPersonalHealthFunction : public CBaseFunction {

public:
	
	CPersonalHealthFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CPersonalMoraleFunction : public CBaseFunction {
	
public:
	
	CPersonalMoraleFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};
	
	virtual float ffGetValue();
};
 
class CPersonalCreatureTypeFunction : public CBaseFunction {
	
public:
	
	CPersonalCreatureTypeFunction()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 22.0;
		strcat(m_caName,"PersonalCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CPersonalWeaponTypeFunction : public CBaseFunction {

	float ffGetTheBestWeapon();

public:
	
	CPersonalWeaponTypeFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"PersonalWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};

class CPersonalAccuracyFunction : public CBaseFunction {
public:
	
	CPersonalAccuracyFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalAccuracy");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CPersonalIntelligenceFunction : public CBaseFunction {
public:
	
	CPersonalIntelligenceFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalIntelligence");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CPersonalRelationFunction : public CBaseFunction {
public:
	
	CPersonalRelationFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalRelation");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CPersonalGreedFunction : public CBaseFunction {
public:
	
	CPersonalGreedFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalGreed");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CPersonalAggressivenessFunction : public CBaseFunction {
public:
	
	CPersonalAggressivenessFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalAggressiveness");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CEnemyHealthFunction : public CBaseFunction {

public:
	
	CEnemyHealthFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CEnemyMoraleFunction : public CBaseFunction {
	
public:
	
	CEnemyMoraleFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};
	virtual float ffGetValue();
};
 
class CEnemyCreatureTypeFunction : public CBaseFunction {
	
public:
	
	CEnemyCreatureTypeFunction()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 22.0;
		strcat(m_caName,"EnemyCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};
	virtual float ffGetValue();
};
 
class CEnemyWeaponTypeFunction : public CBaseFunction {
	
public:
	
	CEnemyWeaponTypeFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CEnemyEquipmentCostFunction : public CBaseFunction {
	
public:
	
	CEnemyEquipmentCostFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyEquipmentCost");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CEnemyRukzakWeightFunction : public CBaseFunction {
	
public:
	
	CEnemyRukzakWeightFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyRukzakWeight");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CEnemyAnomalityFunction : public CBaseFunction {
	
public:
	
	CEnemyAnomalityFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyAnomality");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};
 
class CAI_DDD {

public:
	CEntityAlive							*m_tpCurrentMember;
	CEntityAlive							*m_tpCurrentEnemy;
	// primary functions
	CBaseFunction							*fpaBaseFunctions		[AI_MAX_EVALUATION_FUNCTION_COUNT];

	CDistanceFunction						pfDistance;

	CPersonalHealthFunction					pfPersonalHealth;
	CPersonalMoraleFunction					pfPersonalMorale;
	CPersonalCreatureTypeFunction			pfPersonalCreatureType;
	CPersonalWeaponTypeFunction				pfPersonalWeaponType;
	CPersonalAccuracyFunction				pfPersonalAccuracy;
	CPersonalIntelligenceFunction			pfPersonalIntelligence;
	CPersonalRelationFunction				pfPersonalRelation;
	CPersonalGreedFunction					pfPersonalGreed;
	CPersonalAggressivenessFunction			pfPersonalAggressiveness;

	CEnemyHealthFunction					pfEnemyHealth;
	CEnemyCreatureTypeFunction				pfEnemyCreatureType;
	CEnemyWeaponTypeFunction				pfEnemyWeaponType;
	CEnemyEquipmentCostFunction				pfEnemyEquipmentCost;
	CEnemyRukzakWeightFunction				pfEnemyRukzakWeight;
	CEnemyAnomalityFunction					pfEnemyAnomality;

	// complex functions
	CPatternFunction						pfWeaponEffectiveness;
	CPatternFunction						pfCreatureEffectiveness;
	CPatternFunction						pfIntellectCreatureEffectiveness;
	CPatternFunction						pfAccuracyWeaponEffectiveness;
	CPatternFunction						pfFinalCreatureEffectiveness;
	CPatternFunction						pfVictoryProbability;
	CPatternFunction						pfEntityCost;
	CPatternFunction						pfExpediency;

											CAI_DDD()
	{	
		Memory.mem_fill						(fpaBaseFunctions,0,sizeof(CBaseFunction*)*AI_MAX_EVALUATION_FUNCTION_COUNT);
		fpaBaseFunctions[0]					= &pfDistance;
		
		fpaBaseFunctions[21]				= &pfPersonalHealth;
		fpaBaseFunctions[22]				= &pfPersonalMorale;
		fpaBaseFunctions[23]				= &pfPersonalCreatureType;
		fpaBaseFunctions[24]				= &pfPersonalWeaponType;
		fpaBaseFunctions[25]				= &pfPersonalAccuracy;
		fpaBaseFunctions[26]				= &pfPersonalIntelligence;
		fpaBaseFunctions[27]				= &pfPersonalRelation;
		fpaBaseFunctions[28]				= &pfPersonalGreed;
		fpaBaseFunctions[29]				= &pfPersonalAggressiveness;
		
		fpaBaseFunctions[41]				= &pfEnemyHealth;
		fpaBaseFunctions[42]				= &pfEnemyCreatureType;
		fpaBaseFunctions[43]				= &pfEnemyWeaponType;
		fpaBaseFunctions[44]				= &pfEnemyEquipmentCost;
		fpaBaseFunctions[45]				= &pfEnemyRukzakWeight;
		fpaBaseFunctions[46]				= &pfEnemyAnomality;

		pfWeaponEffectiveness.vfLoadEF				("common\\WeaponEffectiveness.dat",this);
		pfCreatureEffectiveness.vfLoadEF			("common\\CreatureEffectiveness.dat",this);
		pfIntellectCreatureEffectiveness.vfLoadEF	("common\\IntCreatureEffectiveness.dat",this);
		pfAccuracyWeaponEffectiveness.vfLoadEF		("common\\AccWeaponEffectiveness.dat",this);
		pfFinalCreatureEffectiveness.vfLoadEF		("common\\FinCreatureEffectiveness.dat",this);
		pfVictoryProbability.vfLoadEF				("common\\VictoryProbability.dat",this);
		pfEntityCost.vfLoadEF						("common\\EntityCost.dat",this);
		pfExpediency.vfLoadEF						("common\\Expediency.dat",this);
	}

	virtual									~CAI_DDD()
	{
	};
};