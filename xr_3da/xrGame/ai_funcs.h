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

#include "CustomMonster.h"

class CBaseFunction {
protected:
	DWORD			m_dwLastUpdate;
	double			m_dLastValue;
	CCustomMonster	*m_tpLastMonster;
	double			m_dMinResultValue;
	double			m_dMaxResultValue;

public:

	virtual	void	vfLoadEF(const char *caFileName, CBaseFunction **fpaBaseFunctions) {};
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions) = 0;
	
	virtual DWORD	dwfGetDiscreteValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions, DWORD dwDiscretizationValue)
	{
		double dTemp = dfGetValue(tpCustomMonster,fpaBaseFunctions);
		if (dTemp <= m_dMinResultValue)
			return(0);
		else
			if (dTemp >= m_dMaxResultValue)
				return(dwDiscretizationValue - 1);
			else {
				double dDummy = (m_dMaxResultValue - m_dMinResultValue)/(double)dwDiscretizationValue;
				dDummy = (dTemp - m_dMinResultValue)/dDummy;
				return(iFloor((float)dDummy));
			}
	}
};

class CPatternFunction : public CBaseFunction {
	
private:
	typedef struct tagSPattern {
		DWORD		dwCardinality;
		DWORD		*dwaVariableIndexes;
	} SPattern;

	DWORD		*m_dwaAtomicFeatureRange;
	DWORD		*m_dwaPatternIndexes;
	SPattern	*m_tpPatterns;
	double		*m_daParameters;

	DWORD		m_dwPatternCount;
	DWORD		m_dwParameterCount;

	IC DWORD dwfGetPatternIndex(DWORD *dwpTest, int iPatternIndex)
	{
		SPattern &tPattern = m_tpPatterns[iPatternIndex];
		for (DWORD i=1, dwIndex = dwpTest[tPattern.dwaVariableIndexes[0]]; i<(int)tPattern.dwCardinality; i++)
			dwIndex = dwIndex*m_dwaAtomicFeatureRange[tPattern.dwaVariableIndexes[i]] + dwpTest[tPattern.dwaVariableIndexes[i]];
		return(dwIndex + m_dwaPatternIndexes[iPatternIndex]);
	}
	
	double		dfEvaluate();

public:
	
	DWORD		m_dwVariableCount;
	DWORD		m_dwFunctionType;
	DWORD		*m_dwaVariableTypes;
	DWORD		*m_dwaVariableValues;

				CPatternFunction(const char *caEFFileName, CBaseFunction **fpaBaseFunctions);
				CPatternFunction();
				~CPatternFunction();

	virtual	void	vfLoadEF(const char *caEFFileName, CBaseFunction **fpaBaseFunctions);
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions);
};

class CHealthFunction : public CBaseFunction {
	
public:
	CHealthFunction() {
		m_dMinResultValue = 0.0;
		m_dMaxResultValue = 100.0;
	}
	
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_dLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_dLastValue = tpCustomMonster->g_Health());
	};
	
};
 
class CArmorFunction : public CBaseFunction {
	
public:
	CArmorFunction() {
		m_dMinResultValue = 0.0;
		m_dMaxResultValue = 100.0;
	}
	
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_dLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_dLastValue = tpCustomMonster->g_Armor());
	};
	
};
 
class CMoraleFunction : public CBaseFunction {
	
public:
	CMoraleFunction() {
		m_dMinResultValue = 0.0;
		m_dMaxResultValue = 100.0;
	}
	
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_dLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_dLastValue = m_dMaxResultValue);
	};
	
};
 
class CStrengthFunction : public CBaseFunction {
	
public:
	CStrengthFunction() {
		m_dMinResultValue = 0.0;
		m_dMaxResultValue = 100.0;
	}
	
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_dLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_dLastValue = m_dMaxResultValue);
	};
	
};
 
class CAccuracyFunction : public CBaseFunction {
	
public:
	CAccuracyFunction() {
		m_dMinResultValue = 0.0;
		m_dMaxResultValue = 100.0;
	}
	
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_dLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_dLastValue = m_dMaxResultValue);
	};
	
};
 
class CReactionFunction : public CBaseFunction {
	
public:
	CReactionFunction() {
		m_dMinResultValue = 0.0;
		m_dMaxResultValue = 100.0;
	}
	
	virtual double	dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_dLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_dLastValue = m_dMaxResultValue);
	};
	
};
 
#endif