////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_primary_funcs.h
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary function classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_base_funcs.h"

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