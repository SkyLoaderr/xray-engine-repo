////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern function class
////////////////////////////////////////////////////////////////////////////

#pragma once

#define AI_MAX_EVALUATION_FUNCTION_COUNT	128

#include "ai_base_funcs.h"

class CDistanceFunction;
class CPersonalHealthFunction;
class CPersonalMoraleFunction;
class CPersonalCreatureTypeFunction;
class CPersonalWeaponTypeFunction;
class CPersonalAccuracyFunction;
class CPersonalIntelligenceFunction;
class CPersonalRelationFunction;
class CPersonalGreedFunction;
class CPersonalAggressivenessFunction;
class CEnemyHealthFunction;
class CEnemyCreatureTypeFunction;
class CEnemyWeaponTypeFunction;
class CEnemyEquipmentCostFunction;
class CEnemyRukzakWeightFunction;
class CEnemyAnomalityFunction;
class CAI_DDD;

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

class CAI_DDD {

public:
	CEntityAlive							*m_tpCurrentMember;
	CEntityAlive							*m_tpCurrentEnemy;
	// primary functions
	CBaseFunction							*fpaBaseFunctions		[AI_MAX_EVALUATION_FUNCTION_COUNT];

	CDistanceFunction						*pfDistance;

	CPersonalHealthFunction					*pfPersonalHealth;
	CPersonalMoraleFunction					*pfPersonalMorale;
	CPersonalCreatureTypeFunction			*pfPersonalCreatureType;
	CPersonalWeaponTypeFunction				*pfPersonalWeaponType;
	CPersonalAccuracyFunction				*pfPersonalAccuracy;
	CPersonalIntelligenceFunction			*pfPersonalIntelligence;
	CPersonalRelationFunction				*pfPersonalRelation;
	CPersonalGreedFunction					*pfPersonalGreed;
	CPersonalAggressivenessFunction			*pfPersonalAggressiveness;

	CEnemyHealthFunction					*pfEnemyHealth;
	CEnemyCreatureTypeFunction				*pfEnemyCreatureType;
	CEnemyWeaponTypeFunction				*pfEnemyWeaponType;
	CEnemyEquipmentCostFunction				*pfEnemyEquipmentCost;
	CEnemyRukzakWeightFunction				*pfEnemyRukzakWeight;
	CEnemyAnomalityFunction					*pfEnemyAnomality;

	// complex functions
	CPatternFunction						*pfWeaponEffectiveness;
	CPatternFunction						*pfCreatureEffectiveness;
	CPatternFunction						*pfIntellectCreatureEffectiveness;
	CPatternFunction						*pfAccuracyWeaponEffectiveness;
	CPatternFunction						*pfFinalCreatureEffectiveness;
	CPatternFunction						*pfVictoryProbability;
	CPatternFunction						*pfEntityCost;
	CPatternFunction						*pfExpediency;

											CAI_DDD();
	virtual									~CAI_DDD();
};