////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_funcs.h"
#include "CustomMonster.h"

CPatternFunction::CPatternFunction()
{
	m_dwPatternCount = m_dwVariableCount = m_dwParameterCount = 0;
	m_dwaVariableTypes = 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes = 0;
	m_tpPatterns = 0;
	m_daParameters = 0;
	m_dwaVariableValues = 0;
}

CPatternFunction::CPatternFunction(const char *caFileName, CBaseFunction **fpaBaseFunctions)
{
	m_dwPatternCount = m_dwVariableCount = m_dwParameterCount = 0;
	m_dwaVariableTypes = 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes = 0;
	m_tpPatterns = 0;
	m_daParameters = 0;
	m_dwaVariableValues = 0;
	vfLoadEF(caFileName,fpaBaseFunctions);
}

CPatternFunction::~CPatternFunction()
{
	_FREE(m_dwaVariableTypes);
	_FREE(m_dwaAtomicFeatureRange);
	_FREE(m_dwaPatternIndexes);
	for (DWORD i=0; i<m_dwPatternCount; i++)
		_FREE(m_tpPatterns[i].dwaVariableIndexes);
	_FREE(m_tpPatterns);
	_FREE(m_daParameters);
	_FREE(m_dwaVariableValues);
}

void CPatternFunction::vfLoadEF(const char *caFileName, CBaseFunction **fpaBaseFunctions)
{
	FILE *fTestParameters = fopen(caFileName,"rb");
	if (!fTestParameters) {
		Msg("EvaluationFunction : File not found \"%s\"",caFileName);
		return;
	}
	fread(&m_dwVariableCount,1,sizeof(m_dwVariableCount),fTestParameters);
	m_dwaAtomicFeatureRange = (DWORD *)xr_malloc(m_dwVariableCount*sizeof(DWORD));
	ZeroMemory(m_dwaAtomicFeatureRange,m_dwVariableCount*sizeof(DWORD));
	DWORD *m_dwaAtomicIndexes = (DWORD *)xr_malloc(m_dwVariableCount*sizeof(DWORD));
	ZeroMemory(m_dwaAtomicIndexes,m_dwVariableCount*sizeof(DWORD));

	for (DWORD i=0; i<m_dwVariableCount; i++) {
		fread(m_dwaAtomicFeatureRange + i,1,sizeof(DWORD),fTestParameters);
		if (i)
			m_dwaAtomicIndexes[i] = m_dwaAtomicIndexes[i-1] + m_dwaAtomicFeatureRange[i-1];
	}

	m_dwaVariableTypes = (DWORD *)xr_malloc(m_dwVariableCount*sizeof(DWORD));
	fread(m_dwaVariableTypes,m_dwVariableCount,sizeof(DWORD),fTestParameters);

	fread(&m_dwFunctionType,1,sizeof(DWORD),fTestParameters);

	fread(&m_dMinResultValue,1,sizeof(double),fTestParameters);
	fread(&m_dMaxResultValue,1,sizeof(double),fTestParameters);

	fread(&m_dwPatternCount,1,sizeof(m_dwPatternCount),fTestParameters);
	m_tpPatterns = (SPattern *)xr_malloc(m_dwPatternCount*sizeof(SPattern));
	m_dwaPatternIndexes = (DWORD *)xr_malloc(m_dwPatternCount*sizeof(DWORD));
	ZeroMemory(m_dwaPatternIndexes,m_dwPatternCount*sizeof(DWORD));
	m_dwParameterCount = 0;
	for ( i=0; i<m_dwPatternCount; i++) {
		if (i)
			m_dwaPatternIndexes[i] = m_dwParameterCount;
		fread(&(m_tpPatterns[i].dwCardinality),1,sizeof(m_tpPatterns[i].dwCardinality),fTestParameters);
		m_tpPatterns[i].dwaVariableIndexes = (DWORD *)xr_malloc(m_tpPatterns[i].dwCardinality*sizeof(DWORD));
		fread(m_tpPatterns[i].dwaVariableIndexes,m_tpPatterns[i].dwCardinality,sizeof(DWORD),fTestParameters);
		DWORD m_dwComplexity = 1;
		for (int j=0; j<(int)m_tpPatterns[i].dwCardinality; j++)
			m_dwComplexity *= m_dwaAtomicFeatureRange[m_tpPatterns[i].dwaVariableIndexes[j]];
		m_dwParameterCount += m_dwComplexity;
	}
	
	m_daParameters = (double *)xr_malloc(m_dwParameterCount*sizeof(double));
	fread(m_daParameters,m_dwParameterCount,sizeof(double),fTestParameters);
	fclose(fTestParameters);

	m_dwaVariableValues = (DWORD *)xr_malloc(m_dwVariableCount*sizeof(DWORD));
	
	_FREE(m_dwaAtomicIndexes);
    
	fpaBaseFunctions[m_dwFunctionType - 1] = this;
}

double CPatternFunction::dfEvaluate()
{
	double dResult = 0.0;
	for (DWORD i=0; i<m_dwPatternCount; i++)
		dResult += m_daParameters[dwfGetPatternIndex(m_dwaVariableValues,i)];
	return(dResult);
}

double CPatternFunction::dfGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
		return(m_dLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = tpCustomMonster;
	for (DWORD i=0; i<m_dwVariableCount; i++)
		m_dwaVariableValues[i] = fpaBaseFunctions[m_dwaVariableTypes[i] - 1]->dwfGetDiscreteValue(tpCustomMonster,fpaBaseFunctions,m_dwaAtomicFeatureRange[i]);
	return(m_dLastValue = dfEvaluate());
}
