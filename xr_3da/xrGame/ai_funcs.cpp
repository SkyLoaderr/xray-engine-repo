////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_funcs.h"
#include "ai_primary_funcs.h"
#include "ai_space.h"

//#define WRITE_TO_LOG
#define AI_PATH			"ai\\"

#ifndef DEBUG
	#undef WRITE_TO_LOG
#endif

CPatternFunction::CPatternFunction() : CBaseFunction()
{
	m_dwPatternCount = m_dwVariableCount = m_dwParameterCount = 0;
	m_dwaVariableTypes = 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes = 0;
	m_tpPatterns = 0;
	m_faParameters = 0;
	m_dwaVariableValues = 0;
}

CPatternFunction::CPatternFunction(LPCSTR caFileName, CAI_DDD *tpAI_DDD) : CBaseFunction()
{
	m_dwPatternCount = m_dwVariableCount = m_dwParameterCount = 0;
	m_dwaVariableTypes = 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes = 0;
	m_tpPatterns = 0;
	m_faParameters = 0;
	m_dwaVariableValues = 0;
	vfLoadEF(caFileName, tpAI_DDD);
}

CPatternFunction::~CPatternFunction()
{
	xr_free(m_dwaVariableTypes);
	xr_free(m_dwaAtomicFeatureRange);
	xr_free(m_dwaPatternIndexes);
	for (u32 i=0; i<m_dwPatternCount; i++)
		xr_free(m_tpPatterns[i].dwaVariableIndexes);
	xr_free(m_tpPatterns);
	xr_free(m_faParameters);
	xr_free(m_dwaVariableValues);
}

void CPatternFunction::vfLoadEF(LPCSTR caFileName, CAI_DDD *tpAI_DDD)
{
	char			caPath	[260];
	strconcat		(caPath,AI_PATH,caFileName);
	if (!FS.exist	(caPath, "$game_data$", caPath)) {
		Msg("! Evaluation function : File not found \"%s\"",caPath);
		R_ASSERT(false);
		return;
	}
	
	IReader *F = FS.r_open(caPath);
	F->r(&m_tEFHeader,sizeof(SEFHeader));

	if (m_tEFHeader.dwBuilderVersion != EFC_VERSION) {
		FS.r_close(F);
		Msg("! Evaluation function (%s) : Not supported version of the Evaluation Function Contructor",caPath);
		R_ASSERT(false);
		return;
	}

	F->r(&m_dwVariableCount,sizeof(m_dwVariableCount));
	m_dwaAtomicFeatureRange = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	ZeroMemory(m_dwaAtomicFeatureRange,m_dwVariableCount*sizeof(u32));
	u32 *m_dwaAtomicIndexes = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	ZeroMemory(m_dwaAtomicIndexes,m_dwVariableCount*sizeof(u32));

	for (u32 i=0; i<m_dwVariableCount; i++) {
		F->r(m_dwaAtomicFeatureRange + i,sizeof(u32));
		if (i)
			m_dwaAtomicIndexes[i] = m_dwaAtomicIndexes[i-1] + m_dwaAtomicFeatureRange[i-1];
	}

	m_dwaVariableTypes = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	F->r(m_dwaVariableTypes,m_dwVariableCount*sizeof(u32));

	F->r(&m_dwFunctionType,sizeof(u32));

	F->r(&m_fMinResultValue,sizeof(float));
	F->r(&m_fMaxResultValue,sizeof(float));

	F->r(&m_dwPatternCount,sizeof(m_dwPatternCount));
	m_tpPatterns = (SPattern *)xr_malloc(m_dwPatternCount*sizeof(SPattern));
	m_dwaPatternIndexes = (u32 *)xr_malloc(m_dwPatternCount*sizeof(u32));
	ZeroMemory(m_dwaPatternIndexes,m_dwPatternCount*sizeof(u32));
	m_dwParameterCount = 0;
	for ( i=0; i<m_dwPatternCount; i++) {
		if (i)
			m_dwaPatternIndexes[i] = m_dwParameterCount;
		F->r(&(m_tpPatterns[i].dwCardinality),sizeof(m_tpPatterns[i].dwCardinality));
		m_tpPatterns[i].dwaVariableIndexes = (u32 *)xr_malloc(m_tpPatterns[i].dwCardinality*sizeof(u32));
		F->r(m_tpPatterns[i].dwaVariableIndexes,m_tpPatterns[i].dwCardinality*sizeof(u32));
		u32 m_dwComplexity = 1;
		for (int j=0; j<(int)m_tpPatterns[i].dwCardinality; j++)
			m_dwComplexity *= m_dwaAtomicFeatureRange[m_tpPatterns[i].dwaVariableIndexes[j]];
		m_dwParameterCount += m_dwComplexity;
	}
	
	m_faParameters = (float *)xr_malloc(m_dwParameterCount*sizeof(float));
	F->r(m_faParameters,m_dwParameterCount*sizeof(float));
	FS.r_close(F);

	m_dwaVariableValues = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	
	xr_free(m_dwaAtomicIndexes);
    
	tpAI_DDD->m_fpaBaseFunctions[m_dwFunctionType] = this;
	
	_splitpath(caPath,0,0,m_caName,0);

	Msg("* Evaluation function \"%s\" is successfully loaded",m_caName);
}

float CPatternFunction::ffEvaluate()
{
	float fResult = 0.0;
	for (u32 i=0; i<m_dwPatternCount; i++)
		fResult += m_faParameters[dwfGetPatternIndex(m_dwaVariableValues,i)];
	return(fResult);
}

float CPatternFunction::ffGetValue()
{
	if (inherited::bfCheckForCachedResult())
		return(m_fLastValue);

	for (u32 i=0; i<m_dwVariableCount; i++)
		m_dwaVariableValues[i] = getAI().m_fpaBaseFunctions[m_dwaVariableTypes[i]]->dwfGetDiscreteValue(m_dwaAtomicFeatureRange[i]);
#ifndef WRITE_TO_LOG
	return(m_fLastValue = ffEvaluate());
#else
	m_fLastValue = ffEvaluate();
	char caString[256];
	int j = sprintf(caString,"%32s (",m_caName);
	for ( i=0; i<m_dwVariableCount; i++)
		j += sprintf(caString + j," %3d",m_dwaVariableValues[i] + 1);
	sprintf(caString + j,") = %7.2f",m_fLastValue);
	Msg("- %s",caString);
	return(m_fLastValue);
#endif
}

CAI_DDD::CAI_DDD()
{	
	Memory.mem_fill						(m_fpaBaseFunctions,0,sizeof(CBaseFunction*)*AI_MAX_EVALUATION_FUNCTION_COUNT);
	
	m_fpaBaseFunctions[0]				= m_pfDistance				= xr_new<CDistanceFunction>				();
	m_fpaBaseFunctions[1]				= m_pfGraphPointType0		= xr_new<CGraphPointType0>				();
										  
	m_fpaBaseFunctions[21]				= m_pfPersonalHealth		= xr_new<CPersonalHealthFunction>		();
	m_fpaBaseFunctions[22]				= m_pfPersonalMorale		= xr_new<CPersonalMoraleFunction>		();			
	m_fpaBaseFunctions[23]				= m_pfPersonalCreatureType	= xr_new<CPersonalCreatureTypeFunction>	();	
	m_fpaBaseFunctions[24]				= m_pfPersonalWeaponType	= xr_new<CPersonalWeaponTypeFunction>	();		
	m_fpaBaseFunctions[25]				= m_pfPersonalAccuracy		= xr_new<CPersonalAccuracyFunction>		();		
	m_fpaBaseFunctions[26]				= m_pfPersonalIntelligence	= xr_new<CPersonalIntelligenceFunction>	();	
	m_fpaBaseFunctions[27]				= m_pfPersonalRelation		= xr_new<CPersonalRelationFunction>		();		
	m_fpaBaseFunctions[28]				= m_pfPersonalGreed			= xr_new<CPersonalGreedFunction>		();			
	m_fpaBaseFunctions[29]				= m_pfPersonalAggressiveness= xr_new<CPersonalAggressivenessFunction>();	
										  
	m_fpaBaseFunctions[41]				= m_pfEnemyHealth			= xr_new<CEnemyHealthFunction>			();			
	m_fpaBaseFunctions[42]				= m_pfEnemyCreatureType		= xr_new<CEnemyCreatureTypeFunction>	();		
	m_fpaBaseFunctions[43]				= m_pfEnemyWeaponType		= xr_new<CEnemyWeaponTypeFunction>		();		
	m_fpaBaseFunctions[44]				= m_pfEnemyEquipmentCost	= xr_new<CEnemyEquipmentCostFunction>	();		
	m_fpaBaseFunctions[45]				= m_pfEnemyRukzakWeight		= xr_new<CEnemyRukzakWeightFunction>	();		
	m_fpaBaseFunctions[46]				= m_pfEnemyAnomality		= xr_new<CEnemyAnomalityFunction>		();			

	m_pfWeaponEffectiveness				= xr_new<CPatternFunction>	("common\\WeaponEffectiveness.dat",		this);
	m_pfCreatureEffectiveness			= xr_new<CPatternFunction>	("common\\CreatureEffectiveness.dat",	this);
	m_pfIntellectCreatureEffectiveness	= xr_new<CPatternFunction>	("common\\IntCreatureEffectiveness.dat",this);
	m_pfAccuracyWeaponEffectiveness		= xr_new<CPatternFunction>	("common\\AccWeaponEffectiveness.dat",	this);
	m_pfFinalCreatureEffectiveness		= xr_new<CPatternFunction>	("common\\FinCreatureEffectiveness.dat",this);
	m_pfVictoryProbability				= xr_new<CPatternFunction>	("common\\VictoryProbability.dat",		this);
	m_pfEntityCost						= xr_new<CPatternFunction>	("common\\EntityCost.dat",				this);
	m_pfExpediency						= xr_new<CPatternFunction>	("common\\Expediency.dat",				this);
	m_pfSurgeDeathProbability			= xr_new<CPatternFunction>	("common\\SurgeDeathProbability.dat",	this);
}

CAI_DDD::~CAI_DDD()
{
	for (int i=0; i<AI_MAX_EVALUATION_FUNCTION_COUNT; i++)
		xr_delete						(m_fpaBaseFunctions[i]);
}