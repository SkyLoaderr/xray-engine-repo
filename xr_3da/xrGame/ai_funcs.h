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
	float			m_fLastValue;
	CCustomMonster	*m_tpLastMonster;
	float			m_fMinResultValue;
	float			m_fMaxResultValue;

public:

	virtual	void	vfLoadEF(const char *caFileName, CBaseFunction **fpaBaseFunctions) {};
	virtual float	ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions) = 0;
	
	virtual DWORD	dwfGetDiscreteValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions, DWORD dwDiscretizationValue)
	{
		float fTemp = ffGetValue(tpCustomMonster,fpaBaseFunctions);
		if (fTemp <= m_fMinResultValue)
			return(0);
		else
			if (fTemp >= m_fMaxResultValue)
				return(dwDiscretizationValue - 1);
			else {
				float fDummy = (m_fMaxResultValue - m_fMinResultValue)/(float)dwDiscretizationValue;
				fDummy = (fTemp - m_fMinResultValue)/fDummy;
				return(iFloor(fDummy));
			}
	}
};

class CPatternFunction : public CBaseFunction {
	
private:
	static const DWORD	EFC_VERSION	= 1;
	typedef struct tagSEFHeader {
		DWORD	dwBuilderVersion;
		DWORD	dwDataFormat;
	} SEFHeader;

	typedef struct tagSPattern {
		DWORD		dwCardinality;
		DWORD		*dwaVariableIndexes;
	} SPattern;

	DWORD		*m_dwaAtomicFeatureRange;
	DWORD		*m_dwaPatternIndexes;
	SPattern	*m_tpPatterns;
	float		*m_faParameters;

	DWORD		m_dwPatternCount;
	DWORD		m_dwParameterCount;

	SEFHeader	m_tEFHeader;

	IC DWORD dwfGetPatternIndex(DWORD *dwpTest, int iPatternIndex)
	{
		SPattern &tPattern = m_tpPatterns[iPatternIndex];
		for (DWORD i=1, dwIndex = dwpTest[tPattern.dwaVariableIndexes[0]]; i<(int)tPattern.dwCardinality; i++)
			dwIndex = dwIndex*m_dwaAtomicFeatureRange[tPattern.dwaVariableIndexes[i]] + dwpTest[tPattern.dwaVariableIndexes[i]];
		return(dwIndex + m_dwaPatternIndexes[iPatternIndex]);
	}
	
	float		ffEvaluate();

public:
	
	DWORD		m_dwVariableCount;
	DWORD		m_dwFunctionType;
	DWORD		*m_dwaVariableTypes;
	DWORD		*m_dwaVariableValues;

				CPatternFunction(const char *caEFFileName, CBaseFunction **fpaBaseFunctions);
				CPatternFunction();
				~CPatternFunction();

	virtual	void	vfLoadEF(const char *caEFFileName, CBaseFunction **fpaBaseFunctions);
	virtual float	ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions);
};

class CHealthFunction : public CBaseFunction {
	
public:
	CHealthFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = tpCustomMonster->g_Health());
	};
	
};
 
class CArmorFunction : public CBaseFunction {
	
public:
	CArmorFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = tpCustomMonster->g_Armor());
	};
	
};
 
class CMoraleFunction : public CBaseFunction {
	
public:
	CMoraleFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = m_fMaxResultValue);
	};
	
};
 
class CStrengthFunction : public CBaseFunction {
	
public:
	CStrengthFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = m_fMaxResultValue);
	};
	
};
 
class CAccuracyFunction : public CBaseFunction {
	
public:
	CAccuracyFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = m_fMaxResultValue);
	};
	
};
 
class CReactionFunction : public CBaseFunction {
	
public:
	CReactionFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = m_fMaxResultValue);
	};
	
};
 
#endif