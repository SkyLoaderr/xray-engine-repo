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

class CCustomMonster;

class CPatternFunction {
	
private:
	typedef struct tagSPattern {
		DWORD		dwCardinality;
		DWORD		*dwaVariableIndexes;
	} SPattern;

	DWORD			*dwaAtomicFeatureRange;
	DWORD			*dwaPatternIndexes;
	SPattern		*tpPatterns;
	double			*daParameters;

	DWORD			dwPatternCount;
	DWORD			dwParameterCount;

	__forceinline DWORD dwfGetPatternIndex(DWORD *dwpTest, int iPatternIndex)
	{
		SPattern &tPattern = tpPatterns[iPatternIndex];
		for (DWORD i=1, dwIndex = dwpTest[tPattern.dwaVariableIndexes[0]]; i<(int)tPattern.dwCardinality; i++)
			dwIndex = dwIndex*dwaAtomicFeatureRange[tPattern.dwaVariableIndexes[i - 1]] + dwpTest[tPattern.dwaVariableIndexes[i]];
		return(dwIndex + dwaPatternIndexes[iPatternIndex]);
	}
	
public:
	
	typedef DWORD	STypeFunction();

	DWORD		dwVariableCount;
	DWORD		*dwaVariableTypes;
	DWORD		*dwaVariableValues;

				CPatternFunction(char *caEFFileName);
				CPatternFunction();
				~CPatternFunction();

	void		vfLoadEF(char *caEFFileName);
	double		dfEvaluate();
	double		dfGetValue(CCustomMonster *cAIMonster);
};

#endif