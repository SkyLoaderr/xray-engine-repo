#include "stdafx.h"
#include "ai_monster_invisibility.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ���������� ����������� (��������)
///////////////////////////////////////////////////////////////////////////////////////////////

CMonsterInvisibility::CMonsterInvisibility()
{
	timeStartBlinking		= 0;
	timeLastBlink			= 0; 
	bCurVisibility			= flagVisibility	= true;

	timeStartRestore		= 0;
	timeStartInvisible		= 0;

	bBlinking				= false;
}

void CMonsterInvisibility::Load(LPCTSTR section)
{
	timeBlinking		= pSettings->r_u32(section,"BlinkTime");
	timeBlinkInterval	= pSettings->r_u32(section,"BlinkMicroInterval");
	timeRestoreInterval	= pSettings->r_u32(section,"RestoreInterval");
	timeInvisibilityMin	= pSettings->r_u32(section,"InvisibilityTimeMin");;
	timeInvisibilityMax	= pSettings->r_u32(section,"InvisibilityTimeMax");;
}

bool CMonsterInvisibility::Switch (bool bVis) 
{
	if ((bVis == false) && !IsInvisibilityReady()) return false;		// ���� ��� �� �������������
	if (bBlinking) return false;										// ���� � ���������� ���������
	if (bVis == flagVisibility) return false;							// ���� ��� ��������� � ���� ���������

	timeStartBlinking			= timeCurrent;
	bCurVisibility				= flagVisibility;
	flagVisibility				= bVis;
	timeLastBlink				= 0; 
	timeBlinkIntervalCurrent	= timeBlinkInterval;

	bBlinking					= true;

	if (flagVisibility) timeStartRestore = timeCurrent; // ���� ������ ���� �����
	else {
		timeStartInvisible = timeCurrent;
		timeInvisibleInterval = ::Random.randI(timeInvisibilityMin,timeInvisibilityMax);
	}

	return true;
}	

bool CMonsterInvisibility::Update () 
{
	timeCurrent = Level().timeServer();

	if (timeStartBlinking + timeBlinking > timeCurrent) {
		if (timeLastBlink + timeBlinkInterval < timeCurrent) {
			// blink
			timeLastBlink = timeCurrent; 
			bCurVisibility = !bCurVisibility;

			timeBlinkIntervalCurrent = (TTime) (timeBlinkInterval - (float)(((timeBlinkInterval - 1) * (float)(timeCurrent - timeStartBlinking)) / timeBlinking));
		}
		return bCurVisibility;
	} else {
		bCurVisibility = flagVisibility;
		bBlinking = false;
	}

	// ���� ����� ����������� �����
	if (!flagVisibility && (timeStartInvisible + timeInvisibleInterval < timeCurrent)) Switch(true);

	return flagVisibility;
}

