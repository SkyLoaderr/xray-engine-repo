////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_funcs.h"

#define WRITE_TO_LOG
#define AI_PATH			"ai\\"

CPatternFunction::CPatternFunction()
{
	m_dwPatternCount = m_dwVariableCount = m_dwParameterCount = 0;
	m_dwaVariableTypes = 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes = 0;
	m_tpPatterns = 0;
	m_faParameters = 0;
	m_dwaVariableValues = 0;
}

CPatternFunction::CPatternFunction(const char *caFileName)
{
	m_dwPatternCount = m_dwVariableCount = m_dwParameterCount = 0;
	m_dwaVariableTypes = 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes = 0;
	m_tpPatterns = 0;
	m_faParameters = 0;
	m_dwaVariableValues = 0;
	vfLoadEF(caFileName);
}

CPatternFunction::~CPatternFunction()
{
	_FREE(m_dwaVariableTypes);
	_FREE(m_dwaAtomicFeatureRange);
	_FREE(m_dwaPatternIndexes);
	for (DWORD i=0; i<m_dwPatternCount; i++)
		_FREE(m_tpPatterns[i].dwaVariableIndexes);
	_FREE(m_tpPatterns);
	_FREE(m_faParameters);
	_FREE(m_dwaVariableValues);
}

void CPatternFunction::vfLoadEF(const char *caFileName)
{
	char caPath[260];
	memcpy(caPath,Path.GameData,(strlen(Path.GameData) + 1)*sizeof(char));
	strcat(caPath,AI_PATH);
	strcat(caPath,caFileName);

	FILE *fTestParameters = fopen(caPath,"rb");
	if (!fTestParameters) {
		Msg("! Evaluation function : File not found \"%s\"",caPath);
		R_ASSERT(false);
		return;
	}
	
	fread(&m_tEFHeader,1,sizeof(SEFHeader),fTestParameters);
	if (m_tEFHeader.dwBuilderVersion != EFC_VERSION) {
		fclose(fTestParameters);
		Msg("! Evaluation function (%s) : Not supported version of the Evaluation Function Contructor",caPath);
		R_ASSERT(false);
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

	fread(&m_fMinResultValue,1,sizeof(float),fTestParameters);
	fread(&m_fMaxResultValue,1,sizeof(float),fTestParameters);

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
	
	m_faParameters = (float *)xr_malloc(m_dwParameterCount*sizeof(float));
	fread(m_faParameters,m_dwParameterCount,sizeof(float),fTestParameters);
	fclose(fTestParameters);

	m_dwaVariableValues = (DWORD *)xr_malloc(m_dwVariableCount*sizeof(DWORD));
	
	_FREE(m_dwaAtomicIndexes);
    
	Level().m_tpAI_DDD->fpaBaseFunctions[m_dwFunctionType] = this;
	
	_splitpath(caFileName,0,0,m_caName,0);

	Msg("* Evaluation function \"%s\" is successfully loaded",m_caName);
}

float CPatternFunction::ffEvaluate()
{
	float fResult = 0.0;
	for (DWORD i=0; i<m_dwPatternCount; i++)
		fResult += m_faParameters[dwfGetPatternIndex(m_dwaVariableValues,i)];
	return(fResult);
}

float CPatternFunction::ffGetValue()
{
	//if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentMember))
	//	return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentMember;
	for (DWORD i=0; i<m_dwVariableCount; i++)
		m_dwaVariableValues[i] = Level().m_tpAI_DDD->fpaBaseFunctions[m_dwaVariableTypes[i]]->dwfGetDiscreteValue(m_dwaAtomicFeatureRange[i]);
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

// primary functions
float CDistanceFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentMember;
	return(m_fLastValue = Level().m_tpAI_DDD->m_tpCurrentMember->Position().distance_to(Level().m_tpAI_DDD->m_tpCurrentEnemy->Position()));
};

float CPersonalHealthFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentMember;
	m_fMaxResultValue = Level().m_tpAI_DDD->m_tpCurrentMember->g_MaxHealth();
	return(m_fLastValue = Level().m_tpAI_DDD->m_tpCurrentMember->g_Health());
};

float CPersonalMoraleFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentMember;
	return(m_fLastValue = Level().m_tpAI_DDD->m_tpCurrentMember->m_fMorale);
};

float CPersonalCreatureTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentMember;
	switch (Level().m_tpAI_DDD->m_tpCurrentMember->SUB_CLS_ID) {
		case CLSID_AI_RAT				: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_RAT_WOLF			: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_ZOMBIE			: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_ZOMBIE_HUMAN		: {
			m_fLastValue =  4;
			break;
		}
		case CLSID_AI_POLTERGEIST		: {
			m_fLastValue =  5;
			break;
		}
		case CLSID_AI_DOG				: {
			m_fLastValue =  6;
			break;
		}
		case CLSID_AI_FLESH				: {
			m_fLastValue =  7;
			break;
		}
		case CLSID_AI_DWARF				: {
			m_fLastValue =  8;
			break;
		}
		case CLSID_AI_SCIENTIST			: {
			m_fLastValue =  9;
			break;
		}
		case CLSID_AI_PHANTOM			: {
			m_fLastValue = 10;
			break;
		}
		case CLSID_AI_SPONGER			: {
			m_fLastValue = 11;
			break;
		}
		case CLSID_AI_CONTROLLER		: {
			m_fLastValue = 12;
			break;
		}
		case CLSID_AI_BLOODSUCKER		: {
			m_fLastValue = 13;
			break;
		}
		case CLSID_AI_SOLDIER			: {
			m_fLastValue = 14;
			break;
		}
		case CLSID_AI_STALKER_DARK		: {
			m_fLastValue = 15;
			break;
		}
		case CLSID_AI_STALKER_MILITARY	: {
			m_fLastValue = 16;
			break;
		}
		case CLSID_OBJECT_ACTOR			: 
		case CLSID_AI_STALKER			: {
			m_fLastValue = 17;
			break;
		}
		case CLSID_AI_BURER				: {
			m_fLastValue = 18;
			break;
		}
		case CLSID_AI_GIANT				: {
			m_fLastValue = 19;
			break;
		}
		case CLSID_AI_CHIMERA	: {
			m_fLastValue = 20;
			break;
		}
		case CLSID_AI_FRACTURE	:
		case CLSID_AI_DOG_BLACK	: {
			m_fLastValue = 21;
			break;
		}
	}
	return(m_fLastValue -= 1.f);
};

float CPersonalWeaponTypeFunction::ffGetTheBestWeapon() 
{
	DWORD dwBestWeapon = 2;
	for (int i=0; i<(int)Level().m_tpAI_DDD->m_tpCurrentMember->GetItemList()->getWeaponCount(); i++) {
		CWeapon *tpCustomWeapon = Level().m_tpAI_DDD->m_tpCurrentMember->GetItemList()->getWeaponByIndex(i);
		if (tpCustomWeapon->GetAmmoCurrent() > tpCustomWeapon->GetAmmoMagSize()/10) {
			DWORD dwCurrentBestWeapon = 0;
			switch (tpCustomWeapon->SUB_CLS_ID) {
				case CLSID_OBJECT_W_M134	: {
					dwCurrentBestWeapon = 9;
					break;
				}
				case CLSID_OBJECT_W_FN2000	: {
					dwCurrentBestWeapon = 8;
					break;
				}
				case CLSID_OBJECT_W_AK74	: {
					dwCurrentBestWeapon = 6;
					break;
				}
				case CLSID_OBJECT_W_LR300	: {
					dwCurrentBestWeapon = 6;
					break;
				}
				case CLSID_OBJECT_W_HPSA	: {
					dwCurrentBestWeapon = 5;
					break;
				}
				case CLSID_OBJECT_W_PM		: {
					dwCurrentBestWeapon = 5;
					break;
				}
				case CLSID_OBJECT_W_FORT	: {
					dwCurrentBestWeapon = 5;
					break;
				}
				default						: {
					dwCurrentBestWeapon = 0;
					break;
				}
			}
			if (dwCurrentBestWeapon > dwBestWeapon)
				dwBestWeapon = dwCurrentBestWeapon;
		}
	}
	return(float(dwBestWeapon));
}

float CPersonalWeaponTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentMember;
	switch (Level().m_tpAI_DDD->m_tpCurrentMember->SUB_CLS_ID) {
		case CLSID_AI_RAT				: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_RAT_WOLF			: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_ZOMBIE			: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_ZOMBIE_HUMAN		: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_POLTERGEIST		: {
			// 1 or 12
			m_fLastValue =  12;
			break;
		}
		case CLSID_AI_DOG				: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_FLESH				: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_DWARF				: {
			m_fLastValue =  1;
			break;
		}
		case CLSID_AI_SCIENTIST			: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_PHANTOM			: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_SPONGER			: {
			m_fLastValue =  2;
			break;
		}
		case CLSID_AI_CONTROLLER		: {
			//2 or 11
			m_fLastValue =  11;
			break;
		}
		case CLSID_AI_BLOODSUCKER		: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_SOLDIER			: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_STALKER_DARK		: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_STALKER_MILITARY	: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_OBJECT_ACTOR			: 
		case CLSID_AI_STALKER			: {
			m_fLastValue =  ffGetTheBestWeapon();
			break;
		}
		case CLSID_AI_BURER				: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_GIANT				: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_CHIMERA	: {
			m_fLastValue =  3;
			break;
		}
		case CLSID_AI_FRACTURE	: {
			m_fLastValue =  4;
			break;
		}
		case CLSID_AI_DOG_BLACK	: {
			m_fLastValue =  4;
			break;
		}
	}
	return(m_fLastValue -= 1.f);
};

// enemy inversion functions	
float CEnemyHealthFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	CEntityAlive *tpEntity = Level().m_tpAI_DDD->m_tpCurrentMember;
	Level().m_tpAI_DDD->m_tpCurrentMember = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	m_fLastValue = Level().m_tpAI_DDD->pfPersonalHealth.ffGetValue();
	Level().m_tpAI_DDD->m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};

float CEnemyMoraleFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	CEntityAlive *tpEntity = Level().m_tpAI_DDD->m_tpCurrentMember;
	Level().m_tpAI_DDD->m_tpCurrentMember = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	m_fLastValue = Level().m_tpAI_DDD->pfPersonalMorale.ffGetValue();
	Level().m_tpAI_DDD->m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};

float CEnemyCreatureTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	CEntityAlive *tpEntity = Level().m_tpAI_DDD->m_tpCurrentMember;
	Level().m_tpAI_DDD->m_tpCurrentMember = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	m_fLastValue = Level().m_tpAI_DDD->pfPersonalCreatureType.ffGetValue();
	Level().m_tpAI_DDD->m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};

float CEnemyWeaponTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == Level().m_tpAI_DDD->m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Level().timeServer();
	m_tpLastMonster = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	CEntityAlive *tpEntity = Level().m_tpAI_DDD->m_tpCurrentMember;
	Level().m_tpAI_DDD->m_tpCurrentMember = Level().m_tpAI_DDD->m_tpCurrentEnemy;
	m_fLastValue = Level().m_tpAI_DDD->pfPersonalWeaponType.ffGetValue();
	Level().m_tpAI_DDD->m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};
