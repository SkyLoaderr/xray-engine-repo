////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_funcs.h"
#include "ai_space.h"
#include "inventory.h"

#define WRITE_TO_LOG
#define AI_PATH			"ai\\"

#ifndef DEBUG
	#undef WRITE_TO_LOG
#endif

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

CPatternFunction::CPatternFunction(LPCSTR caFileName, CAI_DDD *tpAI_DDD)
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
	inherited::vfLoadEF		(caFileName,tpAI_DDD);
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
    
	m_tpAI_DDD = tpAI_DDD;
	m_tpAI_DDD->fpaBaseFunctions[m_dwFunctionType] = this;
	
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
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = m_tpAI_DDD->m_tpCurrentMember;
	for (u32 i=0; i<m_dwVariableCount; i++)
		m_dwaVariableValues[i] = m_tpAI_DDD->fpaBaseFunctions[m_dwaVariableTypes[i]]->dwfGetDiscreteValue(m_dwaAtomicFeatureRange[i]);
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
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = getAI().m_tpCurrentMember->Position().distance_to(getAI().m_tpCurrentEnemy->Position()));
};

float CPersonalHealthFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	m_fMaxResultValue = getAI().m_tpCurrentMember->g_MaxHealth();
	return(m_fLastValue = getAI().m_tpCurrentMember->g_Health());
};

float CPersonalMoraleFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = getAI().m_tpCurrentMember->m_fMorale);
};

float CPersonalCreatureTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	switch (getAI().m_tpCurrentMember->SUB_CLS_ID) {
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
	return(m_fLastValue);
};

float CPersonalWeaponTypeFunction::ffGetTheBestWeapon() 
{
	u32 dwBestWeapon = 0;
	CInventoryOwner *tpInventoryOwner = dynamic_cast<CInventoryOwner*>(getAI().m_tpCurrentMember);
	if (tpInventoryOwner) {
		vector<CInventorySlot>::iterator I = tpInventoryOwner->m_inventory.m_slots.begin(), B = I;
		vector<CInventorySlot>::iterator E = tpInventoryOwner->m_inventory.m_slots.end();
		u32 best_slot = -1;
		for ( ; I != E; I++)
			if ((*I).m_pIItem) {
				CWeapon *tpCustomWeapon = dynamic_cast<CWeapon*>((*I).m_pIItem);
				if (tpCustomWeapon && (tpCustomWeapon->GetAmmoCurrent() > tpCustomWeapon->GetAmmoMagSize()/10)) {
					u32 dwCurrentBestWeapon = 0;
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
	}	
	return(float(dwBestWeapon));
}

float CPersonalWeaponTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	switch (getAI().m_tpCurrentMember->SUB_CLS_ID) {
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

float CPersonalAccuracyFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = getAI().m_tpCurrentMember->m_fAccuracy);
};

float CPersonalIntelligenceFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = getAI().m_tpCurrentMember->m_fIntelligence);
};

float CPersonalRelationFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = 0);
}

float CPersonalGreedFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = 0);
}

float CPersonalAggressivenessFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentMember))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentMember;
	return(m_fLastValue = 0);
}

// enemy inversion functions	
float CEnemyHealthFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentEnemy;
	CEntityAlive *tpEntity = getAI().m_tpCurrentMember;
	getAI().m_tpCurrentMember = getAI().m_tpCurrentEnemy;
	m_fLastValue = getAI().pfPersonalHealth.ffGetValue();
	getAI().m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};

float CEnemyCreatureTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentEnemy;
	CEntityAlive *tpEntity = getAI().m_tpCurrentMember;
	getAI().m_tpCurrentMember = getAI().m_tpCurrentEnemy;
	m_fLastValue = getAI().pfPersonalCreatureType.ffGetValue();
	getAI().m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};

float CEnemyWeaponTypeFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate = Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentEnemy;
	CEntityAlive *tpEntity = getAI().m_tpCurrentMember;
	getAI().m_tpCurrentMember = getAI().m_tpCurrentEnemy;
	m_fLastValue = getAI().pfPersonalWeaponType.ffGetValue();
	getAI().m_tpCurrentMember = tpEntity;
	return(m_fLastValue);
};

float CEnemyEquipmentCostFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate	= Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentEnemy;
	m_fLastValue	= 0;
	return			(m_fLastValue);
};

float CEnemyRukzakWeightFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate	= Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentEnemy;
	CInventoryOwner* tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpLastMonster);
	if (tpInventoryOwner)
		m_fLastValue	= tpInventoryOwner->m_inventory.TotalWeight();
	else
		m_fLastValue	= 0;
	return			(m_fLastValue);
};

float CEnemyAnomalityFunction::ffGetValue()
{
	if ((m_dwLastUpdate == Device.dwTimeGlobal) && (m_tpLastMonster == getAI().m_tpCurrentEnemy))
		return(m_fLastValue);
	m_dwLastUpdate	= Device.dwTimeGlobal;
	m_tpLastMonster = getAI().m_tpCurrentEnemy;
	m_fLastValue	= 0;
	return			(m_fLastValue);
};