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

double CPatternFunction::dfEvaluate()
{
	double dResult = 0.0;
	for (DWORD i=0; i<dwPatternCount; i++)
		dResult += daParameters[dwfGetPatternIndex(dwaVariableValues,i)];
	return(dResult);
}

CPatternFunction::CPatternFunction()
{
	dwPatternCount = dwVariableCount = dwParameterCount = 0;
	dwaVariableTypes = 0;
	dwaAtomicFeatureRange = 0;
	dwaPatternIndexes = 0;
	tpPatterns = 0;
	daParameters = 0;
	dwaVariableValues = 0;
}

CPatternFunction::CPatternFunction(char *caFileName)
{
	dwPatternCount = dwVariableCount = dwParameterCount = 0;
	dwaVariableTypes = 0;
	dwaAtomicFeatureRange = 0;
	dwaPatternIndexes = 0;
	tpPatterns = 0;
	daParameters = 0;
	dwaVariableValues = 0;
	vfLoadEF(caFileName);
}

void CPatternFunction::vfLoadEF(char *caFileName)
{
	FILE *fTestParameters = fopen(caFileName,"rb");
	if (!fTestParameters) {
		Msg("EvaluationFunction : File not found \"%s\"",caFileName);
		return;
	}
	fread(&dwVariableCount,1,sizeof(dwVariableCount),fTestParameters);
	dwaAtomicFeatureRange = (DWORD *)xr_malloc(dwVariableCount*sizeof(DWORD));
	ZeroMemory(dwaAtomicFeatureRange,dwVariableCount*sizeof(DWORD));
	DWORD *dwaAtomicIndexes = (DWORD *)xr_malloc(dwVariableCount*sizeof(DWORD));
	ZeroMemory(dwaAtomicIndexes,dwVariableCount*sizeof(DWORD));

	for (DWORD i=0; i<dwVariableCount; i++) {
		fread(dwaAtomicFeatureRange + i,1,sizeof(DWORD),fTestParameters);
		if (i)
			dwaAtomicIndexes[i] = dwaAtomicIndexes[i-1] + dwaAtomicFeatureRange[i-1];
	}

	dwaVariableTypes = (DWORD *)xr_malloc(dwVariableCount*sizeof(DWORD));
	fread(dwaVariableTypes,dwVariableCount,sizeof(DWORD),fTestParameters);

	fread(&dwPatternCount,1,sizeof(dwPatternCount),fTestParameters);
	tpPatterns = (SPattern *)xr_malloc(dwPatternCount*sizeof(SPattern));
	dwaPatternIndexes = (DWORD *)xr_malloc(dwPatternCount*sizeof(DWORD));
	ZeroMemory(dwaPatternIndexes,dwPatternCount*sizeof(DWORD));
	dwParameterCount = 0;
	for ( i=0; i<dwPatternCount; i++) {
		if (i)
			dwaPatternIndexes[i] = dwParameterCount;
		fread(&(tpPatterns[i].dwCardinality),1,sizeof(tpPatterns[i].dwCardinality),fTestParameters);
		tpPatterns[i].dwaVariableIndexes = (DWORD *)xr_malloc(tpPatterns[i].dwCardinality*sizeof(DWORD));
		fread(tpPatterns[i].dwaVariableIndexes,tpPatterns[i].dwCardinality,sizeof(DWORD),fTestParameters);
		DWORD dwComplexity = 1;
		for (int j=0; j<(int)tpPatterns[i].dwCardinality; j++)
			dwComplexity *= dwaAtomicFeatureRange[tpPatterns[i].dwaVariableIndexes[j]];
		dwParameterCount += dwComplexity;
	}
	
	daParameters = (double *)xr_malloc(dwParameterCount*sizeof(double));
	fread(daParameters,dwParameterCount,sizeof(double),fTestParameters);
	fclose(fTestParameters);

	dwaVariableValues = (DWORD *)xr_malloc(dwVariableCount*sizeof(DWORD));
	
	_FREE(dwaAtomicIndexes);
}

CPatternFunction::~CPatternFunction()
{
	_FREE(dwaVariableTypes);
	_FREE(dwaAtomicFeatureRange);
	_FREE(dwaPatternIndexes);
	for (DWORD i=0; i<dwPatternCount; i++)
		_FREE(tpPatterns[i].dwaVariableIndexes);
	_FREE(tpPatterns);
	_FREE(daParameters);
	_FREE(dwaVariableValues);
}

double CPatternFunction::dfGetValue(CCustomMonster *cAIMonster)
{
	for (DWORD i=0; i<dwVariableCount; i++)
		dwaVariableValues[i] = (*(cAIMonster->fpaTypeFunctions[dwaVariableTypes[i] - 1]))();
	return(dfEvaluate());
}