////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#ifndef __AI_FUNCS__
#define __AI_FUNCS__

////////////////////////////////////////////////////////////////////////////
// included headers
////////////////////////////////////////////////////////////////////////////

#include "Entity.h"
#include "xr_weapon_list.h"

#define AI_MAX_EVALUATION_FUNCTION_COUNT	128
#define OUT_MESSAGE(s1,s2)					Msg(s1,s2);

class CBaseFunction {
protected:
	u32				m_dwLastUpdate;
	float			m_fLastValue;
	CEntity			*m_tpLastMonster;
	float			m_fMinResultValue;
	float			m_fMaxResultValue;
	char			m_caName[260];

public:

					CBaseFunction() {m_caName[0] = 0;};
	virtual	void	vfLoadEF(const char *caFileName) {};
	virtual float	ffGetValue() = 0;
	virtual u32	dwfGetDiscreteValue(u32 dwDiscretizationValue)
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
	static const u32	EFC_VERSION	= 1;
	typedef struct tagSEFHeader {
		u32	dwBuilderVersion;
		u32	dwDataFormat;
	} SEFHeader;

	typedef struct tagSPattern {
		u32		dwCardinality;
		u32		*dwaVariableIndexes;
	} SPattern;

	u32		*m_dwaAtomicFeatureRange;
	u32		*m_dwaPatternIndexes;
	SPattern	*m_tpPatterns;
	float		*m_faParameters;

	u32		m_dwPatternCount;
	u32		m_dwParameterCount;

	SEFHeader	m_tEFHeader;

	IC u32 dwfGetPatternIndex(u32 *dwpTest, int iPatternIndex)
	{
		SPattern &tPattern = m_tpPatterns[iPatternIndex];
		for (u32 i=1, dwIndex = dwpTest[tPattern.dwaVariableIndexes[0]]; i<(int)tPattern.dwCardinality; i++)
			dwIndex = dwIndex*m_dwaAtomicFeatureRange[tPattern.dwaVariableIndexes[i]] + dwpTest[tPattern.dwaVariableIndexes[i]];
		return(dwIndex + m_dwaPatternIndexes[iPatternIndex]);
	}
	
	float		ffEvaluate();

public:
	
	u32		m_dwVariableCount;
	u32		m_dwFunctionType;
	u32		*m_dwaVariableTypes;
	u32		*m_dwaVariableValues;

				CPatternFunction(const char *caEFFileName);
				CPatternFunction();
				~CPatternFunction();

	virtual	void	vfLoadEF(const char *caEFFileName);
	virtual float	ffGetValue();
};

class CDistanceFunction : public CBaseFunction {

public:
	CDistanceFunction() {
		m_fMinResultValue = 3.0;
		m_fMaxResultValue = 20.0;
		strcat(m_caName,"Distance");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
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
	CPersonalHealthFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CPersonalMoraleFunction : public CBaseFunction {
	
public:
	CPersonalMoraleFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CPersonalCreatureTypeFunction : public CBaseFunction {
	
public:
	CPersonalCreatureTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 22.0;
		strcat(m_caName,"PersonalCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CPersonalWeaponTypeFunction : public CBaseFunction {

	float ffGetTheBestWeapon();

public:
	CPersonalWeaponTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"PersonalWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CEnemyHealthFunction : public CBaseFunction {

public:
	CEnemyHealthFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CEnemyMoraleFunction : public CBaseFunction {
	
public:
	CEnemyMoraleFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CEnemyCreatureTypeFunction : public CBaseFunction {
	
public:
	CEnemyCreatureTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 21.0;
		strcat(m_caName,"EnemyCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CEnemyWeaponTypeFunction : public CBaseFunction {
	
public:
	CEnemyWeaponTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	}
	virtual float ffGetValue();
};
 
class CAI_DDD {

public:
	CEntityAlive					*m_tpCurrentMember;
	CEntityAlive					*m_tpCurrentEnemy;
	// primary functions
	CBaseFunction					*fpaBaseFunctions		[AI_MAX_EVALUATION_FUNCTION_COUNT];

	CDistanceFunction				pfDistance;

	CPersonalHealthFunction			pfPersonalHealth;
	CPersonalMoraleFunction			pfPersonalMorale;
	CPersonalCreatureTypeFunction	pfPersonalCreatureType;
	CPersonalWeaponTypeFunction		pfPersonalWeaponType;

	CEnemyHealthFunction			pfEnemyHealth;
	CEnemyMoraleFunction			pfEnemyMorale;
	CEnemyCreatureTypeFunction		pfEnemyCreatureType;
	CEnemyWeaponTypeFunction		pfEnemyWeaponType;

	// complex functions
	CPatternFunction				pfWeaponEffectiveness;
	CPatternFunction				pfAttackSuccessProbability;
	CPatternFunction				pfDefendSuccessProbability;
	CPatternFunction				pfCreatureEffectiveness;

	void vfLoad() {	
		fpaBaseFunctions[0] = &pfDistance;
		
		fpaBaseFunctions[21] = &pfPersonalHealth;
		fpaBaseFunctions[22] = &pfPersonalMorale;
		fpaBaseFunctions[23] = &pfPersonalCreatureType;
		fpaBaseFunctions[24] = &pfPersonalWeaponType;
		
		fpaBaseFunctions[41] = &pfEnemyHealth;
		fpaBaseFunctions[42] = &pfEnemyMorale;
		fpaBaseFunctions[43] = &pfEnemyCreatureType;
		fpaBaseFunctions[44] = &pfEnemyWeaponType;

		pfWeaponEffectiveness.		vfLoadEF("common\\WeaponEffectiveness.dat");
		pfAttackSuccessProbability.	vfLoadEF("common\\AttackSuccessProbability.dat");
		pfDefendSuccessProbability.	vfLoadEF("common\\DefendSuccessProbability.dat");
		pfCreatureEffectiveness.	vfLoadEF("common\\CreatureEffectiveness.dat");
	}
};

#endif