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
#include "xr_weapon_list.h"

class CBaseFunction {
protected:
	DWORD			m_dwLastUpdate;
	float			m_fLastValue;
	CCustomMonster	*m_tpLastMonster;
	float			m_fMinResultValue;
	float			m_fMaxResultValue;
	char			m_caName[64];

public:

	virtual	void	vfLoadEF(const char *caFileName, CBaseFunction **fpaBaseFunctions) {};
	virtual float	ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions) = 0;
					CBaseFunction() {m_caName[0] = 0;};
	
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

class CDistanceFunction : public CBaseFunction {

public:
	CDistanceFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 150.0;
		strcat(m_caName,"Distance");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		return(m_fLastValue = tpCustomMonster->Position().distance_to(tpCustomMonster->m_tpCurrentEnemy->Position()));
	};
};
 
class CPersonalHealthFunction : public CBaseFunction {

public:
	CPersonalHealthFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalHealth");
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
 
class CPersonalMoraleFunction : public CBaseFunction {
	
public:
	CPersonalMoraleFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalMorale");
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
 
class CPersonalCreatureTypeFunction : public CBaseFunction {
	
public:
	CPersonalCreatureTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 22.0;
		strcat(m_caName,"PersonalCreatureType");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		switch (tpCustomMonster->SUB_CLS_ID) {
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
			case CLSID_AI_FRACTURE	: {
				m_fLastValue = 21;
				break;
			}
			case CLSID_AI_DOG_BLACK	: {
				m_fLastValue = 22;
				break;
			}
		}
		return(m_fLastValue);
	};
};
 
class CPersonalWeaponTypeFunction : public CBaseFunction {
	
	float ffGetTheBestWeapon(CCustomMonster *tpCustomMonster) 
	{
		DWORD dwBestWeapon = 2;
		for (int i=0; i<(int)tpCustomMonster->tpfGetWeapons()->getWeaponCount(); i++) {
			CWeapon *tpCustomWeapon = tpCustomMonster->tpfGetWeapons()->getWeaponByIndex(i);
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

public:
	CPersonalWeaponTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"PersonalWeaponType");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster;
		switch (tpCustomMonster->SUB_CLS_ID) {
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
				m_fLastValue =  ffGetTheBestWeapon(tpCustomMonster);
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
				m_fLastValue =  ffGetTheBestWeapon(tpCustomMonster);
				break;
			}
			case CLSID_AI_STALKER_DARK		: {
				m_fLastValue =  ffGetTheBestWeapon(tpCustomMonster);
				break;
			}
			case CLSID_AI_STALKER_MILITARY	: {
				m_fLastValue =  ffGetTheBestWeapon(tpCustomMonster);
				break;
			}
			case CLSID_AI_STALKER			: {
				m_fLastValue =  ffGetTheBestWeapon(tpCustomMonster);
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
		return(m_fLastValue);
	};
};
 
class CEnemyHealthFunction : public CBaseFunction {

public:
	CEnemyHealthFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyHealth");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster->m_tpCurrentEnemy))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster->m_tpCurrentEnemy;
		return(m_fLastValue = tpCustomMonster->pfPersonalHealth.ffGetValue(tpCustomMonster->m_tpCurrentEnemy,fpaBaseFunctions));
	};
};
 
class CEnemyMoraleFunction : public CBaseFunction {
	
public:
	CEnemyMoraleFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyMorale");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster->m_tpCurrentEnemy))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster->m_tpCurrentEnemy;
		return(m_fLastValue = tpCustomMonster->pfPersonalMorale.ffGetValue(tpCustomMonster->m_tpCurrentEnemy,fpaBaseFunctions));
	};
};
 
class CEnemyCreatureTypeFunction : public CBaseFunction {
	
public:
	CEnemyCreatureTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 22.0;
		strcat(m_caName,"EnemyCreatureType");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster->m_tpCurrentEnemy))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster->m_tpCurrentEnemy;
		return(m_fLastValue = tpCustomMonster->pfPersonalCreatureType.ffGetValue(tpCustomMonster->m_tpCurrentEnemy,fpaBaseFunctions));
	};
};
 
class CEnemyWeaponTypeFunction : public CBaseFunction {
	
public:
	CEnemyWeaponTypeFunction() {
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyWeaponType");
	}
	
	virtual float ffGetValue(CCustomMonster *tpCustomMonster, CBaseFunction **fpaBaseFunctions)
	{
		if ((m_dwLastUpdate == Level().timeServer()) && (m_tpLastMonster == tpCustomMonster->m_tpCurrentEnemy))
			return(m_fLastValue);
		m_dwLastUpdate = Level().timeServer();
		m_tpLastMonster = tpCustomMonster->m_tpCurrentEnemy;
		return(m_fLastValue = tpCustomMonster->pfPersonalWeaponType.ffGetValue(tpCustomMonster->m_tpCurrentEnemy,fpaBaseFunctions));
	};
};
 
#endif