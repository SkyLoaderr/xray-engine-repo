////////////////////////////////////////////////////////////////////////////
//	Module 		: fitter.cpp
//	Created 	: 25.03.2002
//  Modified 	: 28.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Weight Fitting Algorithm
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fitter.h"
#include <functional>

IC REAL dfEvaluation(REAL &A, REAL &C, REAL &D)
{
	return					(A*C + D);
}

REAL dfComputeEvalResults(vector<vector<REAL> >	&daEvalResults, vector<vector<REAL> > &A, vector<vector<REAL> > &B, vector<REAL> &C, vector<REAL> &D)
{
	REAL dResult			= 0.0;
	u32 dwTestCount			= B.size();
	u32 dwParameterCount	= B[0].size();
	
	for (u32 i=0; i<dwTestCount; i++) {
		for (u32 j=0; j<dwParameterCount; j++) {
			
			daEvalResults[i][j] = dfEvaluation(A[i][j],C[j],D[j]);
			REAL dTemp			= B[i][j] - daEvalResults[i][j];
			dResult				+= dTemp*dTemp;
		}
	}
	return					(dResult);
}

vector<REAL> &dafGradient(vector<vector<REAL> >	&daEvalResults, vector<REAL> &daResult, vector<vector<REAL> > &B, REAL dNormaFactor)
{
	REAL					dNorma = 0.0;
	u32						dwTestCount = B.size();
	u32 dwParameterCount	= B[0].size();
	daResult.assign			(dwParameterCount,0);
	
	for (u32 i=0; i<dwParameterCount; i++) {
		for (u32 j=0; j<dwTestCount; j++)
			daResult[i]		-= B[j][i] - daEvalResults[j][i];
		dNorma				+= daResult[i]*daResult[i];
	}
	dNorma					= _sqrt(dNorma);

	for ( i=0; i<dwParameterCount; i++)
		daResult[i]			/= (dNorma > 1.f ? dNorma : 1.f)*dNormaFactor;

	return					(daResult);
}

void vfOptimizeParameters(vector<vector<REAL> > &A, vector<vector<REAL> > &B, vector<REAL> &C, vector<REAL> &D, REAL dEpsilon, REAL dAlpha, REAL dBeta, REAL dNormaFactor, u32 dwMaxIterationCount)
{
	u32						dwTestCount	= B.size();
	vector<REAL>			daGradient;
	vector<REAL>			daDelta;
	vector<vector<REAL> >	daEvalResults(dwTestCount);
	
	if (!B.size()) {
		clMsg				("ERROR : there are no parameters to fit!");
		return;
	}
	
	u32						dwParameterCount = B[0].size();
	C.assign				(dwParameterCount,0.0f);
	D.assign				(dwParameterCount,0.0f);
	daDelta.assign			(dwParameterCount,0);
	daGradient.assign		(dwParameterCount,0);
	{
		for (u32 i=0; i<dwTestCount; i++)
			daEvalResults[i].resize(dwParameterCount);
	}
	u32						i = 0;
	REAL					dFunctional = dfComputeEvalResults(daEvalResults,A,B,C,D), dPreviousFunctional;
	clMsg					("%6d : %17.8f (%17.8f)",i,dFunctional,dFunctional/dwTestCount);
	do {
		dPreviousFunctional = dFunctional;
		dafGradient			(daEvalResults,			daGradient,			B,					dNormaFactor);
		transform			(daGradient.begin(),	daGradient.end(),	daGradient.begin(),	bind2nd(multiplies<REAL>(), -dAlpha));
		transform			(daDelta.begin(),		daDelta.end(),		daDelta.begin(),	bind2nd(multiplies<REAL>(), dBeta));
		transform			(daGradient.begin(),	daGradient.end(),	daDelta.begin(),	daDelta.begin(),	plus<REAL>());
		transform			(C.begin(),				C.end(),			daDelta.begin(),	C.begin(),			plus<REAL>());
		transform			(D.begin(),				D.end(),			daDelta.begin(),	D.begin(),			plus<REAL>());
		dFunctional			= dfComputeEvalResults(daEvalResults,A,B,C,D);
		i++;
	}
	while ((((dPreviousFunctional - dFunctional)/dwTestCount) > dEpsilon) && (i <= dwMaxIterationCount));
	
	if (dPreviousFunctional < dFunctional) {
		transform			(daDelta.begin(),		daDelta.end(),		daDelta.begin(),	bind2nd(multiplies<REAL>(), -1));
		transform			(C.begin(),				C.end(),			daDelta.begin(),	C.begin(),			plus<REAL>());
		transform			(D.begin(),				D.end(),			daDelta.begin(),	D.begin(),			plus<REAL>());
	}
	
	dFunctional				= dfComputeEvalResults(daEvalResults,A,B,C,D);
	clMsg					("%6d : %17.8f (%17.8f)",i,dFunctional,dFunctional/dwTestCount);
}