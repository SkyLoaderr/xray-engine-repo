////////////////////////////////////////////////////////////////////////////
//	Module 		: fitter.cpp
//	Created 	: 25.03.2002
//  Modified 	: 09.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern Configuration Generation and Weight Fitting Algorithms
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fitter.h"

#define EPSILON					0.001f
#define ALPHA					1.0f
#define BETA					0.01f
#define MAX_ITERATION			10000

REAL	dEpsilon				= EPSILON;
REAL	dAlphaCoefficient		= ALPHA;
REAL	dBetaCoefficient		= BETA;
u32		uiMaxIterationCount		= MAX_ITERATION;

REAL	*daDelta;
REAL	*daGradient;
REAL	*daEvalResults;

u32		uiParameterCount;
u32		uiTestCount;

__forceinline REAL dfEvaluation(vector<REAL> &A, vector<REAL> &B, vector<REAL> &C, vector<REAL> &D)
{
	REAL dResult = 0.0;
	u32 N = A.size();
	for (u32 i=0; i<N; i++) {
		REAL dTemp = A[i]*C[i] + D[i] - B[i];
		dResult += dTemp*dTemp;
	}
	return(_sqrt(dResult));
}

REAL *dafMultiplicateVectorByConstant(REAL *daParameters, REAL dConstant, REAL *daResult)
{
	for (u32 i=0; i<uiParameterCount; i++)
		daResult[i] = daParameters[i] * dConstant;
	return(daResult);
}

REAL *dafAddVectors(REAL *daVector0, REAL *daVector1, REAL *daResult)
{
	for (u32 i=0; i<uiParameterCount; i++)
		daResult[i] = daVector0[i] + daVector1[i];
	return(daResult);
}

vector<REAL> &dafAddVectors(vector<REAL> &daVector0, REAL *daVector1, vector<REAL> &daResult)
{
	for (u32 i=0; i<uiParameterCount; i++)
		daResult[i] = daVector0[i] + daVector1[i];
	return(daResult);
}

REAL dfComputeEvalResults(vector<vector<REAL> > &A, vector<vector<REAL> > &B, vector<REAL> &C, vector<REAL> &D)
{
	REAL dResult = 0.0;
	for (u32 i=0; i<uiTestCount; i++)
		dResult += (daEvalResults[i] = dfEvaluation(A[i],B[i],C,D));
	return(dResult);
}

REAL *dafGradient(REAL *daResult)
{
	Memory.mem_fill(daResult,0,uiParameterCount*sizeof(REAL));
	
	REAL dNorma = 0.0;
	for (u32 i=0; i<uiParameterCount; i++) {
		for (u32 j=0; j<uiTestCount; j++)
			daResult[i] -= daEvalResults[j];
		dNorma += _abs(daResult[i]);
	}

	for ( i=0; i<uiParameterCount; i++)
		daResult[i] /= (dNorma > 1.f ? dNorma : 1.f)*100;

	return(daResult);
}

void vfOptimizeParameters(vector<vector<REAL> > &A, vector<vector<REAL> > &B, vector<REAL> &C, vector<REAL> &D)
{
	uiTestCount = A.size();
	uiParameterCount = A[0].size();
	daDelta			= (REAL *)calloc(uiParameterCount,sizeof(REAL));
	daGradient		= (REAL *)calloc(uiParameterCount,sizeof(REAL));
	daEvalResults	= (REAL *)malloc(uiTestCount*sizeof(REAL));
	C.assign(uiParameterCount,0.0f);
	D.assign(uiParameterCount,0.0f);
	
	REAL dFunctional = dfComputeEvalResults(A,B,C,D), dPreviousFunctional;
	u32 i = 0;
	do {
		
		dPreviousFunctional = dFunctional;
		
		dafGradient(daGradient);
		dafMultiplicateVectorByConstant(daGradient,-dAlphaCoefficient,daGradient);
		dafMultiplicateVectorByConstant(daDelta,dBetaCoefficient,daDelta);
		dafAddVectors(daGradient,daDelta,daDelta);
		dafAddVectors(C,daDelta,C);
		dafAddVectors(D,daDelta,D);

		dFunctional = dfComputeEvalResults(A,B,C,D);
		i++;
	}
	while ((((dPreviousFunctional - dFunctional)/uiTestCount) > dEpsilon) && (i <= uiMaxIterationCount));

	free(daDelta);
	free(daGradient);
	free(daEvalResults);
}
